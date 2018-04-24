// rdcut_dialog.cpp
//
// A widget to select a Rivendell Cut.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdlib.h>
#include <stdio.h>

#include <qpushbutton.h>
#include <qlabel.h>
#include <qsqlquery.h>
#include <qapplication.h>
#include <qeventloop.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QPixmap>

#include <rdapplication.h>
#include <rdcut_dialog.h>
#include <rdcart_search_text.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>
#include <rdadd_cart.h>
#include <rdprofile.h>
#include <rddb.h>
#include <rdconf.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"


RDCutDialog::RDCutDialog(QString *cutname,QString *filter,QString *group,
			 QString *schedcode,QString username,bool show_clear,
			 bool allow_add,bool exclude_tracks,QWidget *parent)
  : QDialog(parent,"",true)
{
  cut_cutname=cutname;
  cut_exclude_tracks=exclude_tracks;
  cut_group=group;
  cut_schedcode=schedcode;
  cut_username=username;
  cut_allow_clear=show_clear;
  cut_filter_mode=rda->station()->filterMode();

  if(filter==NULL) {
    cut_filter=new QString();
    local_filter=true;
  }
  else {
    cut_filter=filter;
    local_filter=false;
  }

  setCaption(tr("Select Cut"));

  //
  // Create Icons
  //
  cut_playout_map=new QPixmap(play_xpm);
  cut_macro_map=new QPixmap(rml5_xpm);

  //
  // Generate Fonts
  //
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont progress_font("Helvetica",16,QFont::Bold);
  progress_font.setPixelSize(16);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Progress Dialog
  //
  cut_progress_dialog=
    new Q3ProgressDialog(tr("Please Wait..."),"Cancel",10,this,
			"cut_progress_dialog",false,
			Qt::WStyle_Customize|Qt::WStyle_NormalBorder);
  cut_progress_dialog->setCaption(" ");
  QLabel *label=new QLabel(tr("Please Wait..."),cut_progress_dialog);
  label->setAlignment(Qt::AlignCenter);
  label->setFont(progress_font);
  cut_progress_dialog->setLabel(label);
  cut_progress_dialog->setCancelButton(NULL);
  cut_progress_dialog->setMinimumDuration(2000);

  //
  // Filter Selector
  //
  cut_filter_edit=new QLineEdit(this);
  label=new QLabel(cut_filter_edit,tr("Cart Filter:"),this);
  label->setGeometry(10,10,85,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label->setFont(label_font);
  connect(cut_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Filter Search Button
  //
  cut_search_button=new QPushButton(this);
  cut_search_button->setGeometry(sizeHint().width()-140,8,60,24);
  cut_search_button->setText(tr("&Search"));
  cut_search_button->setFont(label_font);
  connect(cut_search_button,SIGNAL(clicked()),this,SLOT(searchButtonData()));

  //
  // Filter Clear Button
  //
  cut_clear_button=new QPushButton(this);
  cut_clear_button->setGeometry(sizeHint().width()-70,8,60,24);
  cut_clear_button->setFont(label_font);
  cut_clear_button->setText(tr("C&lear"));
  connect(cut_clear_button,SIGNAL(clicked()),this,SLOT(clearData()));

  //
  // Group Selector
  //
  cut_group_box=new QComboBox(this);
  cut_group_box->setGeometry(100,40,140,20);
  label=new QLabel(cut_filter_edit,tr("Group:"),this);
  label->setGeometry(10,40,85,20);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  label->setFont(label_font);
  connect(cut_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Scheduler Code Selector
  //
  cut_schedcode_box=new QComboBox(this);
  cut_schedcode_box->setGeometry(380,40,sizeHint().width()-390,20);
  cut_schedcode_label=new QLabel(cut_schedcode_box,tr("Scheduler Code:"),this);
  cut_schedcode_label->setGeometry(260,40,115,20);
  cut_schedcode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  cut_schedcode_label->setFont(label_font);
  connect(cut_schedcode_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupActivatedData(const QString &)));

  //
  // Search Limit Checkbox
  //
  cart_limit_box=new QCheckBox(this);
  cart_limit_box->setGeometry(100,72,15,15);
  cart_limit_box->setChecked(true);
  label=new QLabel(cart_limit_box,tr("Show Only First")+
		   QString().sprintf(" %d ",
				     RD_LIMITED_CART_SEARCH_QUANTITY)+tr("Matches"),this);
  label->setGeometry(120,70,300,20);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  label->setFont(label_font);
  connect(cart_limit_box,SIGNAL(stateChanged(int)),
	  this,SLOT(limitChangedData(int)));

  //
  // Cart List
  //
  cut_cart_list=new RDListView(this);
  cut_cart_list->setGeometry(10,120,300,200);
  cut_cart_list->setAllColumnsShowFocus(true);
  cut_cart_list->setItemMargin(5);
  connect(cut_cart_list,SIGNAL(selectionChanged()),
	  this,SLOT(selectionChangedData()));
  connect(cut_cart_list,SIGNAL(clicked(Q3ListViewItem *)),
	  this,SLOT(cartClickedData(Q3ListViewItem *)));
  label=new QLabel(cut_cart_list,tr("Carts"),this);
  label->setGeometry(15,100,100,20);
  label->setFont(label_font);
  cut_cart_list->addColumn("");
  cut_cart_list->setColumnAlignment(0,Qt::AlignHCenter);
  cut_cart_list->addColumn(tr("NUMBER"));

  cut_cart_list->setColumnAlignment(1,Qt::AlignHCenter);
  cut_cart_list->addColumn(tr("TITLE"));
  cut_cart_list->setColumnAlignment(2,Qt::AlignLeft);

  cut_cart_list->addColumn(tr("GROUP"));
  cut_cart_list->setColumnAlignment(3,Qt::AlignCenter);

  //
  // Cut List
  //
  cut_cut_list=new Q3ListView(this);
  cut_cut_list->setGeometry(320,120,sizeHint().width()-330,200);
  cut_cut_list->setAllColumnsShowFocus(true);
  cut_cut_list->setItemMargin(5);
  label=new QLabel(cut_cut_list,tr("Cuts"),this);
  label->setGeometry(325,100,100,20);
  label->setFont(label_font);
  cut_cut_list->addColumn(tr("DESCRIPTION"));
  cut_cut_list->setColumnAlignment(0,Qt::AlignLeft);

  cut_cut_list->addColumn(tr("NUMBER"));
  cut_cut_list->setColumnAlignment(1,Qt::AlignLeft);


  QPushButton *button=NULL;

  //
  // Add Button
  //
  button=new QPushButton(tr("&Add New\nCart"),this);
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(label_font);
  connect(button,SIGNAL(clicked()),this,SLOT(addButtonData()));
  if(!allow_add) {
    button->hide();
  }

  //
  // Clear Button
  //
  button=new QPushButton(tr("&Clear"),this);
  button->setFont(label_font);
  connect(button,SIGNAL(clicked()),this,SLOT(clearButtonData()));
  if(!show_clear) {
    button->hide();
  }
  if(allow_add) {
    button->setGeometry(100,sizeHint().height()-60,80,50);
  }
  else {
    button->setGeometry(10,sizeHint().height()-60,80,50);
  }

  //
  // OK Button
  //
  cut_ok_button=new QPushButton(tr("&OK"),this);
  cut_ok_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  cut_ok_button->setFont(label_font);
  cut_ok_button->setDefault(true);
  connect(cut_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  cut_cancel_button=new QPushButton(tr("&Cancel"),this);
  cut_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cut_cancel_button->setFont(label_font);
  connect(cut_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  if(cut_cutname->isEmpty()) {
    cut_ok_button->setDisabled(true);
  }
  switch(cut_filter_mode) {
    case RDStation::FilterAsynchronous:
      cut_search_button->setDefault(true);
      cut_filter_edit->setGeometry(100,10,sizeHint().width()-250,20);
      break;

    case RDStation::FilterSynchronous:
      cut_ok_button->setDefault(true);
      cut_search_button->hide();
      cut_filter_edit->setGeometry(100,10,sizeHint().width()-180,20);
  }
  BuildGroupList();
  cut_filter_edit->setText(*cut_filter);
  RefreshCarts();
  RefreshCuts();
  SelectCut(*cut_cutname);
}


RDCutDialog::~RDCutDialog()
{
  if(local_filter) {
    delete cut_filter;
  }
}


QSize RDCutDialog::sizeHint() const
{
  return QSize(550,400);
}


QSizePolicy RDCutDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDCutDialog::exec()
{
  LoadState();
  return QDialog::exec();
}


void RDCutDialog::filterChangedData(const QString &str)
{
  cut_search_button->setEnabled(true);
  switch(cut_filter_mode) {
    case RDStation::FilterSynchronous:
      searchButtonData();
      break;

    case RDStation::FilterAsynchronous:
      break;
  }
}


void RDCutDialog::clearData()
{
  cut_filter_edit->clear();
  filterChangedData("");
}


void RDCutDialog::groupActivatedData(const QString &str)
{
  filterChangedData("");
}


void RDCutDialog::limitChangedData(int state)
{
  filterChangedData("");
}


void RDCutDialog::cartClickedData(Q3ListViewItem *)
{
  cut_ok_button->setEnabled(true);
}


void RDCutDialog::selectionChangedData()
{
  RefreshCuts();
  if(cut_cut_list->firstChild()!=NULL) {
    cut_cut_list->setSelected(cut_cut_list->firstChild(),true);
  }
}


void RDCutDialog::searchButtonData()
{
  if(cut_filter_edit->text().isEmpty()) {
    cut_clear_button->setDisabled(true);
  }
  else {
    cut_clear_button->setEnabled(true);
  }
  RefreshCarts();
  RefreshCuts();
}


void RDCutDialog::clearButtonData()
{
  RDListViewItem *item=(RDListViewItem *)cut_cart_list->selectedItem();
  if(item!=NULL) {
    cut_cart_list->setSelected(item,false);
  }
  cut_cut_list->clear();
}


void RDCutDialog::addButtonData()
{
  QString cart_group=cut_group_box->currentText();
  RDCart::Type cart_type=RDCart::Audio;
  QString cart_title;
  QString sql;
  RDSqlQuery *q;
  int cart_num=-1;

  RDAddCart *add_cart=new RDAddCart(&cart_group,&cart_type,&cart_title,
				    cut_username,rda->system(),this);
  if((cart_num=add_cart->exec())<0) {
    delete add_cart;
    return;
  }
  sql=QString("insert into CART set ")+
    QString().sprintf("NUMBER=%d,",cart_num)+
    QString().sprintf("TYPE=%d,",cart_type)+
    "GROUP_NAME=\""+RDEscapeString(cart_group)+"\","+
    "TITLE=\""+RDEscapeString(cart_title)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("insert into CUTS set ")+
    "CUT_NAME=\""+RDCut::cutName(cart_num,1)+"\","+
    QString().sprintf("CART_NUMBER=%d,",cart_num)+
    "DESCRIPTION=\"Cut 001\"";
  q=new RDSqlQuery(sql);
  delete q;

  RDListViewItem *item=new RDListViewItem(cut_cart_list);
  item->setPixmap(0,*cut_playout_map);
  item->setText(1,QString().sprintf("%06d",cart_num));
  item->setText(2,cart_title);
  cut_cart_list->setSelected(item,true);
  cut_cart_list->ensureItemVisible(item);
  cut_ok_button->setEnabled(true);

  delete add_cart;
}


void RDCutDialog::okData()
{
  RDListViewItem *cart_item=(RDListViewItem *)cut_cart_list->selectedItem();
  Q3ListViewItem *cut_item=cut_cut_list->selectedItem();
  if((cart_item==NULL)||(cut_item==NULL)) {
    if(cut_allow_clear) {
      *cut_cutname="";
      if(!local_filter) {
	*cut_filter=cut_filter_edit->text();
      }
      if(cut_group!=NULL) {
	*cut_group=cut_group_box->currentText();
      }
      if(cut_schedcode!=NULL) {
	*cut_schedcode=cut_schedcode_box->currentText();
      }
      SaveState();
      done(0);
    }
    return;
  }

  *cut_cutname=cart_item->text(1)+QString("_")+cut_item->text(1);
  if(!local_filter) {
    *cut_filter=cut_filter_edit->text();
  }
  if(cut_group!=NULL) {
    *cut_group=cut_group_box->currentText();
  }
  SaveState();
  done(0);
}


void RDCutDialog::cancelData()
{
  SaveState();
  done(1);
}


void RDCutDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RDCutDialog::RefreshCarts()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *l;
  QString group=cut_group_box->currentText();

  if(!cut_cutname->isEmpty()) {
  }
  cut_cart_list->clear();
  if(group==QString(tr("ALL"))) {
    group="";
  }
  QString schedcode="";
  if(cut_schedcode_box->currentText()!=tr("ALL")) {
    schedcode=cut_schedcode_box->currentText();
  }
  sql=QString("select ")+
    "CART.NUMBER,"+
    "CART.TITLE,"+
    "CART.GROUP_NAME,"+
    "GROUPS.COLOR,"+
    "CART.TYPE "+
    "from CART left join GROUPS "+
    "on CART.GROUP_NAME=GROUPS.NAME where "+
    "("+RDCartSearchText(cut_filter_edit->text(),group,schedcode,false)+")&&"+
    QString().sprintf("((CART.TYPE=%u))",RDCart::Audio);
  if(cut_exclude_tracks) {
    sql+="&&(CART.OWNER is null)";
  }
  if(cart_limit_box->isChecked()) {
    sql+=QString().sprintf(" limit %d",RD_LIMITED_CART_SEARCH_QUANTITY);
  }
  q=new RDSqlQuery(sql);
  int step=0;
  int count=0;
  cut_progress_dialog->setTotalSteps(q->size()/RDCUT_DIALOG_STEP_SIZE);
  cut_progress_dialog->setProgress(0);
  while(q->next()) {
    l=new RDListViewItem(cut_cart_list);
    switch((RDCart::Type)q->value(4).toUInt()) {
    case RDCart::Audio:
      l->setPixmap(0,*cut_playout_map);
      break;

    case RDCart::All:
    case RDCart::Macro:
      break;
    }
    l->setText(1,QString().sprintf("%06u",q->value(0).toUInt()));   // Number

    l->setText(2,q->value(1).toString());     // Title
    l->setText(3,q->value(2).toString());     // Group
    l->setTextColor(3,q->value(3).toString(),QFont::Bold);
    if(count++>RDCUT_DIALOG_STEP_SIZE) {
      cut_progress_dialog->setProgress(++step);
      count=0;
      qApp->processEvents(QEventLoop::ExcludeUserInput);
    }
  }
  cut_progress_dialog->reset();
  delete q;
  cut_search_button->setDisabled(true);
}



void RDCutDialog::RefreshCuts()

{
  QString sql;
  RDSqlQuery *q;
  Q3ListViewItem *l;
  Q3ListViewItem *cart_item=cut_cart_list->selectedItem();

  cut_cut_list->clear();
  if(cart_item==NULL) {
    return;
  }
  sql=QString("select ")+
    "DESCRIPTION,"+
    "CUT_NAME "+
    "from CUTS where "+
    "CART_NUMBER="+cart_item->text(1);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new Q3ListViewItem(cut_cut_list);
    l->setText(0,q->value(0).toString());     // Description
    l->setText(1,QString().sprintf("%03u",    // Cut Number
				   q->value(1).toString().right(3).toUInt()));
  }
  delete q;
}


void RDCutDialog::SelectCut(QString cutname)
{
  QString cart=cutname.left(6);
  QString cut=cutname.right(3);
  Q3ListViewItem *item=cut_cart_list->findItem(cart,1);
  if(item!=NULL) {
    cut_cart_list->setSelected(item,true);
    cut_cart_list->ensureItemVisible(item);
  }
  RefreshCuts();
  item=cut_cut_list->findItem(cut,1);
  if(item!=NULL) {
    cut_cut_list->setSelected(item,true);
  }
}


void RDCutDialog::BuildGroupList()
{
  QString sql;
  RDSqlQuery *q;
  
  cut_group_box->clear();
  cut_group_box->insertItem(tr("ALL"));
  if(cut_username.isEmpty()) {
    sql="select NAME from GROUPS order by NAME desc";
  }
  else {
    sql=QString("select GROUP_NAME from USER_PERMS where ")+
      "USER_NAME=\""+RDEscapeString(cut_username)+"\" "+
      "order by GROUP_NAME desc";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cut_group_box->insertItem(q->value(0).toString(),true);
  }
  delete q;

  //
  // Preselect Group
  //
  if(cut_group!=NULL) {
    for(int i=0;i<cut_group_box->count();i++) {
      if(*cut_group==cut_group_box->text(i)) {
	cut_group_box->setCurrentItem(i);
	return;
      }
    }
  }

  //
  // Scheduler Codes
  //
  cut_schedcode_box->clear();
  cut_schedcode_box->insertItem(tr("ALL"));
  sql="select CODE from SCHED_CODES";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    cut_schedcode_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Preselect Scheduler Code
  //
  if(cut_schedcode!=NULL) {
    for(int i=0;i<cut_schedcode_box->count();i++) {
      if(*cut_schedcode==cut_schedcode_box->text(i)) {
	cut_schedcode_box->setCurrentItem(i);
	return;
      }
    }
  }
}

QString RDCutDialog::StateFile() {
  bool home_found = false;
  QString home = RDGetHomeDir(&home_found);
  if (home_found) {
    return QString().sprintf("%s/.rdcartdialog",(const char *)home);
  } else {
    return NULL;
  }
}

void RDCutDialog::LoadState()
{
  QString state_file=StateFile();
  if(state_file.isNull()) {
    return;
  }

  RDProfile *p=new RDProfile();
  p->setSource(state_file);

  bool value_read=false;
  cart_limit_box->setChecked(p->boolValue("RDCartDialog", "LimitSearch", true, &value_read));

  delete p;
}


void RDCutDialog::SaveState()
{
  FILE *f=NULL;

  QString state_file=StateFile();
  if(state_file.isNull()) {
    return;
  }

  if((f=fopen(state_file,"w"))==NULL) {
    return;
  }
  fprintf(f,"[RDCartDialog]\n");
  if(cart_limit_box->isChecked()) {
    fprintf(f,"LimitSearch=Yes\n");
  }
  else {
    fprintf(f,"LimitSearch=No\n");
  }
  fclose(f);
}
