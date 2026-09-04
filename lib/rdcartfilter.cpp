// rdcartfilter.cpp
//
// Filter widget for picking Rivendell carts.
//
//   (C) Copyright 2021-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <QList>
#include <QResizeEvent>

#include "rdapplication.h"
#include "rdcart_search_text.h"
#include "rdcartfilter.h"
#include "rdescape_string.h"
#include "rd.h"

#define RDCARTFILTER_SECOND_SCHED_CODE_WIDTH 810

RDCartFilter::RDCartFilter(bool show_drag_box,bool user_is_admin,
			   QWidget *parent)
  : RDWidget(parent)
{
  d_show_cart_type=RDCart::All;
  d_show_track_carts=true;
  d_user_is_admin=user_is_admin;
  d_cart_model=NULL;
  d_show_drag_box=show_drag_box;

  d_group_model=new RDGroupListModel(true,false,user_is_admin,this);

  //
  // Filter Phrase
  //
  d_filter_edit=new QLineEdit(this);
  d_filter_label=new QLabel(tr("Filter:"),this);
  d_filter_label->setFont(labelFont());
  d_filter_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  connect(d_filter_edit,SIGNAL(returnPressed()),
	  this,SLOT(searchClickedData()));

  //
  // Filter Search Button
  //
  d_search_button=new QPushButton(tr("Search"),this);
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
  d_clear_button=new QPushButton(tr("Clear"),this);
  d_clear_button->setFont(buttonFont());
  d_clear_button->setDisabled(true);
  connect(d_clear_button,SIGNAL(clicked()),this,SLOT(clearClickedData()));

  //
  // Group Filter
  //
  d_group_box=new QComboBox(this);
  d_group_box->setModel(d_group_model);
  d_group_label=new QLabel(tr("Group:"),this);
  d_group_label->setFont(labelFont());
  d_group_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(groupChangedData(const QString &)));

  //
  // Scheduler Codes Filter
  //
  d_codes_box=new QComboBox(this);
  d_codes_label=new QLabel(tr("Scheduler Codes:"),this);
  d_codes_label->setFont(labelFont());
  d_codes_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  connect(d_codes_box,SIGNAL(activated(const QString &)),
	  this,SLOT(schedulerCodeChangedData(const QString &)));
  d_and_codes_box=new QComboBox(this);
  d_and_codes_box->setDisabled(true);
  d_and_codes_label=new QLabel("  "+tr("and")+"  ",this);
  d_and_codes_label->setFont(labelFont());
  d_and_codes_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  d_and_codes_box->insertItem(0,tr("[none]"));
  d_and_codes_label->setDisabled(true);
  connect(d_and_codes_box,SIGNAL(activated(const QString &)),
	  this,SLOT(andSchedulerCodeChangedData(const QString &)));

  //
  // Cart Matches Switch
  //
  d_showmatches_check=new QCheckBox(this);
  d_showmatches_label=new QLabel(tr("Show Only First 100 Carts"),this);
  d_showmatches_label->setFont(labelFont());
  connect(d_showmatches_check,SIGNAL(toggled(bool)),
	  this,SLOT(filterToggledData(bool)));

  //
  // Cart Matches Counter
  //
  d_matches_edit=new QLineEdit(this);
  d_matches_edit->setReadOnly(true);
  d_matches_label=new QLabel(tr("Matches")+":",this);
  d_matches_label->setFont(labelFont());

  //
  // Show Allow Cart Drags Checkbox
  //
  d_allowdrag_box=new QCheckBox(this);
  d_allowdrag_box->setChecked(false);
  d_allowdrag_label=new QLabel(tr("Allow Cart Dragging"),this);
  d_allowdrag_label->setFont(labelFont());
  d_allowdrag_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_allowdrag_box,SIGNAL(stateChanged(int)),
	  this,SLOT(dragsChangedData(int)));
  if((!d_show_drag_box)||(!rda->station()->enableDragdrop())) {
    d_allowdrag_box->hide();
    d_allowdrag_label->hide();
  }

  //
  // Show Type Selector
  //
  d_showtype_box=new QComboBox(this);
  d_showtype_label=new QLabel(tr("Cart Type")+":",this);
  d_showtype_label->setFont(labelFont());
  d_showtype_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showtype_box,SIGNAL(activated(int)),
	  this,SLOT(showtypeActivatedData(int)));
  d_showtype_box->insertItem((int)RDCart::All,tr("All carts"));
  d_showtype_box->insertItem((int)RDCart::Audio,tr("Audio carts"));
  d_showtype_box->insertItem((int)RDCart::Macro,tr("Macro carts"));
  
  //
  // Show Cart Notes Checkbox
  //
  d_shownotes_box=new QCheckBox(this);
  d_shownotes_box->setChecked(true);
  d_shownotes_label=
    new QLabel(tr("Show Note Bubbles"),this);
  d_shownotes_label->setFont(labelFont());
  d_shownotes_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(d_showmatches_check,SIGNAL(stateChanged(int)),
	  this,SLOT(searchLimitChangedData(int)));

  //
  // Load Data
  //
  switch(rda->libraryConf()->limitSearch()) {
  case RDLibraryConf::LimitNo:
    d_showmatches_check->setChecked(false);
    break;

  case RDLibraryConf::LimitYes:
    d_showmatches_check->setChecked(true);
    break;

  case RDLibraryConf::LimitPrevious:
    d_showmatches_check->setChecked(rda->libraryConf()->searchLimited());
    break;
  }

  d_last_search_time = QDateTime::currentMSecsSinceEpoch();
  search_filter_timer=new QTimer(this);
  connect(search_filter_timer,SIGNAL(timeout()),this,SLOT(searchClickedData()));
  search_filter_timer->start(100);
}


