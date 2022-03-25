// edit_log.cpp
//
// Edit a Rivendell Log
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>
#include <QPainter>

#include <rdadd_log.h>
#include <rdconf.h>
#include <rddatedialog.h>
#include <rdtextvalidator.h>

#include "add_meta.h"
#include "edit_log.h"
#include "globals.h"

EditLog::EditLog(QString *filter,QString *group,QString *schedcode,
		 QList<RDLogLine> *clipboard,QWidget *parent)
  : RDDialog(parent)
{
  QColor system_mid_color=palette().mid().color();
  QColor system_button_color=palette().button().color();

  edit_filter=filter;
  edit_group=group;
  edit_schedcode=schedcode;
  edit_clipboard=clipboard;
  edit_default_trans=RDLogLine::Play;
  edit_log=NULL;
  edit_log_lock=NULL;

  setWindowTitle("RDLogEdit - "+tr("Edit Log"));

  //
  // Config Data
  //
  edit_default_trans=rda->logeditConf()->defaultTransType();
  edit_output_card=rda->logeditConf()->outputCard();
  edit_output_port=rda->logeditConf()->outputPort();
  edit_start_macro=rda->logeditConf()->startCart();
  edit_end_macro=rda->logeditConf()->endCart();

  //
  // Fix the Window Size
  //
  setMinimumWidth(RDLOGEDIT_EDITLOG_DEFAULT_WIDTH);
  setMinimumHeight(RDLOGEDIT_EDITLOG_DEFAULT_HEIGHT);

  //
  // Dialogs
  //
  edit_render_dialog=new RenderDialog(this);
  edit_logline_dialog=
    new EditLogLine(edit_filter,edit_group,edit_schedcode,this);
  edit_marker_dialog=new EditMarker(this);
  edit_track_dialog=new EditTrack(this);
  edit_chain_dialog=new EditChain(this);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Log Name
  //
  edit_modified_label=new QLabel(this);
  edit_modified_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  edit_modified_label->setFont(progressFont());
  edit_logname_label=new QLabel(this);
  edit_logname_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_logname_label_label=new QLabel(tr("Log Name:"),this);
  edit_logname_label_label->setFont(labelFont());
  edit_logname_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Track Counts
  //
  edit_track_label=new QLabel(this);
  edit_track_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_track_label_label=new QLabel(tr("Tracks:"),this);
  edit_track_label_label->setFont(labelFont());
  edit_track_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Log Origin
  //
  edit_origin_label=new QLabel(this);
  edit_origin_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_origin_label_label=new QLabel(tr("Origin:"),this);
  edit_origin_label_label->setFont(labelFont());
  edit_origin_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  connect(edit_description_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(descriptionChangedData(const QString &)));
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Purge Date
  //
  edit_purgedate_box=new QCheckBox(this);
  edit_purgedate_label=new QLabel(tr("Delete on"),this);
  edit_purgedate_label->setFont(labelFont());
  edit_purgedate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_purgedate_edit=new RDDateEdit(this);
  edit_purgedate_button=new QPushButton(tr("Select"),this);
  edit_purgedate_button->setFont(buttonFont());
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  this,SLOT(purgeDateToggledData(bool)));
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  edit_purgedate_edit,SLOT(setEnabled(bool)));
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  edit_purgedate_button,SLOT(setEnabled(bool)));
  connect(edit_purgedate_edit,SIGNAL(dateChanged(const QDate &)),
	  this,SLOT(purgeDateChangedData(const QDate &)));
  connect(edit_purgedate_button,SIGNAL(clicked()),
	  this,SLOT(selectPurgeDateData()));

  //
  // Service
  //
  edit_service_box=new QComboBox(this);
  edit_service_edit=new QLineEdit(this);
  edit_service_edit->setReadOnly(true);
  edit_service_label=new QLabel(tr("Service:"),this);
  edit_service_label->setFont(labelFont());
  edit_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  connect(edit_service_box,SIGNAL(activated(const QString &)),
	  this,SLOT(serviceActivatedData(const QString &)));

  //
  // Auto Refresh
  //
  edit_autorefresh_box=new QComboBox(this);
  edit_autorefresh_box->insertItem(0,tr("Yes"));
  edit_autorefresh_box->insertItem(1,tr("No"));
  edit_autorefresh_edit=new QLineEdit(this);
  edit_autorefresh_edit->setReadOnly(true);
  edit_autorefresh_label=new QLabel(tr("Enable AutoRefresh:"),this);
  edit_autorefresh_label->setFont(labelFont());
  edit_autorefresh_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  connect(edit_autorefresh_box,SIGNAL(activated(int)),
	  this,SLOT(autorefreshChangedData(int)));

  //
  // Start Date
  //
  edit_startdate_edit=new RDDateEdit(this);
  edit_startdate_label=new QLabel(tr("Start Date:"),this);
  edit_startdate_label->setFont(labelFont());
  edit_startdate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  connect(edit_startdate_edit,SIGNAL(dateChanged(const QDate &)),
	  this,SLOT(dateValueChangedData(const QDate &)));

  //
  // End Date
  //
  edit_enddate_edit=new RDDateEdit(this);
  edit_enddate_label=new QLabel(tr("End Date:"),this);
  edit_enddate_label->setFont(labelFont());
  edit_enddate_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  
  connect(edit_enddate_edit,SIGNAL(dateChanged(const QDate &)),
	  this,SLOT(dateValueChangedData(const QDate &)));

  //
  // Start Date Checkbox
  //
  edit_startdate_box=new QCheckBox(this);
  connect(edit_startdate_box,SIGNAL(toggled(bool)),
	  this,SLOT(startDateEnabledData(bool)));
  edit_startdate_box_label=new QLabel(tr("Start Date Enabled"),this);
  edit_startdate_box_label->setFont(labelFont());
  edit_startdate_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // End Date Checkbox
  //
  edit_enddate_box=new QCheckBox(this);
  connect(edit_enddate_box,SIGNAL(toggled(bool)),
	  this,SLOT(endDateEnabledData(bool)));
  edit_enddate_box_label=new QLabel(tr("End Date Enabled"),this);
  edit_enddate_box_label->setFont(labelFont());
  edit_enddate_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);  

  //
  // Time Counter Section
  //
  edit_time_groupbox=new QGroupBox(tr("Run Length"),this);
  edit_time_groupbox->setFont(labelFont());

  //
  // Stop Time Counter
  //
  edit_stoptime_edit=new QLineEdit(this);
  edit_stoptime_label=new QLabel(tr("Next Stop:"),this);
  edit_stoptime_label->setFont(labelFont());
  edit_stoptime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  

  //
  // End Time Counter
  //
  edit_endtime_edit=new QLineEdit(this);
  edit_endtime_label=new QLabel(tr("Log End:"),this);
  edit_endtime_label->setFont(labelFont());
  edit_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);  

  //
  // Log Event View
  //
  edit_log_view=new LogTableView(this);
  edit_log_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  edit_log_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  edit_log_view->setShowGrid(false);
  edit_log_view->setSortingEnabled(false);
  edit_log_view->setWordWrap(false);
  edit_log_model=new LogModel(this);
  edit_log_model->setFont(defaultFont());
  edit_log_model->setPalette(palette());
  edit_log_view->setModel(edit_log_model);
  connect(edit_log_model,
	  SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
	  this,SLOT(dataChangedData(const QModelIndex &,const QModelIndex &)));
  connect(edit_log_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_log_view,SIGNAL(clicked(const QModelIndex &)),
	  this,SLOT(clickedData(const QModelIndex &)));
  connect(edit_log_view,SIGNAL(cartDropped(int,RDLogLine *)),
	  this,SLOT(cartDroppedData(int,RDLogLine *)));
  connect(edit_log_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,const QItemSelection)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  edit_log_model,SLOT(processNotification(RDNotification *)));

  //
  //  Insert Cart Button
  //
  edit_cart_button=new QPushButton(this);
  edit_cart_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_cart_button->setFont(buttonFont());
  edit_cart_button->setText(tr("Insert\nCart"));
  connect(edit_cart_button,SIGNAL(clicked()),
	  this,SLOT(insertCartButtonData()));

  //
  //  Insert Marker Button
  //
  edit_marker_button=new QPushButton(this);
  edit_marker_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_marker_button->setFont(buttonFont());
  edit_marker_button->setText(tr("Insert\nMeta"));
  connect(edit_marker_button,SIGNAL(clicked()),
	  this,SLOT(insertMarkerButtonData()));

  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_edit_button->setFont(buttonFont());
  edit_edit_button->setText(tr("Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editButtonData()));

  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_delete_button->setFont(buttonFont());
  edit_delete_button->setText(tr("Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteButtonData()));

  //
  //  Up Button
  //
  edit_up_button=new RDTransportButton(RDTransportButton::Up,this);
  edit_up_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  connect(edit_up_button,SIGNAL(clicked()),this,SLOT(upButtonData()));

  //
  //  Down Button
  //
  edit_down_button=new RDTransportButton(RDTransportButton::Down,this);
  edit_down_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  connect(edit_down_button,SIGNAL(clicked()),this,SLOT(downButtonData()));

  //
  //  Cut Button
  //
  edit_cut_button=new QPushButton(this);
  edit_cut_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_cut_button->setFont(buttonFont());
  edit_cut_button->setText(tr("Cut"));
  connect(edit_cut_button,SIGNAL(clicked()),this,SLOT(cutButtonData()));

  //
  //  Copy Button
  //
  edit_copy_button=new QPushButton(this);
  edit_copy_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_copy_button->setFont(buttonFont());
  edit_copy_button->setText(tr("Copy"));
  connect(edit_copy_button,SIGNAL(clicked()),this,SLOT(copyButtonData()));

  //
  //  Paste Button
  //
  edit_paste_button=new QPushButton(this);
  edit_paste_button->
    setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_paste_button->setFont(buttonFont());
  edit_paste_button->setText(tr("Paste"));
  connect(edit_paste_button,SIGNAL(clicked()),this,SLOT(pasteButtonData()));

  //
  //  Save Button
  //
  edit_save_button=new QPushButton(this);
  edit_save_button->setFont(buttonFont());
  edit_save_button->setText(tr("Save"));
  edit_save_button->setDisabled(true);
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  edit_saveas_button=new QPushButton(this);
  edit_saveas_button->setFont(buttonFont());
  edit_saveas_button->setText(tr("Save")+"\n"+tr("As"));
  connect(edit_saveas_button,SIGNAL(clicked()),this,SLOT(saveasData()));

  //
  //  Render Button
  //
  edit_renderas_button=new QPushButton(this);
  edit_renderas_button->setFont(buttonFont());
  edit_renderas_button->setText(tr("Render"));
  connect(edit_renderas_button,SIGNAL(clicked()),this,SLOT(renderasData()));

  //
  //  Reports Button
  //
  edit_reports_button=new QPushButton(this);
  edit_reports_button->setFont(buttonFont());
  edit_reports_button->setText(tr("Reports"));
  connect(edit_reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Cart Player
  //
  edit_player=
    new RDSimplePlayer(rda->cae(),rda->ripc(),edit_output_card,edit_output_port,
		       edit_start_macro,edit_end_macro,this);
  edit_player->stopButton()->setOnColor(Qt::red);

  //
  // Start Time Style
  //
  edit_timestyle_box=new QComboBox(this);
  edit_timestyle_box->insertItem(edit_timestyle_box->count(),tr("Estimated"));
  edit_timestyle_box->insertItem(edit_timestyle_box->count(),tr("Scheduled"));
  edit_timestyle_box->setCurrentIndex(global_start_time_style);
  connect(edit_timestyle_box,SIGNAL(activated(int)),
	  this,SLOT(timestyleChangedData(int)));
  edit_timestyle_label=new QLabel(tr("Show Start Times As"),this);
  edit_timestyle_label->setFont(labelFont());
  edit_timestyle_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditLog::~EditLog()
{
  delete edit_log_model;
  delete edit_log_lock;
}


QSize EditLog::sizeHint() const
{
  return global_logedit_window_size;
} 


int EditLog::exec(const QString &logname,QStringList *new_logs)
{
  QString sql;
  RDSqlQuery *q;
  QString username;
  QString stationname;
  QHostAddress addr;
  bool adding_allowed=rda->user()->addtoLog();
  bool deleting_allowed=rda->user()->removefromLog();
  bool editing_allowed=rda->user()->arrangeLog();
  bool saveas_allowed=rda->user()->createLog();
  QStringList services_list;

  edit_logname=logname;
  edit_newlogs=new_logs;
  if(edit_log!=NULL) {
    delete edit_log;
  }
  edit_log=new RDLog(edit_logname);
  edit_logname_label->setText(edit_logname);
  edit_log_model->setLogName(edit_logname);
  edit_log_model->load(true);
  edit_log_view->resizeColumnsToContents();
  edit_origin_label->
    setText(edit_log->originUser()+QString(" - ")+
	    edit_log->originDatetime().toString("MM/dd/yyyy - hh:mm:ss"));
  edit_log_lock=new RDLogLock(edit_logname,rda->user(),rda->station(),this);
  if(!edit_log_lock->tryLock(&username,&stationname,&addr)) {
    QString msg=tr("Log already being edited by")+" "+username+"@"+stationname;
    if(stationname!=addr.toString()) {
      msg+=" ["+addr.toString()+"]";
    }
    msg+=".";
    QMessageBox::warning(this,"RDLogEdit - "+tr("Log Locked"),msg);
    return false;
  }
  edit_description_edit->setText(edit_log->description());

  QDate purge_date=edit_log->purgeDate();
  if(purge_date.isNull()) {
    edit_purgedate_edit->setDate(QDate::currentDate().addMonths(1));
    edit_purgedate_edit->setDisabled(true);
    edit_purgedate_button->setDisabled(true);
  }
  else {
    edit_purgedate_box->setChecked(true);
    edit_purgedate_edit->setDate(purge_date);
  }
  sql=QString("select `NAME` from `SERVICES`");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;

  int n=-1;
  int ncounter=0;
  QString service=edit_log->service();
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end(); ++it ) {
    edit_service_box->insertItem(edit_service_box->count(),*it);
    if(*it==service) {
      n=ncounter;
      edit_service_edit->setText(*it);
    }
    ncounter++;
  }
  if(n>=0) {
    edit_service_box->setCurrentIndex(n);
  }
  if(edit_log->autoRefresh()) {
    edit_autorefresh_box->setCurrentIndex(0);
    edit_autorefresh_edit->setText(tr("Yes"));
  }
  else {
    edit_autorefresh_box->setCurrentIndex(1);
    edit_autorefresh_edit->setText(tr("No"));
  }
  if(!edit_log->startDate().isNull()) {
    edit_startdate_box->setChecked(true);
    edit_startdate_edit->setDate(edit_log->startDate());
  }
  else {
    edit_startdate_edit->setDisabled(true);
    edit_startdate_label->setDisabled(true);
    edit_startdate_box->setChecked(false);
  }
  if(!edit_log->endDate().isNull()) {
    edit_enddate_box->setChecked(true);
    edit_enddate_edit->setDate(edit_log->endDate());
  }
  else {
    edit_enddate_edit->setDisabled(true);
    edit_enddate_label->setDisabled(true);
    edit_enddate_box->setChecked(false);
  }
  serviceActivatedData(edit_service_box->currentText());
  SetLogModified(false);
  UpdateTracks();

  //
  // Set Control Perms
  //
  edit_description_edit->setReadOnly(!editing_allowed);
  if(saveas_allowed) {
    edit_service_edit->hide();
    edit_autorefresh_edit->hide();
  }
  else {
    edit_service_box->hide();
    edit_autorefresh_box->hide();
  }
  if(!editing_allowed) {
    if(edit_startdate_box->isChecked()) {
      edit_startdate_edit->setMinimumDate(edit_startdate_edit->date());
      edit_startdate_edit->setMaximumDate(edit_startdate_edit->date());
    }
    if(edit_enddate_box->isChecked()) {
      edit_enddate_edit->setMinimumDate(edit_enddate_edit->date());
      edit_enddate_edit->setMaximumDate(edit_enddate_edit->date());
    }
  }
  edit_startdate_box->setEnabled(editing_allowed);
  edit_enddate_box->setEnabled(editing_allowed);
  edit_startdate_box->setEnabled(editing_allowed);
  edit_enddate_box->setEnabled(editing_allowed);
  edit_cart_button->setEnabled(adding_allowed&&editing_allowed);
  edit_marker_button->setEnabled(adding_allowed&&editing_allowed);
  edit_edit_button->setEnabled(editing_allowed&&editing_allowed);
  edit_delete_button->setEnabled(deleting_allowed&&editing_allowed);
  edit_up_button->setEnabled(editing_allowed);
  edit_down_button->setEnabled(editing_allowed);
  edit_cut_button->
    setEnabled(adding_allowed&&deleting_allowed&&editing_allowed);
  edit_copy_button->setEnabled(adding_allowed&&editing_allowed);
  edit_paste_button->setEnabled(adding_allowed&&editing_allowed);
  edit_saveas_button->setEnabled(saveas_allowed);

  return QDialog::exec();
}


void EditLog::dataChangedData(const QModelIndex &top_left,
			      const QModelIndex &bottom_right)
{
  SetLogModified(true);
}


void EditLog::descriptionChangedData(const QString &)
{
  SetLogModified(true);
}


void EditLog::purgeDateChangedData(const QDate &date)
{
  SetLogModified(true);
}


void EditLog::purgeDateToggledData(bool state)
{
  SetLogModified(true);
}


void EditLog::selectPurgeDateData()
{
  QDate date=edit_purgedate_edit->date();
  RDDateDialog *d=new RDDateDialog(2008,QDate::currentDate().year()+5,this);
  if(d->exec(&date)==0) {
    edit_purgedate_edit->setDate(date);
    SetLogModified(true);
  }
}


void EditLog::serviceActivatedData(const QString &svcname)
{
  SetLogModified(true);
  edit_log_model->setServiceName(svcname);
  if(!edit_log_model->allGroupsValid()) {
    QMessageBox::warning(this,tr("Invalid Carts"),
			 tr("This log contains one or more carts that are invalid for the selected service!"));
  }
}


void EditLog::startDateEnabledData(bool state)
{
  if(state) {
    edit_startdate_edit->setDate(QDate::currentDate());
  }
  edit_startdate_edit->setEnabled(state);
  edit_startdate_label->setEnabled(state);
  SetLogModified(true);
}


void EditLog::endDateEnabledData(bool state)
{
  if(state) {
    edit_enddate_edit->setDate(QDate::currentDate());
  }
  edit_enddate_edit->setEnabled(state);
  edit_enddate_label->setEnabled(state);
  SetLogModified(true);
}


void EditLog::timestyleChangedData(int index)
{
  bool changed=edit_changed;
  edit_log_model->setStartTimeStyle((RDLogModel::StartTimeStyle)index);
  if(!changed) {
    SetLogModified(false);
  }
}


void EditLog::dateValueChangedData(const QDate &)
{
  SetLogModified(true);
}


void EditLog::autorefreshChangedData(int index)
{
  SetLogModified(true);
}


void EditLog::insertCartButtonData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }

  edit_log_model->insert(line,1);
  edit_log_model->logLine(line)->setTransType(edit_default_trans);
  edit_log_model->logLine(line)->setFadeupGain(-3000);
  edit_log_model->logLine(line)->setFadedownGain(-3000);
  int ret=edit_logline_dialog->
    exec(edit_service_box->currentText(),edit_log_model,
	 edit_log_model->logLine(line),line);
  if(ret>=0) {
    edit_log_model->update(line);
    SetLogModified(true);
  }
  else {
    edit_log_model->remove(line,1);
    return;
  }
  UpdateCounters();
}


void EditLog::insertMarkerButtonData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }

  int ret;
  AddMeta *meta=new AddMeta(this);
  switch((RDLogLine::Type)meta->exec()) {
  case RDLogLine::Marker:
    edit_log_model->insert(line,1);
    edit_log_model->logLine(line)->setType(RDLogLine::Marker);
    ret=edit_marker_dialog->exec(edit_log_model->logLine(line));
    if(ret>=0) {
      edit_log_model->update(line);
      SetLogModified(true);
    }
    else {
      edit_log_model->remove(line,1);
    }
    break;

  case RDLogLine::Track:
    edit_log_model->insert(line,1);
    edit_log_model->logLine(line)->setType(RDLogLine::Track);
    edit_log_model->logLine(line)->setTransType(RDLogLine::Segue);
    edit_log_model->logLine(line)->setMarkerComment(tr("Voice Track"));
    ret=edit_track_dialog->exec(edit_log_model->logLine(line));
    if(ret>=0) {
      edit_log_model->update(line);
      SetLogModified(true);
    }
    else {
      edit_log_model->remove(line,1);
    }
    break;

  case RDLogLine::Chain:
    edit_log_model->insert(line,1);
    edit_log_model->logLine(line)->setType(RDLogLine::Chain);
    ret=edit_chain_dialog->exec(edit_log_model->logLine(line));
    if(ret>=0) {
      edit_log_model->update(line);
      SetLogModified(true);
    }
    else {
      edit_log_model->remove(line,1);
    }
    break;

  default:
    break;
  }
  UpdateTracks();
  UpdateCounters();
}


