// rdlogedit.cpp
//
// The Log Editor Utility for Rivendell.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_log.h"
#include "globals.h"
#include "rdlogedit.h"
#include "voice_tracker.h"

//
// Global Resources
//
bool global_import_running=false;
QSize global_top_window_size;
QSize global_logedit_window_size;
int global_start_time_style;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
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
  LoadPositions();

  //
  // Open the Database
  //
  rda=new RDApplication("RDLogEdit","rdlogedit",RDLOGEDIT_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),err_msg);
    exit(1);
  }
  log_import_path=RDGetHomeDir();

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
  log_filter_widget=
    new RDLogFilter(RDLogFilter::UserFilter,this);
  connect(log_filter_widget,SIGNAL(filterChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Log List
  //
  log_log_view=new RDTableView(this);
  log_log_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  log_log_model=new RDLogListModel(this);
  log_log_model->setFont(defaultFont());
  log_log_model->setPalette(palette());
  log_log_view->setModel(log_log_model);
  log_log_view->resizeColumnsToContents();
  connect(log_filter_widget,SIGNAL(filterChanged(const QString &)),
	  log_log_model,SLOT(setFilterSql(const QString &)));
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
  return global_top_window_size;
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
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
  log_log_model->setFilterSql(log_filter_widget->whereSql());
  log_log_view->resizeColumnsToContents();
  //  RefreshList();

  //
  // Set Control Perms
  //
  log_add_button->setEnabled(rda->user()->createLog());
  log_delete_button->setEnabled(false);
  log_track_button->setEnabled(false);
}


void MainWidget::recentData(bool state)
{
  //  RefreshList();
}


void MainWidget::addData()
{
  QString logname;
  QString svcname;
  QStringList newlogs;
  RDAddLog *log;
  QModelIndex row;

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
    EditLog *editlog=new EditLog(logname,&log_filter,&log_group,&log_schedcode,
				 &log_clipboard,&newlogs,this);
    editlog->exec();
    delete editlog;

    row=log_log_model->addLog(logname);
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
  EditLog *log=
    new EditLog(logname,&log_filter,&log_group,&log_schedcode,
		&log_clipboard,&newlogs,this);
  if(log->exec()) {
    log_log_model->refresh(row);
  }
  UnlockList();
  delete log;
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
			       QString().sprintf(" %u ",tracks)+
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
			       QString().sprintf(" %d ",rows.size())+
			       tr("logs?"),QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	return;
      }
      if(tracks>0) {
	if(QMessageBox::question(this,"RDLogEdit - "+tr("Tracks Exist"),
				 tr("This will also delete the")+
			       QString().sprintf(" %u ",tracks)+
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
  VoiceTracker *dialog=
    new VoiceTracker(log_log_model->logName(row),&log_import_path);
  dialog->exec();
  delete dialog;
  log_log_model->refresh(row);
  UnlockList();
}


void MainWidget::reportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  //
  // Generate Header
  //
  report=RDReport::center("Rivendell Log Listing",132)+"\n";
  report+=QString("Generated: ")+QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"\n";
  report+="\n";
  report+="Rdy -Log Name-------------------- -Description----------------- -Service------------ Mus Tfc Tracks- Start Date -End Date- -Mod Date-\n";

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
      QString().sprintf("%3u/%3u ",q->value(8).toUInt(),q->value(7).toUInt());
    
    //
    // Start Date
    //
    if(q->value(9).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=q->value(9).toDate().toString("MM/dd/yyyy")+" ";
    }
    
    //
    // End Date
    //
    if(q->value(10).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=q->value(10).toDate().toString("MM/dd/yyyy")+" ";
      report+=QString().sprintf("%s ",q->value(10).toDate().
				toString("MM/dd/yyyy").toUtf8().constData());
    }
    
    //
    // Last Modified Date
    //
    report+=q->value(11).toDate().toString("MM/dd/yyyy");

    //
    // End of Line
    //
    report+="\n";
  }
  delete q;

  RDTextFile(report);
}


void MainWidget::filterChangedData(const QString &str)
{
  //  RefreshList();
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
  SavePositions();
  exit(0);
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

  global_top_window_size=e->size();
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
  // Load Translations
  //
  QString tr_path;
  QString qt_path;

  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    tr_path=QString(PREFIX)+QString("/share/rivendell/");
    qt_path=QString("/usr/share/qt4/translation/");

    QTranslator qt(0);
    qt.load(qt_path+QString("qt_")+loc,".");
    a.installTranslator(&qt);

    QTranslator rd(0);
    rd.load(tr_path+QString("librd_")+loc,".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(tr_path+QString("librdhpi_")+loc,".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(tr_path+QString("rdlogedit_")+loc,".");
    a.installTranslator(&tr);
  }

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->setGeometry(QRect(QPoint(w->geometry().x(),w->geometry().y()),
		 w->sizeHint()));
  w->show();
  return a.exec();
}


void MainWidget::LoadPositions() const
{
  QString filename=RDHomeDir()+"/"+RDLOGEDIT_POSITION_FILENAME;
  RDProfile *p=new RDProfile();
  p->setSource(filename);
  global_top_window_size=
    QSize(p->intValue("RDLogEdit","TopWindowWidth",RDLOGEDIT_DEFAULT_WIDTH),
	  p->intValue("RDLogEdit","TopWindowHeight",RDLOGEDIT_DEFAULT_HEIGHT));
  global_logedit_window_size=
    QSize(p->intValue("RDLogEdit","LogeditWindowWidth",RDLOGEDIT_EDITLOG_DEFAULT_WIDTH),
	  p->intValue("RDLogEdit","LogeditWindowHeight",RDLOGEDIT_EDITLOG_DEFAULT_HEIGHT));
  global_start_time_style=p->intValue("RDLogEdit","StartTimeStyle",0);

  delete p;
}


void MainWidget::SavePositions() const
{
  QString filename=RDHomeDir()+"/"+RDLOGEDIT_POSITION_FILENAME;
  QString temp_filename=RDHomeDir()+"/"+RDLOGEDIT_POSITION_FILENAME+"-temp";
  FILE *f=NULL;

  if((f=fopen(temp_filename.toUtf8(),"w"))!=NULL) {
    fprintf(f,"[RDLogEdit]\n");
    fprintf(f,"TopWindowWidth=%d\n",global_top_window_size.width());
    fprintf(f,"TopWindowHeight=%d\n",global_top_window_size.height());
    fprintf(f,"LogeditWindowWidth=%d\n",global_logedit_window_size.width());
    fprintf(f,"LogeditWindowHeight=%d\n",global_logedit_window_size.height());
    fprintf(f,"StartTimeStyle=%d\n",global_start_time_style);
    fclose(f);
    rename(temp_filename.toUtf8(),filename.toUtf8());
  }
}
