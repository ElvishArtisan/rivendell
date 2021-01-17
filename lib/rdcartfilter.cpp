// rdcartfilter.cpp
//
// Filter widget for picking Rivendell carts.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QResizeEvent>

#include "rdapplication.h"
#include "rdcart_search_text.h"
#include "rdcartfilter.h"
#include "rdescape_string.h"

RDCartFilter::RDCartFilter(QWidget *parent)
  : RDWidget(parent)
{
  d_show_cart_type=RDCart::All;
  d_user_is_admin=false;

  d_filter_edit=new QLineEdit(this);
  d_filter_label=new QLabel(d_filter_edit,tr("Filter:"),this);
  d_filter_label->setFont(labelFont());
  d_filter_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  connect(d_filter_edit,SIGNAL(returnPressed()),
	  this,SLOT(searchClickedData()));

  //
  // Filter Search Button
  //
  d_search_button=new QPushButton(tr("&Search"),this);
  d_search_button->setFont(buttonFont());
  connect(d_search_button,SIGNAL(clicked()),this,SLOT(searchClickedData()));
  switch(rda->station()->filterMode()) {
  case RDStation::FilterSynchronous:
    d_search_button->hide();
    break;

  case RDStation::FilterAsynchronous:
    break;
  }

  //
  // Filter Clear Button
  //
  d_clear_button=new QPushButton(tr("&Clear"),this);
  d_clear_button->setFont(buttonFont());
  d_clear_button->setDisabled(true);
  connect(d_clear_button,SIGNAL(clicked()),this,SLOT(clearClickedData()));

  //
  // Group Filter
  //
  d_group_box=new QComboBox(this);
  d_group_label=new QLabel(d_group_box,tr("Group:"),this);
  d_group_label->setFont(labelFont());
  d_group_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupChangedData(const QString &)));

  //
  // Scheduler Codes Filter
  //
  d_codes_box=new QComboBox(this);
  d_codes_label=new QLabel(d_codes_box,tr("Scheduler Code:"),this);
  d_codes_label->setFont(labelFont());
  d_codes_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_codes_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupChangedData(const QString &)));

  //
  // Scheduler Codes2 Filter
  //
  d_codes2_box=new QComboBox(this);
  d_codes2_label=new QLabel(d_codes2_box,tr("And Scheduler Code:"),this);
  d_codes2_label->setFont(labelFont());
  d_codes2_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_codes2_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupChangedData(const QString &)));

  //
  // Results Counter
  //
  d_matches_edit=new QLineEdit(this);
  d_matches_edit->setReadOnly(true);
  d_matches_label=new QLabel(d_matches_edit,tr("Matching Carts:"),this);
  d_matches_label->setFont(labelFont());

  //
  // Show Allow Cart Drags Checkbox
  //
  d_allowdrag_box=new QCheckBox(this);
  d_allowdrag_box->setChecked(false);
  d_allowdrag_label=
    new QLabel(d_allowdrag_box,tr("Allow Cart Dragging"),this);
  d_allowdrag_label->setFont(labelFont());
  d_allowdrag_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_allowdrag_box,SIGNAL(stateChanged(int)),
	  this,SLOT(dragsChangedData(int)));
  if(!rda->station()->enableDragdrop()) {
    d_allowdrag_box->hide();
    d_allowdrag_label->hide();
  }

  //
  // Show Audio Carts Checkbox
  //
  d_showaudio_check=new QCheckBox(this);
  d_showaudio_check->setChecked(true);
  d_showaudio_label=new QLabel(d_showaudio_check,tr("Show Audio Carts"),this);
  d_showaudio_label->setFont(labelFont());
  d_showaudio_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showaudio_check,SIGNAL(stateChanged(int)),
	  this,SLOT(checkChangedData(int)));

  //
  // Show Macro Carts Checkbox
  //
  d_showmacro_check=new QCheckBox(this);
  d_showmacro_check->setChecked(true);
  d_showmacro_label=new QLabel(d_showmacro_check,tr("Show Macro Carts"),this);
  d_showmacro_label->setFont(labelFont());
  d_showmacro_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showmacro_check,SIGNAL(stateChanged(int)),
	  this,SLOT(checkChangedData(int)));

  //
  // Show Cart Notes Checkbox
  //
  d_shownotes_box=new QCheckBox(this);
  d_shownotes_box->setChecked(true);
  d_shownotes_label=
    new QLabel(d_shownotes_box,tr("Show Note Bubbles"),this);
  d_shownotes_label->setFont(labelFont());
  d_shownotes_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Show Matches Checkbox
  //
  d_showmatches_box=new QCheckBox(this);
  d_showmatches_label=
    new QLabel(d_showmatches_box,tr("Show Only First ")+
	       QString().sprintf("%d",RD_LIMITED_CART_SEARCH_QUANTITY)+
	       tr(" Matches"),this);
  d_showmatches_label->setFont(labelFont());
  d_showmatches_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showmatches_box,SIGNAL(stateChanged(int)),
	  this,SLOT(searchLimitChangedData(int)));

  //
  // Load Data
  //
  switch(rda->libraryConf()->limitSearch()) {
  case RDLibraryConf::LimitNo:
    d_showmatches_box->setChecked(false);
    break;

  case RDLibraryConf::LimitYes:
    d_showmatches_box->setChecked(true);
    break;

  case RDLibraryConf::LimitPrevious:
    d_showmatches_box->setChecked(rda->libraryConf()->searchLimited());
    break;
  }
}