RDCartFilter::~RDCartFilter()
{
  delete d_group_model;
}


QSize RDCartFilter::sizeHint() const
{
  return QSize(1010,90);
}


QSizePolicy RDCartFilter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}


QString RDCartFilter::filterSql(const QStringList &and_fields) const
{
  QString sql=QString(" where ");

  for(int i=0;i<and_fields.size();i++) {
    sql+="("+and_fields.at(i)+") && ";
  }

  //
  // Cart Type Filter
  //
  sql+=RDCartFilter::typeFilter((RDCart::Type)(d_showtype_box->currentIndex()),
				d_show_cart_type);

  //
  // Full Text Filter
  //
  sql+=RDCartFilter::phraseFilter(d_filter_edit->text().trimmed(),true);

  //
  // Group Filter
  //
  QStringList groups;
  for(int i=0;i<d_group_box->count();i++) {
    groups.push_back(d_group_box->itemText(i));
  }
  if(groups.size()==0) {  // No groups selected, so force an empty selection
    return QString(" where `CART`.`NUMBER`<0");  // An impossibility
  }
  sql+=RDCartFilter::groupFilter(d_group_box->currentText(),groups);
  if(d_show_track_carts) {
    sql=sql.left(sql.length()-3);  // Remove the final "&& "
  }
  else {
    sql+="`CART`.`OWNER` is null ";
  }

  //
  // Schedule Code Filter
  //
  if(d_codes_box->currentIndex()>0) {
    sql+="&&(`CART_SCHED_CODES`.`SCHED_CODE`='"+
      RDEscapeString(d_codes_box->currentText())+"') ";
    if((d_and_codes_box->currentIndex()>0)&&
       (size().width()>RDCARTFILTER_SECOND_SCHED_CODE_WIDTH)) {
      //
      // Generate a list of carts with the second scheduler code
      //
      QString sub_sql;
      QString cart_sql=QString("select ")+
	"`CART_NUMBER` "+  // 00
	"from `CART_SCHED_CODES` where "+
	"`SCHED_CODE`='"+RDEscapeString(d_and_codes_box->currentText())+"'";
      RDSqlQuery *q=new RDSqlQuery(cart_sql);
      while(q->next()) {
	sub_sql+=
	  QString::asprintf("(`CART`.`NUMBER`=%u)||",q->value(0).toUInt());
      }
      delete q;
      if(!sub_sql.isEmpty()) {
	sql+="&&("+sub_sql.left(sub_sql.length()-2)+")";
      }
    }
  }

  return sql;
}