void EditLog::clickedData(const QModelIndex &index)
{
  int line;
  RDLogLine *ll=NULL;

  if((line=SingleSelectionLine())<0) {
    edit_player->setCart(0);
  }
  else {
    if((ll=edit_log_model->logLine(line))!=NULL) {
      if(ll->type()==RDLogLine::Cart) {
	edit_player->setCart(ll->cartNumber());
      }
    }
    else {
      edit_player->setCart(0);
    }
  }
}


void EditLog::selectionChangedData(const QItemSelection &selected,
				   const QItemSelection &deselected)
{
  UpdateCounters();
}


void EditLog::doubleClickedData(const QModelIndex &index)
{
  int line=index.row();

  if(line>=(edit_log_model->rowCount()-1)) {
    insertCartButtonData();
  }
  else {
    editButtonData();
  }
}


void EditLog::editButtonData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }

  switch(edit_log_model->logLine(line)->type()) {
  case RDLogLine::Cart:
  case RDLogLine::Macro:
    if(edit_logline_dialog->exec(edit_service_box->currentText(),edit_log_model,
				 edit_log_model->logLine(line),line)>=0) {
      edit_log_model->update(line);
      SetLogModified(true);
    }
    break;

  case RDLogLine::Marker:
    if(edit_marker_dialog->exec(edit_log_model->logLine(line))>=0) {
      SetLogModified(true);
    }
    break;

  case RDLogLine::Track:
    if(edit_track_dialog->exec(edit_log_model->logLine(line))>=0) {
      SetLogModified(true);
    }
    break;

  case RDLogLine::Chain:
    if(edit_chain_dialog->exec(edit_log_model->logLine(line))>=0) {
      SetLogModified(true);
    }
    break;

  default:
    break;
  }
  UpdateCounters();
}