RDCartFilter::~RDCartFilter()
{
}


QSize RDCartFilter::sizeHint() const
{
  return QSize(1010,90);
}


QSizePolicy RDCartFilter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}


void RDCartFilter::loadConfig(RDProfile *p)
{
  d_shownotes_box->setChecked(p->boolValue("RDLibrary","ShowNoteBubbles",true));
  d_allowdrag_box->
    setChecked(p->boolValue("RDLibrary","AllowCartDragging",false));
}


void RDCartFilter::saveConfig(FILE *f) const
{
  if(d_shownotes_box->isChecked()) {
    fputs("Yes\n",f);
  }
  else {
    fputs("No\n",f);
  }
  fprintf(f,"AllowCartDragging=");
  if(d_allowdrag_box->isChecked()) {
    fprintf(f,"Yes\n");
  }
  else {
    fprintf(f,"No\n");
  }
}


QString RDCartFilter::filterSql(const QStringList &and_fields) const
{
  QString sql=" where ";
  sql+=RDCartFilter::typeFilter(d_showaudio_check->isChecked(),
				d_showmacro_check->isChecked(),
				d_show_cart_type);
  //  sql+=RDCartFilter::phraseFilter(d_filter_edit->text().trimmed(),true);
  QStringList groups;
  for(int i=0;i<d_group_box->count();i++) {
    groups.push_back(d_group_box->text(i));
  }
  sql+=RDCartFilter::groupFilter(d_group_box->currentText(),groups);
  sql=sql.left(sql.length()-2);  // Remove the final "&&"
  sql+="order by CART.NUMBER ";
  if(d_showmatches_box->isChecked()) {
    sql+=QString().sprintf("limit %d ",RD_LIMITED_CART_SEARCH_QUANTITY);
  }

  //  printf("FILTER SQL: %s\n",sql.toUtf8().constData());

  return sql;

  /*

  if(type_filter.isEmpty()) {
    return QString("where CART.NUMBER=0 ");
  }

  QStringList schedcodes;
  if(d_codes_box->currentText()!=tr("ALL")) {
    schedcodes << d_codes_box->currentText();
  }
  if(d_codes2_box->currentText()!=tr("ALL")) {
    schedcodes << d_codes2_box->currentText();
  }
  if(d_group_box->currentText()==QString(tr("ALL"))) {
    if(d_user_is_admin) {
      // sql=" where ";
    }
    else {
      sql+=RDAllCartSearchText(d_filter_edit->text(),schedcodes,
			       rda->user()->name(),true)+" "+type_filter;
    }
  }
  else {
    sql+=RDCartSearchText(d_filter_edit->text(),d_group_box->currentText(),
		       schedcodes,true)+" "+type_filter;      
  }
  for(int i=0;i<and_fields.size();i++) {
    sql+="&& "+and_fields.at(i)+" ";
  }
  //  sql+=" group by CART.NUMBER order by CART.NUMBER ";
  sql+=" order by CART.NUMBER ";
  if(d_showmatches_box->isChecked()) {
    sql+=QString().sprintf("limit %d ",RD_LIMITED_CART_SEARCH_QUANTITY);
  }

  //  printf("SQL: %s\n",sql.toUtf8().constData());

  return sql;
  */
}