int RDCartFilter::cartLimit() const
{
  if(d_showmatches_check->isChecked()) {
    return RD_LIMITED_CART_SEARCH_QUANTITY;
  }
  return RD_MAX_CART_NUMBER+1;  // Effectively "unlimited"
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


bool RDCartFilter::showNoteBubbles() const
{
  return d_shownotes_box->isChecked();
}


RDCart::Type RDCartFilter::showCartType() const
{
  return d_show_cart_type;
}


void RDCartFilter::setShowCartType(RDCart::Type type)
{
  if(type!=d_show_cart_type) {
    d_showtype_box->setVisible(type==RDCart::All);
    d_showtype_label->setVisible(type==RDCart::All);
    d_show_cart_type=type;
  }
}


bool RDCartFilter::showTrackCarts() const
{
  return d_show_track_carts;
}


void RDCartFilter::setShowTrackCarts(bool state)
{
  if(state!=d_show_track_carts) {
    d_show_track_carts=state;
  }
}


bool RDCartFilter::limitSearch() const
{
  return d_showmatches_check->isChecked();
}


void RDCartFilter::setLimitSearch(bool state)
{
  d_showmatches_check->setChecked(state);
}


QString RDCartFilter::service() const
{
  return d_service;
}


void RDCartFilter::setService(const QString &svc)
{

  if(svc!=d_service) {
    d_service=svc;
    if(!d_service.isEmpty()) {
      LoadServiceGroups();
    }
  }
}


RDLibraryModel *RDCartFilter::model() const
{
  return d_cart_model;
}


void RDCartFilter::setDragEnabled(bool state)
{
  return d_allowdrag_box->setChecked(state);
}


void RDCartFilter::setShowNoteBubbles(bool state)
{
  d_shownotes_box->setChecked(state);
}


void RDCartFilter::setModel(RDLibraryModel *model)
{
  connect(this,SIGNAL(filterChanged(const QString &,int)),
	  model,SLOT(setFilterSql(const QString &,int)));
  connect(d_shownotes_box,SIGNAL(stateChanged(int)),
	  model,SLOT(setShowNotes(int)));
  connect(model,SIGNAL(rowCountChanged(int)),this,SLOT(setMatchCount(int)));
  model->setShowNotes(d_shownotes_box->isChecked());
}


void RDCartFilter::setFilterText(const QString &str)
{
  d_filter_edit->setText(str);
  filterChangedData(str);
}


void RDCartFilter::setSelectedGroup(const QString &grpname)
{
  for(int i=0;i<d_group_box->count();i++) {
    if(d_group_box->itemText(i)==grpname) {
      d_group_box->setCurrentIndex(i);
      groupChangedData(d_group_box->currentText());
    }
  }
}


void RDCartFilter::changeUser()
{
  QString sql;
  RDSqlQuery *q;

  if(d_service.isEmpty()) {
    d_group_model->changeUser();
    d_group_box->setCurrentIndex(0);
  }

  d_codes_box->clear();
  d_codes_box->insertItem(0,tr("ALL"));
  sql=QString::asprintf("select `CODE` from `SCHED_CODES` order by `CODE`");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_codes_box->insertItem(d_codes_box->count(),q->value(0).toString());
  }
  delete q;
  d_search_button->setDisabled(true);

  UpdateModel();
}