void EditLog::deleteButtonData()
{
  QModelIndexList rows=edit_log_view->selectionModel()->selectedRows();

  if(rows.last().row()>=(edit_log_model->rowCount()-1)) {
    rows.removeLast();    // So we don't remove the end handle!
  }
  if(rows.size()==0) {
    return;
  }
  DeleteLines(rows.first().row(),rows.size());
  edit_log_view->selectionModel()->
    select(edit_log_model->index(rows.first().row(),0),
	   QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
  UpdateCounters();
}


void EditLog::upButtonData()
{
  int line;

  if((line=SingleSelectionLine())<=0) {
    return;
  }
  edit_log_model->move(line,line-1);
  SetLogModified(true);
  edit_log_view->selectionModel()->
    select(edit_log_model->index(line-1,0),
	   QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
  UpdateCounters();
}


void EditLog::downButtonData()
{
  int line;

  if(((line=SingleSelectionLine())<0)||
     (line>=(edit_log_model->lineCount()-1))) {
    return;
  }
  edit_log_model->move(line,line+1);
  SetLogModified(true);
  edit_log_view->selectionModel()->
    select(edit_log_model->index(line+1,0),
	   QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
  UpdateCounters();
}


void EditLog::cutButtonData()
{
  LoadClipboard(false);
  deleteButtonData();
  UpdateTracks();
  UpdateCounters();
}


void EditLog::copyButtonData()
{
  LoadClipboard(true);
}


void EditLog::pasteButtonData()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    return;
  }
  edit_log_model->insert(line,edit_clipboard->size());
  for(int i=0;i<edit_clipboard->size();i++) {
    *edit_log_model->logLine(line+i)=edit_clipboard->at(i);
    edit_log_model->logLine(line+i)->setSource(RDLogLine::Manual);
    (*edit_clipboard)[i].clearExternalData();
  }
  SetLogModified(true);
  UpdateTracks();
  UpdateCounters();
}


void EditLog::cartDroppedData(int line,RDLogLine *ll)
{
  bool appended=false;

  if((line<0)||(line==edit_log_model->lineCount())) {
    line=edit_log_model->lineCount();
    appended=true;
  }
  if(ll->cartNumber()==0) {  // Empty Cart
    if(!appended) {
      DeleteLines(line,1);
    }
    return;
  }
  edit_log_model->insert(line,1);
  edit_log_model->setLogLine(line,ll);
  edit_log_model->logLine(line)->setTransType(edit_default_trans);
  edit_log_model->logLine(line)->setFadeupGain(-3000);
  edit_log_model->logLine(line)->setFadedownGain(-3000);
  edit_log_model->update(line);
  SetLogModified(true);
}


void EditLog::saveData()
{
  if(!edit_log_model->allGroupsValid()) {
    if(QMessageBox::warning(this,tr("Invalid Carts"),
			    tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }
  if(!DeleteTracks()) {
    QMessageBox::warning(this,tr("RDLogEdit"),
			 tr("Unable to save log, audio deletion error!"));
    return;
  }
  SaveLog();
  SetLogModified(false);
  edit_log->setAutoRefresh(edit_autorefresh_box->currentIndex()==0);
  edit_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


void EditLog::saveasData()
{
  if(!edit_log_model->allGroupsValid()) {
    if(QMessageBox::warning(this,tr("Invalid Carts"),
			    tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }
  QString logname;
  QString svcname=edit_service_box->currentText();
  RDAddLog *log=NULL;
  QString err_msg;

  if(rda->user()->createLog()) {
    log=new RDAddLog(&logname,&svcname,RDLogFilter::UserFilter,
		     tr("Add Log"),this);
    if(log->exec()<0) {
      return;
    }
    if(!RDLog::create(logname,svcname,QDate(),rda->ripc()->user(),&err_msg,
		      rda->config())) {
      QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),err_msg);
      return;
    }
    SendNotification(RDNotification::AddAction,logname);
    delete edit_log;
    edit_newlogs->push_back(logname);
    edit_log=new RDLog(logname);
    edit_log_model->setLogName(logname);
    for(int i=0;i<edit_service_box->count();i++) {
      if(edit_service_box->itemText(i)==svcname) {
	edit_service_box->setCurrentIndex(i);
      }
    }
    SaveLog();
    edit_logname_label->setText(logname);
    edit_origin_label->
      setText(edit_log->originUser()+QString(" - ")+
	      edit_log->originDatetime().toString("MM/dd/yyyy - hh:mm:ss"));
  }
  delete log;
  SetLogModified(false);
  edit_deleted_tracks.clear();
}


void EditLog::renderasData()
{
  QModelIndexList rows=edit_log_view->selectionModel()->selectedRows();

  if((rows.size()>0)&&(rows.last().row()>=(edit_log_model->rowCount()-1))) {
    rows.removeLast();    // So we don't include the end handle!
  }
  if(rows.size()==0) {
    edit_render_dialog->exec(rda->user(),edit_log_model,0,0);
  }
  else {
    edit_render_dialog->exec(rda->user(),edit_log_model,rows.first().row(),
			     1+rows.last().row());
  }
}


void EditLog::reportsData()
{
  QDate start_date;
  if(edit_startdate_box->isChecked()) {
    start_date=edit_startdate_edit->date();
  }
  QDate end_date;
  if(edit_enddate_box->isChecked()) {
    end_date=edit_enddate_edit->date();
  }
  ListReports *lr=
    new ListReports(edit_log->name(),edit_description_edit->text(),
		    edit_service_box->currentText(),start_date,end_date,
		    edit_autorefresh_box->currentIndex()==0,
		    edit_log_model,this);
  lr->exec();
  delete lr;
}


void EditLog::okData()
{
  if(edit_changed) {
    if(!edit_log_model->allGroupsValid()) {
      if(QMessageBox::warning(this,tr("Invalid Carts"),
			      tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
	return;
      }
    }
    SaveLog();
    DeleteTracks();
  }
  edit_player->stop();
  for(int i=0;i<edit_clipboard->size();i++) {
    (*edit_clipboard)[i].clearExternalData();
  }
  delete edit_log_lock;
  edit_log_lock=NULL;
  done(true);
}


void EditLog::cancelData()
{
  if(edit_changed) {
    switch(QMessageBox::question(this,
	   tr("RDLogEdit"),
	   tr("The log has been modified.\nDo you want to save your changes?"),
				 QMessageBox::Yes,QMessageBox::No,
				 QMessageBox::Cancel)) {
    case QMessageBox::Yes:
      if(!edit_log_model->allGroupsValid()) {
	QMessageBox::warning(this,tr("Invalid Carts"),
			     tr("The log contains carts that are disabled\nfor the selected service!"));
	    return;
      }
      SaveLog();
      break;

    case QMessageBox::Cancel:
    case QMessageBox::NoButton:
      return;
      break;
    }
  }
  edit_player->stop();
  for(int i=0;i<edit_clipboard->size();i++) {
    (*edit_clipboard)[i].clearExternalData();
  }
  delete edit_log_lock;
  edit_log_lock=NULL;
  done(false);
}


void EditLog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditLog::resizeEvent(QResizeEvent *e)
{
  edit_logname_label->setGeometry(155,11,size().width()-500,18);
  edit_logname_label_label->setGeometry(80,11,70,18);
  edit_modified_label->setGeometry(60,14,20,18);
  edit_track_label->setGeometry(size().width()-425,11,40,18);
  edit_track_label_label->setGeometry(size().width()-510,11,80,18);
  edit_origin_label->setGeometry(size().width()-320,11,310,18);
  edit_origin_label_label->setGeometry(size().width()-370,11,45,18);
  edit_description_edit->setGeometry(110,40,size().width()-390,20);
  edit_description_label->setGeometry(10,40,95,20);
  edit_purgedate_box->setGeometry(size().width()-255,42,15,15);
  edit_purgedate_label->setGeometry(size().width()-240,40,60,20);
  edit_purgedate_edit->setGeometry(size().width()-170,40,100,20);
  edit_purgedate_button->setGeometry(size().width()-60,37,50,26);
  edit_service_box->setGeometry(110,68,120,22);
  edit_service_edit->setGeometry(110,68,120,22);
  edit_service_label->setGeometry(10,68,95,22);
  edit_autorefresh_box->setGeometry(180,94,50,22);
  edit_autorefresh_edit->setGeometry(180,94,50,22);
  edit_autorefresh_label->setGeometry(10,94,165,22);
  edit_startdate_edit->setGeometry(315,68,100,22);
  edit_startdate_label->setGeometry(240,68,70,22);
  edit_enddate_edit->setGeometry(490,68,100,22);
  edit_enddate_label->setGeometry(415,68,70,22);
  edit_startdate_box->setGeometry(250,98,15,15);
  edit_enddate_box->setGeometry(430,98,15,15);
  edit_startdate_box_label->setGeometry(270,96,175,20);
  edit_enddate_box_label->setGeometry(450,96,140,20);

  edit_time_groupbox->setGeometry(620,65,140,59);
  edit_stoptime_label->setGeometry(625,82,65,18);
  edit_stoptime_edit->setGeometry(695,82,60,18);
  edit_endtime_label->setGeometry(625,102,65,18);
  edit_endtime_edit->setGeometry(695,102,60,18);

  edit_log_view->setGeometry(10,127,size().width()-20,size().height()-257);
  edit_cart_button->setGeometry(20,size().height()-125,80,50);
  edit_marker_button->setGeometry(110,size().height()-125,80,50);
  edit_edit_button->setGeometry(200,size().height()-125,80,50);
  edit_delete_button->setGeometry(290,size().height()-125,80,50);
  edit_up_button->setGeometry(390,size().height()-125,50,50);
  edit_down_button->setGeometry(450,size().height()-125,50,50);
  edit_cut_button->
    setGeometry(size().width()-280,size().height()-125,80,50);
  edit_copy_button->
    setGeometry(size().width()-190,size().height()-125,80,50);
  edit_paste_button->
    setGeometry(size().width()-100,size().height()-125,80,50);
  edit_save_button->setGeometry(10,size().height()-60,80,50);
  edit_saveas_button->setGeometry(100,size().height()-60,80,50);
  edit_reports_button->setGeometry(300,size().height()-60,80,50);
  edit_renderas_button->setGeometry(190,size().height()-60,80,50);
  edit_player->playButton()->setGeometry(410,size().height()-60,80,50);
  edit_player->stopButton()->setGeometry(500,size().height()-60,80,50);

  edit_timestyle_label->setGeometry(600,size().height()-60,150,20);
  edit_timestyle_box->setGeometry(625,size().height()-40,100,30);

  edit_ok_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);

  global_logedit_window_size=e->size();
}


void EditLog::paintEvent(QPaintEvent *e)
{
  QColor system_mid_color = palette().mid().color();
  QPainter *p=new QPainter(this);
  p->fillRect(60,8,size().width()-120,24,QColor(system_mid_color));
  p->fillRect(9,size().height()-130,size().width()-20,60,
	      QColor(system_mid_color));
  p->end();
  delete p;
}


void EditLog::DeleteLines(int line,int count)
{
  edit_log_model->remove(line,count);
  UpdateTracks();
  SetLogModified(true);
}


void EditLog::SaveLog()
{
  edit_log->setDescription(edit_description_edit->text());
  if(edit_purgedate_box->isChecked()) {
    edit_log->setPurgeDate(edit_purgedate_edit->date());
  }
  else {
    edit_log->setPurgeDate(QDate());
  }
  edit_log->setService(edit_service_box->currentText());
  if(edit_startdate_box->isChecked()) {
    edit_log->setStartDate(edit_startdate_edit->date());
  }
  else {
    edit_log->setStartDate(QDate());
  }
  if(edit_enddate_box->isChecked()) {
    edit_log->setEndDate(edit_enddate_edit->date());
  }
  else {
    edit_log->setEndDate(QDate());
  }
  edit_log->setAutoRefresh(edit_autorefresh_box->currentIndex()==0);
  edit_log_model->save(rda->config());
  edit_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
  SendNotification(RDNotification::ModifyAction,edit_log->name());
}


void EditLog::UpdateCounters()
{
  int line;

  if((line=SingleSelectionLine())<0) {
    edit_endtime_edit->setText("");
    edit_stoptime_label->setText(tr("Selected:"));
    QItemSelectionModel *sel=edit_log_view->selectionModel();
    QModelIndexList rows=sel->selectedRows();
    if(rows.size()==0) {
      edit_endtime_edit->setText("");
      edit_stoptime_edit->setText("");
    }
    else {
      if(rows.last().row()>=(edit_log_model->rowCount()-1)) {
	rows.removeLast();    // So we don't include the end handle!
      }
      edit_stoptime_edit->
	setText(RDGetTimeLength(edit_log_model->length(rows.first().row(),
						       rows.last().row()+1),
				true,false));
    }
  }
  else {
    edit_endtime_edit->setText(RDGetTimeLength(edit_log_model->
      length(line,edit_log_model->lineCount()),true,false));
    edit_stoptime_label->setText(tr("Next Stop:"));
    int stoplen=edit_log_model->lengthToStop(line);
    if(stoplen>=0) {
      edit_stoptime_edit->setText(RDGetTimeLength(stoplen,true,false));
    }
    else {
      edit_stoptime_edit->setText("");
    }
  }
}


int EditLog::SingleSelectionLine(bool incl_end_handle)
{
  int offset=-1;
  if(incl_end_handle) {
    offset=0;
  }
  QItemSelectionModel *sel=edit_log_view->selectionModel();
  if((sel->selectedRows().size()==1)&&
     (sel->selectedRows().at(0).row()<(edit_log_model->rowCount()-offset))) {
    return sel->selectedRows().at(0).row();
  }
  return -1;
}


void EditLog::UpdateTracks()
{
  unsigned markers=0;
  unsigned tracks=0;
  RDLogLine *logline;

  for(int i=0;i<edit_log_model->lineCount();i++) {
    logline=edit_log_model->logLine(i);
    if(logline->type()==RDLogLine::Track) {
      markers++;
    }
    if(logline->source()==RDLogLine::Tracker) {
      tracks++;
    }
  }
  edit_track_label->
    setText(QString::asprintf("%u / %u",tracks,markers+tracks));
}


bool EditLog::DeleteTracks()
{
  RDCart *cart;
  for(unsigned i=0;i<edit_deleted_tracks.size();i++) {
    cart=new RDCart(edit_deleted_tracks[i]);
    if(!cart->remove(rda->station(),rda->user(),rda->config())) {
      delete cart;
      return false;
    }
    delete cart;
  }
  return true;
}


void EditLog::LoadClipboard(bool clear_ext)
{
  QItemSelectionModel *sel=edit_log_view->selectionModel();

  edit_clipboard->clear();
  QModelIndexList rows=sel->selectedRows();
  for(int i=0;i<rows.size();i++) {
    edit_clipboard->
      push_back(*edit_log_model->logLine(rows.at(i).row()));
    if(clear_ext) {
      edit_clipboard->back().clearExternalData();
    }
  }
}


void EditLog::SetLogModified(bool state)
{
  if(state!=edit_changed) {
    if(state) {
      edit_modified_label->setText("*");
    }
    else {
      edit_modified_label->setText("");
    }
    edit_save_button->setEnabled(state);
    edit_changed=state;
  }
}


void EditLog::SendNotification(RDNotification::Action action,
			       const QString &log_name)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(log_name));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}