QString RDCartFilter::filterText() const
{
  return d_filter_edit->text();
}


QString RDCartFilter::selectedGroup() const
{
  return d_group_box->currentText();
}


QString RDCartFilter::selectedSchedCode() const
{
  return d_codes_box->currentText();
}


bool RDCartFilter::dragEnabled() const
{
  return d_allowdrag_box->isChecked();
}


RDCart::Type RDCartFilter::showCartType() const
{
  return d_show_cart_type;
}


void RDCartFilter::setShowCartType(RDCart::Type type)
{
  if(type!=d_show_cart_type) { 
    if(type==RDCart::All) {
      d_showaudio_check->show();
      d_showaudio_label->show();
      d_showmacro_check->show();
      d_showmacro_label->show();
    }
    else {
      d_showaudio_check->hide();
      d_showaudio_label->hide();
      d_showmacro_check->hide();
      d_showmacro_label->hide();
    }
    d_show_cart_type=type;

    emit filterChanged(filterSql());
  }
}


bool RDCartFilter::limitSearch() const
{
  return d_showmatches_box->isChecked();
}


void RDCartFilter::setLimitSearch(bool state)
{
  d_showmatches_box->setChecked(state);
}


bool RDCartFilter::userIsAdmin() const
{
  return d_user_is_admin;
}


void RDCartFilter::setUserIsAdmin(bool state)
{
  if(state!=d_user_is_admin) {
    d_user_is_admin=state;
    changeUser();
  }
}


void RDCartFilter::setFilterText(const QString &str)
{
  d_filter_edit->setText(str);
  filterChangedData(str);
}


void RDCartFilter::setSelectedGroup(const QString &grpname)
{
  for(int i=0;i<d_group_box->count();i++) {
    if(d_group_box->text(i)==grpname) {
      d_group_box->setCurrentItem(i);
      groupChangedData(d_group_box->currentText());
    }
  }
}


void RDCartFilter::setMatchCount(int matches)
{
  d_matches_edit->setText(QString().sprintf("%d",matches));
}


void RDCartFilter::changeUser()
{
  QString sql;
  RDSqlQuery *q;

  d_group_box->clear();
  d_group_box->insertItem(tr("ALL"));
  if(d_user_is_admin) {
    sql=QString("select NAME from GROUPS order by NAME ");
  }
  else {
    sql=QString("select GROUP_NAME from USER_PERMS where ")+
      "USER_NAME=\""+RDEscapeString(rda->user()->name())+"\" "+
      "order by GROUP_NAME";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_box->insertItem(q->value(0).toString());
  }
  delete q;

  d_codes_box->clear();
  d_codes_box->insertItem(tr("ALL"));
  d_codes2_box->clear();
  d_codes2_box->insertItem(tr("ALL"));
  sql=QString().sprintf("select CODE from SCHED_CODES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_codes_box->insertItem(q->value(0).toString());
    d_codes2_box->insertItem(q->value(0).toString());
  }
  delete q;
  d_search_button->setDisabled(true);
  groupChangedData(d_group_box->currentText());
}


void RDCartFilter::filterChangedData(const QString &str)
{
  d_search_button->setEnabled(true);
  if(rda->station()->filterMode()!=RDStation::FilterSynchronous) {
    return;
  }
  searchClickedData();
}


void RDCartFilter::searchClickedData()
{
  d_search_button->setDisabled(true);
  if(d_filter_edit->text().isEmpty()) {
    d_clear_button->setDisabled(true);
  }
  else {
    d_clear_button->setEnabled(true);
  }
  emit filterChanged(filterSql());
}


void RDCartFilter::clearClickedData()
{
  d_filter_edit->clear();
  filterChangedData("");
}


void RDCartFilter::groupChangedData(const QString &str)
{
  if(str!=tr("ALL")) {
    emit selectedGroupChanged(str);
  }
  filterChangedData("");
}


void RDCartFilter::checkChangedData(int n)
{
  filterChangedData("");
}


void RDCartFilter::dragsChangedData(int n)
{
  emit dragEnabledChanged((bool)n);
}


void RDCartFilter::searchLimitChangedData(int state)
{
  rda->libraryConf()->setSearchLimited(state);
  filterChangedData("");
}