void RDCartFilter::filterChangedData(const QString &str)
{
  d_last_search_time = QDateTime::currentMSecsSinceEpoch();  
  d_search_button->setEnabled(true);
  if(rda->station()->filterMode()!=RDStation::FilterSynchronous) {
    return;
  }
  searchClickedData();
}


void RDCartFilter::filterToggledData(bool state)
{
  filterChangedData("");
}


void RDCartFilter::setMatchCount(int matches)
{
  d_matches_edit->setText(QString::asprintf("%d",matches));
}


void RDCartFilter::searchClickedData()
{

  qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (now-d_last_search_time<750)
    return;
  d_last_search_time = now;
  d_search_button->setDisabled(true);
  if(d_filter_edit->text().isEmpty()) {
    d_clear_button->setDisabled(true);
  }
  else {
    d_clear_button->setEnabled(true);
  }
  UpdateModel();
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


void RDCartFilter::schedulerCodeChangedData(const QString &str)
{
  QString sql;
  RDSqlQuery *q=NULL;

  d_and_codes_label->setEnabled(d_codes_box->currentIndex()>0);
  d_and_codes_box->setEnabled(d_codes_box->currentIndex()>0);
  d_and_codes_box->clear();
  d_and_codes_box->insertItem(0,tr("[none]"));
  if(d_codes_box->currentIndex()>0) {
    sql=QString("select ")+
      "`CODE` "+  // 00
      "from `SCHED_CODES` where "+
      "`CODE`!='"+RDEscapeString(d_codes_box->currentText())+"' "+
      "order by `CODE`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      d_and_codes_box->insertItem(d_codes_box->count(),q->value(0).toString());
    }
    delete q;
  }
  filterChangedData("");
}


void RDCartFilter::andSchedulerCodeChangedData(const QString &str)
{
  filterChangedData("");
}

/*
void RDCartFilter::checkChangedData(int n)
{
  filterChangedData("");
}
*/

void RDCartFilter::showtypeActivatedData(int n)
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
  int w=e->size().width();

  switch(rda->station()->filterMode()) {
  case RDStation::FilterSynchronous:
    d_filter_edit->setGeometry(80,5,w-180,20);
    break;

  case RDStation::FilterAsynchronous:
    d_search_button->setGeometry(w-180,5,80,50);
    d_filter_edit->setGeometry(80,5,w-280,20);
    break;
  }
  d_clear_button->setGeometry(w-90,5,80,50);
  d_filter_label->setGeometry(20,5,55,20);
  d_group_label->setGeometry(20,33,55,20);
  d_group_box->setGeometry(80,31,140,24);
  d_codes_label->setGeometry(225,33,115,20);
  d_codes_box->setGeometry(345,31,120,24);
  d_and_codes_label->
    setGeometry(465,33,labelFontMetrics()->width(d_and_codes_label->text()),20);
  d_and_codes_label->setVisible(w>RDCARTFILTER_SECOND_SCHED_CODE_WIDTH);
  d_and_codes_box->
    setGeometry(d_and_codes_label->x()+d_and_codes_label->width(),31,120,24);
  d_and_codes_box->setVisible(w>810);
  
  int x_pos=10;

  if(d_show_cart_type==RDCart::All) {
    d_showtype_label->setGeometry(x_pos,61,65,24);
    d_showtype_box->setGeometry(x_pos+70,61,100,24);
    x_pos+=190;
  }
  else {
    x_pos+=70;
  }
  
  d_shownotes_box->setGeometry(x_pos,70-5,15,15);
  d_shownotes_label->setGeometry(x_pos+20,61,130,24);
  x_pos+=160;
  
  if(d_show_drag_box) {
    d_allowdrag_box->setGeometry(x_pos,70-5,15,15);
    d_allowdrag_label->setGeometry(x_pos+20,61,130,24);
    x_pos+=160;
  }

  d_showmatches_check->setGeometry(w-340,65,15,15);
  d_showmatches_label->setGeometry(w-320,61,180,24);
  
  d_matches_label->setGeometry(w-135,61,60,24);
  d_matches_edit->setGeometry(w-75,61,65,24);
}


