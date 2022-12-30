// rdlogedit.cpp
//
// The Log Editor Utility for Rivendell.
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

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

#include <rdadd_log.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdprofile.h>
#include <rdreport.h>
#include <rdtextfile.h>
#include <rdtrackerwidget.h>

#include "edit_log.h"
#include "globals.h"
#include "rdlogedit.h"

//
// Global Resources
//
bool global_import_running=false;
QSize global_logedit_window_size;
int global_start_time_style;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDMainWindow("rdlogedit",c)
{
  QString err_msg;

  log_resize=false;
  log_log_view=NULL;
  log_log_model=NULL;
  log_list_locked=false;

  //
  // Fix the Window Size
  //
  setMinimumWidth(RDLOGEDIT_DEFAULT_WIDTH);
  setMinimumHeight(RDLOGEDIT_DEFAULT_HEIGHT);

  //
  // Open the Database
  //
  rda=new RDApplication("RDLogEdit","rdlogedit",RDLOGEDIT_USAGE,this);
  if(!rda->open(&err_msg,NULL,true)) {
    QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),err_msg);
    exit(1);
  }
  log_import_path=RDGetHomeDir();

  if(rda->logeditConf()->isSingleton()) {
    //
    // Ensure that we're the only instance
    //
    if(!rda->makeSingleInstance(&err_msg)) {
      QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),
			    tr("Startup error")+": "+err_msg+".");
      exit(RDCoreApplication::ExitPriorInstance);
    }
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  loadSettings(true);

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
          this,SLOT(caeConnectedData(bool)));
  rda->cae()->connectHost();

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()
    ->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Create Icons
  //
  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::RdLogEdit,22));

  //
  // Log Filter
  //
  log_filter_widget=new RDLogFilter(RDLogFilter::UserFilter,this);

  //
  // Dialogs
  //
  log_edit_dialog=new EditLog(&log_filter,&log_group,&log_schedcode,this);
  log_tracker_dialog=new VoiceTracker(&log_import_path,this);
  
  //
  // Log List
  //
  log_log_view=new RDTableView(this);
  log_log_view->setSortingEnabled(true);
  log_log_view->sortByColumn(0,Qt::AscendingOrder);
  log_log_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  log_log_model=new RDLogListModel(this);
  log_log_model->setFont(defaultFont());
  log_log_model->setPalette(palette());
  log_log_view->setModel(log_log_model);
  log_log_view->resizeColumnsToContents();
  connect(log_filter_widget,
	  SIGNAL(filterChanged(const QString &,const QString &)),
	  log_log_model,SLOT(setFilterSql(const QString &,const QString &)));
  connect(log_log_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(log_log_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,const QItemSelection)));

  //
  // Add Button
  //
  log_add_button=new QPushButton(this);
  log_add_button->setFont(buttonFont());
  log_add_button->setText(tr("Add"));
  connect(log_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  log_edit_button=new QPushButton(this);
  log_edit_button->setFont(buttonFont());
  log_edit_button->setText(tr("Edit"));
  connect(log_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
  log_edit_button->setDisabled(true);

  //
  // Delete Button
  //
  log_delete_button=new QPushButton(this);
  log_delete_button->setFont(buttonFont());
  log_delete_button->setText(tr("Delete"));
  connect(log_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Tracker Button
  //
  log_track_button=new QPushButton(this);
  log_track_button->setFont(buttonFont());
  log_track_button->setText(tr("Voice\nTracker"));
  connect(log_track_button,SIGNAL(clicked()),this,SLOT(trackData()));

  //
  // Log Report Button
  //
  log_report_button=new QPushButton(this);
  log_report_button->setFont(buttonFont());
  log_report_button->setText(tr("Log\nReport"));
  connect(log_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  // Close Button
  //
  log_close_button=new QPushButton(this);
  log_close_button->setFont(buttonFont());
  log_close_button->setText(tr("Close"));
  connect(log_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  setWindowTitle(QString("RDLogEdit")+"v"+VERSION+" - "+tr("Host")+": "+
	     rda->config()->stationName()+", "+
	     tr("User")+": ["+tr("Unknown")+"]");

  log_resize=true;
}


QSize MainWidget::sizeHint() const
{
  return QSize(RDLOGEDIT_DEFAULT_WIDTH,RDLOGEDIT_DEFAULT_HEIGHT);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::loadLocalSettings(RDProfile *p)
{
  global_start_time_style=
    p->intValue("rdlogedit","StartTimeStyle",(int)RDLogModel::Estimated);
}


void MainWidget::saveLocalSettings(FILE *f) const
{
  fprintf(f,"StartTimeStyle=%d\n",global_start_time_style);
}


void MainWidget::caeConnectedData(bool state)
{
  QList<int> cards;
  RDLogeditConf *conf=new RDLogeditConf(rda->config()->stationName());

  cards.push_back(conf->inputCard());
  cards.push_back(conf->outputCard());
  rda->cae()->enableMetering(&cards);
}

void MainWidget::userData()
{
  QString str1;
  QString str2;

  str1=QString("RDLogEdit")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setWindowTitle(str1+": "+rda->config()->stationName()+", "+str2+": "+
		 rda->ripc()->user());

  log_filter_widget->changeUser();
  log_log_model->setFilterSql(log_filter_widget->whereSql(),
			      log_filter_widget->limitSql());
  log_log_view->resizeColumnsToContents();
 
  //
  // Set Control Perms
  //
  log_add_button->setEnabled(rda->user()->createLog());
  log_delete_button->setEnabled(false);
  log_track_button->setEnabled(false);
}


void MainWidget::addData()
{
  QString logname;
  QStringList newlogs;
  RDAddLog *log;
  QModelIndex row;
  QString svcname=log_filter_widget->filterServiceName();

  if(rda->user()->createLog()) {
    log=new RDAddLog(&logname,&svcname,RDLogFilter::UserFilter,
		     tr("Add Log"),this);
    if(log->exec()!=0) {
      delete log;
      return;
    }
    delete log;
    QString username(rda->ripc()->user());
    QString err_msg;
    if(!RDLog::create(logname,svcname,QDate(),username,&err_msg,
		      rda->config())) {
      QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),err_msg);
      return;
    }
    LockList();
    SendNotification(RDNotification::AddAction,logname);
    log_edit_dialog->exec(logname,&newlogs);
    log_log_view->selectRow(row.row());
    UnlockList();
  }
}


void MainWidget::editData()
{
  QModelIndex row=SingleSelectedRow();

  if(!row.isValid()) {
    return;
  }
  QString logname=log_log_model->logName(row);
  QStringList newlogs;
  LockList();
  if(log_edit_dialog->exec(logname,&newlogs)) {
    log_log_model->refresh(row);
  }
  UnlockList();
}


void MainWidget::deleteData()
{
  QString filename;
  QString str1;
  QString str2;
  unsigned tracks=0;

  if(rda->user()->deleteLog()) {
    QModelIndexList rows=log_log_view->selectionModel()->selectedRows();
    for(int i=0;i<rows.size();i++) {
      RDLog *log=new RDLog(log_log_model->logName(rows.at(i)));
      tracks+=log->completedTracks();
      delete log;
    }
    if(rows.size()==1) {
      if(QMessageBox::question(this,"RDLogEdit - "+tr("Delete Log"),
			       tr("Are you sure you want to delete the")+" \""+
			       log_log_model->logName(rows.at(0))+"\" "+
			       tr("log?"),QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	return;
      }
      if(tracks>0) {
	if(QMessageBox::question(this,"RDLogEdit - "+tr("Tracks Exist"),
				 tr("This will also delete the")+
			       QString::asprintf(" %u ",tracks)+
				 tr("voice tracks associated with this log.")+
				 "\n"+tr("Continue?"),
				 QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  return;
	}
      }
    }
    else {
      if(QMessageBox::question(this,"RDLogEdit - "+tr("Delete Log"),
			       tr("Are you sure you want to delete these")+
			       QString::asprintf(" %d ",rows.size())+
			       tr("logs?"),QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	return;
      }
      if(tracks>0) {
	if(QMessageBox::question(this,"RDLogEdit - "+tr("Tracks Exist"),
				 tr("This will also delete the")+
			       QString::asprintf(" %u ",tracks)+
				 tr("voice tracks associated with these logs.")+
				 "\n"+tr("Continue?"),
				 QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  return;
	}
      }
    }

    //
    // Sort selected rows in decreasing order
    //
    QList<int> rowtable;
    for(int i=0;i<rows.size();i++) {
      rowtable.push_back(i);
    }
    bool modified;
    do {
      modified=false;
      for(int i=0;i<(rows.size()-1);i++) {
	if(rows.at(rowtable.at(i))<rows.at(rowtable.at(i+1))) {
	  rowtable.swap(i,i+1);
	  modified=true;
	}
      }
    } while(modified);

    //
    // Do it!
    //
    LockList();
    for(int i=0;i<rows.size();i++) {
      QString username;
      QString stationname;
      QHostAddress addr;
      QModelIndex row=rows.at(rowtable.at(i));
      RDLogLock *log_lock=
	new RDLogLock(log_log_model->logName(row),rda->user(),
		      rda->station(),this);
      if(log_lock->tryLock(&username,&stationname,&addr)) {
	RDLog *log=new RDLog(log_log_model->logName(row));
	if(log->remove(rda->station(),rda->user(),rda->config())) {
	  SendNotification(RDNotification::DeleteAction,log->name());
	  log_log_model->removeLog(row);
	}
	else {
	  QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),
			       tr("Unable to delete log")+" \""+
			       log_log_model->logName(row)+"\", "+
			       tr("audio deletion error!"));
	}
	delete log;
      }
      else {
	QString msg=
	  tr("Log")+" "+log_log_model->logName(row)+"\" "+
	  tr("is in use by")+" "+username+"@"+stationname;
	if(stationname!=addr.toString()) {
	  msg+=" ["+addr.toString()+"]";
	}
	QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),msg);
      }
      delete log_lock;
    }
    UnlockList();
  }
}


void MainWidget::trackData()
{
  QModelIndex row=SingleSelectedRow();

  if(!row.isValid()) {
    return;
  }
  LockList();
  log_tracker_dialog->exec(log_log_model->logName(row));
  log_log_model->refresh(row);
  UnlockList();
}

/*
void MainWidget::trackData()
{
  QModelIndex row=SingleSelectedRow();

  if(!row.isValid()) {
    return;
  }
  LockList();
  VoiceTracker *dialog=
    new VoiceTracker(log_log_model->logName(row),&log_import_path);
  dialog->exec();
  delete dialog;
  log_log_model->refresh(row);
  UnlockList();
}
*/

void MainWidget::reportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  //
  // Generate Header
  //
  report=RDReport::center("Rivendell Log Listing",135)+"\n";
  report+=QString("Generated: ")+QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"\n";
  report+="\n";
  report+="Rdy -Log Name-------------------- -Description----------------- -Service------------ Mus Tfc Tracks- Start Date- -End Date-- -Mod Date--\n";

  //
  // Report Body
  //
  sql=QString("select ")+
    "`NAME`,"+               // 00
    "`DESCRIPTION`,"+        // 01
    "`SERVICE`,"+            // 02
    "`MUSIC_LINKS`,"+        // 03
    "`MUSIC_LINKED`,"+       // 04
    "`TRAFFIC_LINKS`,"+      // 05
    "`TRAFFIC_LINKED`,"+     // 06
    "`COMPLETED_TRACKS`,"+   // 07
    "`SCHEDULED_TRACKS`,"+   // 08
    "`START_DATE`,"+         // 09
    "`END_DATE`,"+           // 10
    "`MODIFIED_DATETIME` "+  // 11
    "from `LOGS` order by `NAME` ";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Summary Status
    //
    if(((q->value(3).toInt()==0)||(q->value(4).toString()=="Y"))&&
       ((q->value(5).toInt()==0)||(q->value(6).toString()=="Y"))&&
       (q->value(7).toUInt()==q->value(8).toUInt())) {
      report+=" Y  ";
    }
    else {
      report+=" N  ";
    }

    //
    // Log Name
    //
    report+=RDReport::leftJustify(q->value(0).toString(),29)+" ";
    
    //
    // Description
    //
    report+=RDReport::leftJustify(q->value(1).toString(),29)+" ";

    //
    // Service
    //
    report+=RDReport::leftJustify(q->value(2).toString(),20)+" ";

    //
    // Music Linked
    //
    if(q->value(3).toInt()>0) {
      report+=QString(" ")+q->value(4).toString()+"  ";
    }
    else {
      report+="n/a ";
    }

    //
    // Traffic Linked
    //
    if(q->value(5).toInt()>0) {
      report+=QString(" ")+q->value(6).toString()+"  ";
    }
    else {
      report+="n/a ";
    }

    //
    // Voice Tracks
    //
    report+=
      QString::asprintf("%3u/%-3u ",q->value(7).toUInt(),q->value(8).toUInt());
    
    //
    // Start Date
    //
    if(q->value(9).toDate().isNull()) {
      report+="[none]      ";
    }
    else {
      report+=RDReport::leftJustify(rda->shortDateString(q->value(9).toDate())+" ",12);
    }
    
    //
    // End Date
    //
    if(q->value(10).toDate().isNull()) {
      report+="[none]      ";
    }
    else {
      report+=RDReport::leftJustify(rda->shortDateString(q->value(10).toDate())+" ",12);
    }
    
    //
    // Last Modified Date
    //
    report+=rda->shortDateString(q->value(11).toDate());

    //
    // End of Line
    //
    report+="\n";
  }
  delete q;

  RDTextFile(report);
}


void MainWidget::selectionChangedData(const QItemSelection &selected,
				      const QItemSelection &deselected)
{
  int count=log_log_view->selectionModel()->selectedRows().size();

  log_edit_button->setEnabled(count==1);
  log_delete_button->setEnabled(count>0&&rda->user()->deleteLog());
  log_track_button->setEnabled(rda->system()->allowDuplicateCartTitles()&&
			       (count==1)&&rda->user()->voicetrackLog());
}


void MainWidget::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void MainWidget::notificationReceivedData(RDNotification *notify)
{
  if(notify->type()==RDNotification::LogType) {
    QString logname=notify->id().toString();
    switch(notify->action()) {
    case RDNotification::AddAction:
      log_log_model->addLog(logname);
      break;

    case RDNotification::ModifyAction:
      log_log_model->refresh(logname);
      break;

    case RDNotification::DeleteAction:
      if(log_list_locked) {
	log_deleted_logs.push_back(logname);
      }
      else {
	log_log_model->removeLog(logname);
      }
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainWidget::quitMainWidget()
{
  saveSettings();
  exit(0);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  quitMainWidget();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if((log_log_view==NULL)||(!log_resize)) {
    return;
  }
  log_filter_widget->setGeometry(10,10,size().width()-10,
				 log_filter_widget->sizeHint().height());

  log_log_view->setGeometry(10,log_filter_widget->sizeHint().height(),
			    size().width()-20,size().height()-
			    log_filter_widget->sizeHint().height()-65);
  log_add_button->setGeometry(10,size().height()-55,80,50);
  log_edit_button->setGeometry(100,size().height()-55,80,50);
  log_delete_button->setGeometry(190,size().height()-55,80,50);
  log_track_button->setGeometry(300,size().height()-55,80,50);
  log_report_button->setGeometry(400,size().height()-55,80,50);
  log_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
}


void MainWidget::SendNotification(RDNotification::Action action,
				  const QString &logname)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,action,
					    QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void MainWidget::LockList()
{
  log_list_locked=true;
}


void MainWidget::UnlockList()
{
  for(int i=0;i<log_deleted_logs.size();i++) {
    log_log_model->removeLog(log_deleted_logs.at(i));
  }
  log_deleted_logs.clear();
  log_list_locked=false;
}


QModelIndex MainWidget::SingleSelectedRow() const
{
  if(log_log_view->selectionModel()->selectedRows().size()!=1) {
    return QModelIndex();
  }
  return log_log_view->selectionModel()->selectedRows().at(0);
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