void RDCartFilter::resizeEvent(QResizeEvent *e)
{
  switch(rda->station()->filterMode()) {
  case RDStation::FilterSynchronous:
    d_filter_edit->setGeometry(70,10,e->size().width()-170,20);
    break;

  case RDStation::FilterAsynchronous:
    d_search_button->setGeometry(e->size().width()-180,10,80,50);
    d_filter_edit->setGeometry(70,10,e->size().width()-260,20);
    break;
  }
  d_clear_button->setGeometry(e->size().width()-90,10,80,50);
  d_filter_label->setGeometry(10,10,55,20);
  d_group_label->setGeometry(10,40,55,20);
  d_group_box->setGeometry(70,40,100,20);
  d_codes_label->setGeometry(175,40,115,20);
  d_codes_box->setGeometry(295,40,100,20);
  d_codes2_label->setGeometry(410,40,130,20);
  d_codes2_box->setGeometry(545,40,100,20);
  d_matches_label->setGeometry(660,40,100,20);
  d_matches_edit->setGeometry(765,40,55,20);
  d_showmatches_label->setGeometry(760,66,200,20);
  d_showmatches_box->setGeometry(740,68,15,15);
  d_allowdrag_label->setGeometry(580,66,130,20);
  d_allowdrag_box->setGeometry(560,68,15,15);
  d_showaudio_label->setGeometry(90,66,130,20);
  d_showaudio_check->setGeometry(70,68,15,15);
  d_showmacro_label->setGeometry(250,66,130,20);
  d_showmacro_check->setGeometry(230,68,15,15);
  d_shownotes_label->setGeometry(410,66,130,20);
  d_shownotes_box->setGeometry(390,68,15,15);
}


QString RDCartFilter::phraseFilter(const QString &phrase, bool incl_cuts)
{
  QString sql="";

  if(phrase.isEmpty()) {
    sql=" &&";
  }
  else {
    sql+=QString(" (")+
      "(CART.TITLE like \"%%\")||"+
      "(CART.ARTIST like \"%%\")||"+
      "(CART.CLIENT like \"%%\")||"+
      "(CART.AGENCY like \"%%\")||"+
      "(CART.ALBUM like \"%%\")||"+
      "(CART.LABEL like \"%%\")||"+
      "(CART.NUMBER like \"%%\")||"+
      "(CART.PUBLISHER like \"%%\")||"+
      "(CART.COMPOSER like \"%%\")||"+
      "(CART.CONDUCTOR like \"%%\")||"+
      "(CART.SONG_ID like \"%%\")||"+
      "(CART.USER_DEFINED like \"%%\")";
    if(incl_cuts) {
      sql+=QString("||(CUTS.ISCI like \"%%\")")+
	"||(CUTS.ISRC like \"%%\")"+
	"||(CUTS.DESCRIPTION like \"%%\")"+
	"||(CUTS.OUTCUE like \"%%\")";
    }
    sql+=") &&";
  }

  return sql;
}


QString RDCartFilter::groupFilter(const QString &group,
				  const QStringList &groups)
{
  QString sql=" (";

  if(group==tr("ALL")) {
    for(int i=1;i<groups.size();i++) {
      sql+="(CART.GROUP_NAME=\""+RDEscapeString(groups.at(i))+"\")||";
    }
    sql=sql.left(sql.length()-2);
  }
  else {
    sql+="CART.GROUP_NAME=\""+RDEscapeString(group)+"\"";
  }

  sql+=") &&";

  return sql;
}


QString RDCartFilter::typeFilter(bool incl_audio,bool incl_macro,
				    RDCart::Type mask)
{
  QString sql;

  switch(mask) {
  case RDCart::Audio:
    sql="((CART.TYPE=1)||(CART.TYPE=3)) &&";
    break;

  case RDCart::Macro:
    sql="(CART.TYPE=2) &&";
    break;

  case RDCart::All:
    if(incl_audio) {
      if(incl_macro) {
	sql="((CART.TYPE=1)||(CART.TYPE=2)||(CART.TYPE=3)) &&";
      }
      else {
	sql="((CART.TYPE=1)||(CART.TYPE=3)) &&";
      }
    }
    else {
      if(incl_macro) {
	sql="(CART.TYPE=2) &&";
      }
      else {
	sql="(CART.TYPE=0) &&";  // NEVER matches!
      }
    }
    break;
  }
  return sql;
}