QString RDCartFilter::phraseFilter(QString phrase, bool incl_cuts)
{
  QString sql="";
  QList<unsigned> cart_numbers;
  QStringList clauses;
  bool ok=false;

  if(phrase.trimmed().isEmpty()) {
    sql=" ";
  }
  else {
    //
    // Separate Out Cart Numbers
    //
    QStringList words=phrase.split(" ",QString::KeepEmptyParts);
    for(int i=0;i<words.size();i++) {
      unsigned cartnum=words.at(i).toUInt(&ok);
      if(ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER)) {
	cart_numbers.push_back(cartnum);
	words.removeAt(i);
	i--;
      }
    }
    phrase=words.join(" ").trimmed();

    //
    // Generate Clauses
    //
    QString accum;
    for(int i=0;i<phrase.size();i++) {
      if(phrase.at(i)==QChar('"')) {
	if(!accum.trimmed().isEmpty()) {
	  clauses.push_back(accum.trimmed());
	}
	accum="";
      }
      else {
	accum+=phrase.at(i);
      }
    }
    if(!accum.isEmpty()) {
      clauses.push_back(accum.trimmed());
    }

    //
    // Compose SQL
    //
    if((clauses.size()==0)&&(cart_numbers.size()==0)) {
      sql="";
    }
    else {
      sql+=" (";
      for(int i=0;i<clauses.size();i++) {
	sql+=RDCartFilter::ClauseSql(clauses.at(i),incl_cuts);
      }
      for(int i=0;i<cart_numbers.size();i++) {
	sql+=QString::asprintf("(`CART`.`NUMBER`=%u)||",cart_numbers.at(i));
      }
      if(sql.right(2)=="||") {
	sql=sql.left(sql.length()-2);
      }
      sql+=") && ";
    }
  }

  return sql;
}


QString RDCartFilter::groupFilter(const QString &group,
				  const QStringList &groups)
{
  if(groups.size()<=1) {  // No actual groups included!
    return QString("(`CART`.`NUMBER`<0) &&");  // Force empty selection
  }

  QString sql=" (";

  if(group==tr("ALL")) {
    for(int i=1;i<groups.size();i++) {
      sql+="(`CART`.`GROUP_NAME`=\""+RDEscapeString(groups.at(i))+"\")||";
    }
    sql=sql.left(sql.length()-2);
  }
  else {
    sql+="`CART`.`GROUP_NAME`=\""+RDEscapeString(group)+"\"";
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
    sql="((`CART`.`TYPE`=1)||(`CART`.`TYPE`=3)) &&";
    break;

  case RDCart::Macro:
    sql="(`CART`.`TYPE`=2) &&";
    break;

  case RDCart::All:
    if(incl_audio) {
      if(incl_macro) {
	sql="((`CART`.`TYPE`=1)||(`CART`.`TYPE`=2)||(`CART`.`TYPE`=3)) &&";
      }
      else {
	sql="((`CART`.`TYPE`=1)||(`CART`.`TYPE`=3)) &&";
      }
    }
    else {
      if(incl_macro) {
	sql="(`CART`.`TYPE`=2) &&";
      }
      else {
	sql="(`CART`.`TYPE`=0) &&";  // NEVER matches!
      }
    }
    break;
  }
  return sql;
}


QString RDCartFilter::typeFilter(RDCart::Type type,RDCart::Type mask)
{
  QString sql;

  switch(mask) {
  case RDCart::Audio:
    switch(type) {
    case RDCart::Audio:
    case RDCart::All:
      sql="(`CART`.`TYPE`=1) &&";
      break;

    case RDCart::Macro:
      sql="(`CART`.`TYPE`=0) &&";
      break;
    }
    break;

  case RDCart::Macro:
    switch(type) {
    case RDCart::Macro:
    case RDCart::All:
      sql="(`CART`.`TYPE`=2) &&";
      break;

    case RDCart::Audio:
      sql="(`CART`.`TYPE`=0) &&";
      break;
    }
    break;

  case RDCart::All:
    switch(type) {
    case RDCart::All:
      sql="((`CART`.`TYPE`=1)||(`CART`.`TYPE`=2)) &&";
      break;

    case RDCart::Audio:
      sql="(`CART`.`TYPE`=1) &&";
      break;

    case RDCart::Macro:
      sql="(`CART`.`TYPE`=2) &&";
      break;
    }
    break;
  }

  return sql;
}


void RDCartFilter::showEvent(QShowEvent *e)
{
  UpdateModel();
  QWidget::showEvent(e);
}


void RDCartFilter::LoadUserGroups()
{
  QString sql;
  RDSqlQuery *q;

  d_group_box->clear();
  d_group_box->insertItem(0,tr("ALL"));
  if(d_user_is_admin) {
    sql=QString("select `NAME` from `GROUPS` order by `NAME` ");
  }
  else {
    sql=QString("select `GROUP_NAME` from `USER_PERMS` where ")+
      "`USER_NAME`='"+RDEscapeString(rda->user()->name())+"' "+
      "order by `GROUP_NAME`";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_box->insertItem(d_group_box->count(),q->value(0).toString());
  }
  delete q;

  groupChangedData(d_group_box->currentText());
}


void RDCartFilter::LoadServiceGroups()
{
  QString sql;
  RDSqlQuery *q=NULL;

  d_group_box->clear();
  d_group_box->insertItem(0,tr("ALL"));
  sql=QString("select ")+
    "`GROUP_NAME` "+
    "from `AUDIO_PERMS` where "+
    "`SERVICE_NAME`='"+RDEscapeString(d_service)+"' "+
    "order by `GROUP_NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_box->insertItem(d_group_box->count(),q->value(0).toString());
  }
  delete q;
}


void RDCartFilter::UpdateModel()
{
  if((filterSql()!=d_model_filter_sql)||(cartLimit()!=d_model_cart_limit)) {
    d_model_filter_sql=filterSql();
    d_model_cart_limit=cartLimit();
    if(rda->config()->logSearchStrings()) {
      rda->syslog(rda->config()->logSearchStringsLevel(),
		  "searching cart library by string: \"%s\" [%s]",
		  d_filter_edit->text().toUtf8().constData(),
		  RDConfig::hexify(d_filter_edit->text()).toUtf8().constData());
    }
    emit filterChanged(d_model_filter_sql,d_model_cart_limit);
  }
}


QString RDCartFilter::ClauseSql(const QString &clause,bool incl_cuts)
{
  QString search=RDEscapeString(clause);
  QString sql="";
  QStringList words = search.split(' ');

  bool first=false;
  foreach (const QString &word, words) {
    if (first)
        sql=sql+QString(" AND ");

    sql=sql+QString(" CONCAT(`CART`.`TITLE`,")+
                            " `CART`.`ARTIST`,"+
                            " `CART`.`AGENCY`,"+
                            " `CART`.`ALBUM`,"+
                            " `CART`.`LABEL`,"+
                            " `CART`.`PUBLISHER`,"+
                            " `CART`.`COMPOSER`,"+
                            " `CART`.`CONDUCTOR`,"+
                            " `CART`.`SONG_ID`,"+
                            " `CART`.`USER_DEFINED`"+
                            ") like \"%" +word+ "%\" ";
    first=true;
  }

  if (sql!="")
    sql=" ("+sql+") ||";

  if(incl_cuts) {
    sql+=QString("(`CUTS`.`ISCI` like '%")+search+"%')||"+
      "(`CUTS`.`ISRC` like '%"+search+"%')||"+
      "(`CUTS`.`DESCRIPTION` like '%"+search+"%')||"+
      "(`CUTS`.`OUTCUE` like '%"+search+"%')||";
  }

  return sql;
}
