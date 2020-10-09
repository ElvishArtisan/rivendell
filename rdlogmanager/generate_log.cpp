// generate_log.cpp
//
// Generate a Rivendell Log
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

#include <rddatedecode.h>
#include <rddatedialog.h>
#include <rdlog.h>
#include <rdsvc.h>
#include <rdtextfile.h>

#include "edit_grid.h"
#include "generate_log.h"
#include "globals.h"

//
// Icons
//
#include "../icons/whiteball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"

GenerateLog::GenerateLog(QWidget *parent,int cmd_switch,QString *cmd_service,
			 QDate *cmd_date)
  : RDDialog(parent)
{
  QStringList services_list;
  bool  cmdservicefit=false;
  cmdswitch=cmd_switch;
  cmdservice = cmd_service;
  cmddate = cmd_date;

  setWindowTitle("RDLogManager - "+tr("Generate Log"));

  gen_music_enabled=false;
  gen_traffic_enabled=false;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Icons
  //
  gen_whiteball_map=new QPixmap(whiteball_xpm);
  gen_greenball_map=new QPixmap(greenball_xpm);
  gen_redball_map=new QPixmap(redball_xpm);

  //
  // Progress Dialog
  //
  gen_progress_dialog=
    new QProgressDialog(tr("Generating Log..."),tr("Cancel"),0,24,this);
  gen_progress_dialog->setCaption("Progress");
  gen_progress_dialog->setCancelButton(NULL);

  //
  // Service Name
  //
  gen_service_box=new QComboBox(this);
  connect(gen_service_box,SIGNAL(activated(int)),
	  this,SLOT(serviceActivatedData(int)));
  gen_service_label=new QLabel(gen_service_box,tr("Service:"),this);
  gen_service_label->setFont(labelFont());
  gen_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  QString sql="select NAME from SERVICES";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end();
        ++it ) {
    gen_service_box->insertItem(*it);
    if (cmdswitch != 0 && *cmdservice == *it)
      cmdservicefit=true;
  }

  //
  // Date
  //
  gen_date_edit=new QDateEdit(this);
  gen_date_edit->setDisplayFormat("MM/dd/yyyy");
  gen_date_label=new QLabel(gen_date_edit,tr("Date:"),this);
  gen_date_label->setFont(labelFont());
  gen_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  if (cmdswitch==0)
  gen_date_edit->setDate(QDate::currentDate().addDays(1));
  else
    gen_date_edit->setDate(*cmddate);

  connect(gen_date_edit,SIGNAL(dateChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

  //
  // Date Select Button
  //
  gen_select_button=new QPushButton(this);
  gen_select_button->setFont(subButtonFont());
  gen_select_button->setText(tr("&Select"));
  connect(gen_select_button,SIGNAL(clicked()),this,SLOT(selectDateData()));

  //
  //  Create Log Button
  //
  gen_create_button=new QPushButton(this);
  gen_create_button->setFont(buttonFont());
  gen_create_button->setText(tr("&Create New Log"));
  connect(gen_create_button,SIGNAL(clicked()),this,SLOT(createData()));

  //
  //  Merge Music Log Button
  //
  gen_music_button=new QPushButton(this);
  gen_music_button->setFont(buttonFont());
  gen_music_button->setText(tr("Merge &Music"));
  connect(gen_music_button,SIGNAL(clicked()),this,SLOT(musicData()));

  //
  //  Merge Traffic Log Button
  //
  gen_traffic_button=new QPushButton(this);
  gen_traffic_button->setFont(buttonFont());
  gen_traffic_button->setText(tr("Merge &Traffic"));
  connect(gen_traffic_button,SIGNAL(clicked()),this,SLOT(trafficData()));

  //
  // Status Lights
  //
  // Headers
  //
  gen_import_label=new QLabel(tr("Import Data"),this);
  gen_import_label->setFont(labelFont());
  gen_import_label->setAlignment(Qt::AlignCenter);

  gen_available_label=new QLabel(tr("Available"),this);
  gen_available_label->setFont(subLabelFont());
  gen_available_label->setAlignment(Qt::AlignCenter);

  gen_merged_label=new QLabel(tr("Merged"),this);
  gen_merged_label->setFont(subLabelFont());
  gen_merged_label->setAlignment(Qt::AlignCenter);

  //
  // Music Indicators
  //
  gen_mus_avail_label=new QLabel(this);
  gen_mus_avail_label->setPixmap(*gen_whiteball_map);
  gen_mus_avail_label->setFont(subLabelFont());
  gen_mus_avail_label->setAlignment(Qt::AlignCenter);

  gen_mus_merged_label=new QLabel(this);
  gen_mus_merged_label->setPixmap(*gen_whiteball_map);
  gen_mus_merged_label->setFont(subLabelFont());
  gen_mus_merged_label->setAlignment(Qt::AlignCenter);

  //
  // Traffic Indicators
  //
  gen_tfc_avail_label=new QLabel(this);
  gen_tfc_avail_label->setPixmap(*gen_whiteball_map);
  gen_tfc_avail_label->setFont(subLabelFont());
  gen_tfc_avail_label->setAlignment(Qt::AlignCenter);

  gen_tfc_merged_label=new QLabel(this);
  gen_tfc_merged_label->setPixmap(*gen_whiteball_map);
  gen_tfc_merged_label->setFont(subLabelFont());
  gen_tfc_merged_label->setAlignment(Qt::AlignCenter);


  //
  //  Close Button
  //
  gen_close_button=new QPushButton(this);
  gen_close_button->setDefault(true);
  gen_close_button->setFont(buttonFont());
  gen_close_button->setText(tr("C&lose"));
  connect(gen_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  UpdateControls();

  //
  // File Scan Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(fileScanData()));
  timer->start(GENERATE_LOG_FILESCAN_INTERVAL);
 
  if(cmdswitch==1 && cmdservicefit)
    {
    gen_service_box->setCurrentText(*cmdservice);
    createData();
    }
  if(cmdswitch==2 && cmdservicefit)
    {
    gen_service_box->setCurrentText(*cmdservice);
    musicData();
    }
  if(cmdswitch==3 && cmdservicefit)
    {
    gen_service_box->setCurrentText(*cmdservice);
    trafficData();
    }
}


QSize GenerateLog::sizeHint() const
{
  return QSize(240,270);
} 


QSizePolicy GenerateLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void GenerateLog::serviceActivatedData(int index)
{
  UpdateControls();
}


void GenerateLog::dateChangedData(const QDate &date)
{
  UpdateControls();
}


void GenerateLog::selectDateData()
{
  QDate date=gen_date_edit->date();
  QDate current_date=QDate::currentDate();

  RDDateDialog *datedialog=
    new RDDateDialog(current_date.year(),current_date.year()+1,this);
  if(datedialog->exec(&date)<0) {
    delete datedialog;
    return;
  }
  gen_date_edit->setDate(date);
  delete datedialog;
  UpdateControls();
}


void GenerateLog::createData()
{
  QString report;
  QString unused_report;
  unsigned tracks=0;
  QString err_msg;

  //
  // Generate Log
  //
  RDSvc *svc=
    new RDSvc(gen_service_box->currentText(),rda->station(),rda->config(),this);
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
			       rda->station(),rda->config(),svc->name());
  RDLog *log=new RDLog(logname);
  if(log->exists()) {
    if(QMessageBox::question(this,"RDLogManager - "+tr("Log Exists"),
			     tr("The log for")+" "+
			     gen_date_edit->date().toString("MM/dd/yyyy")+" "+
			     tr("already exists.  Recreating it")+"\n"+
			     tr("will remove any merged Music or Traffic events.")+
			     "\n\n"+tr("Recreate?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete log;
      delete svc;
      return;
    }
    if((tracks=log->completedTracks())>0) {
      if(QMessageBox::warning(this,"RDLogManager - "+tr("Tracks Exist"),
			      tr("This will also delete the")+
			      QString().sprintf(" %u ",tracks)+
			      tr("voice tracks associated with this log.")+
			      "\n"+tr("Continue?"),
			      QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	delete svc;
	return;
      }
    }
  }
  SendNotification(RDNotification::DeleteAction,log->name());
  log->removeTracks(rda->station(),rda->user(),rda->config());

  //
  // Scheduler
  //
  srand(QTime::currentTime().msec());
  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setValue(int)));
  if(!svc->generateLog(gen_date_edit->date(),
		       RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
				    rda->station(),rda->config(),svc->name()),
		       RDDateDecode(svc->nameTemplate(),gen_date_edit->date().
				    addDays(1),rda->station(),rda->config(),
				    svc->name()),
		       &unused_report,rda->user(),&err_msg)) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			 tr("Unable to generate log")+": "+err_msg);
    gen_progress_dialog->setValue(gen_progress_dialog->maximum());
    delete svc;
    delete log;
    return;
  }
  log->updateTracks();
  SendNotification(RDNotification::AddAction,log->name());
  delete log;
  delete svc;

  //
  // Generate Exception Report
  //
  RDLogEvent *event=new RDLogEvent(logname);
  event->load();
  if((event->validate(&report,gen_date_edit->date())==0)&&
     unused_report.isEmpty()) {
    QMessageBox::information(this,tr("No Errors"),\
      tr("No broken rules or validation exceptions found."));
  }
  else {
    int errs=unused_report.count("\n"); 
    if(errs==1) {
      unused_report+=QString().sprintf("\n%d broken rule.\n",errs);
    }
    else {
      unused_report+=QString().sprintf("\n%d broken rules.\n",errs);
    }

    RDTextFile(report+"\n"+unused_report);
  }
  delete event;

  UpdateControls();
}


void GenerateLog::musicData()
{
  unsigned tracks=0;
  QString err_msg;

  RDSvc *svc=
    new RDSvc(gen_service_box->currentText(),rda->station(),rda->config(),this);
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
			       rda->station(),rda->config(),svc->name());
  RDLog *log=new RDLog(logname);
  if(((log->linkState(RDLog::SourceMusic)==RDLog::LinkDone)||
      (log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone))) {
    if(log->includeImportMarkers()) {
      if(QMessageBox::question(this,"RDLogManager - "+tr("Music Exists"),
			       tr("The log for")+" "+
			       gen_date_edit->date().toString("MM/dd/yyyy")+" "+
			       tr("already contains merged music and/or traffic data.")+"\n"+
			       tr("Remerging it will remove this data.  Remerge?"),
			       QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	delete svc;
	return;
      }
      if((tracks=log->completedTracks())>0) {
	if(QMessageBox::warning(this,"RDLogManager - "+tr("Tracks Exist"),
				tr("This will also delete the")+
				QString().sprintf(" %u ",tracks)+
				tr("voice tracks associated with this log.")+
				"\n"+tr("Continue?"),
				QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  delete log;
	  delete svc;
	  return;
	}
      }
    }
    else {
      QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			   tr("The log for")+" "+
			   gen_date_edit->date().toString("MM/dd/yyyy")+" "+
			   tr("cannot be relinked."));
      return;
    }
    log->removeTracks(rda->station(),rda->user(),rda->config());
    if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
      QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			   tr("Unable to clear traffic links")+": "+err_msg);
      delete log;
      delete svc;
      return;
    }
    if(!svc->clearLogLinks(RDSvc::Music,logname,rda->user(),&err_msg)) {
      QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			   tr("Unable to clear music links")+": "+err_msg);
      delete log;
      delete svc;
      return;
    }
  }
  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setValue(int)));
  QString report;
  if(!svc->linkLog(RDSvc::Music,gen_date_edit->date(),logname,&report,
		   rda->user(),&err_msg)) {
    gen_progress_dialog->setValue(gen_progress_dialog->maximum());
    RDTextFile(tr("RDLogManager Error Report")+"\n\n"+
	       tr("Music schedule import failed!")+"\n\n"+err_msg);
    delete log;
    delete svc;
    UpdateControls();
    return;
  }
  SendNotification(RDNotification::ModifyAction,log->name());
  delete log;
  delete svc;
  if(!report.isEmpty()) {
    RDTextFile(report);
  }
  UpdateControls();
}


void GenerateLog::trafficData()
{
  QString err_msg;
  RDSvc *svc=
    new RDSvc(gen_service_box->currentText(),rda->station(),rda->config(),this);
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
			       rda->station(),rda->config(),svc->name());
  RDLog *log=new RDLog(logname);
  if((log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone)) {
    if(log->includeImportMarkers()) {
      if(QMessageBox::question(this,"RDLogManager - "+tr("Traffic Exists"),
			       tr("The log for")+" "+
			       gen_date_edit->date().toString("MM/dd/yyyy")+" "+
			       tr("already contains merged traffic data.")+"\n"+
			       tr("Remerging it will remove this data.  Remerge?"),
			       QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	delete svc;
	return;
      }
    }
    else {
      QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			   tr("The log for")+" "+
			   gen_date_edit->date().toString("MM/dd/yyyy")+" "+
			   tr("cannot be relinked."));
      return;
    }
    if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
      QMessageBox::warning(this,"RDLogManager - "+tr("Error"),
			   tr("Unable to clear traffic links")+": "+err_msg);
      delete log;
      delete svc;
      return;
    }
  }
  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setValue(int)));

  QString report;
  if(!svc->linkLog(RDSvc::Traffic,gen_date_edit->date(),logname,&report,rda->user(),
		   &err_msg)) {
    gen_progress_dialog->setValue(gen_progress_dialog->maximum());
    RDTextFile(tr("RDLogManager Error Report")+"\n\n"+
	       tr("Traffic schedule import failed!")+"\n\n"+err_msg);
    delete log;
    delete svc;
    UpdateControls();
    return;
  }
  SendNotification(RDNotification::ModifyAction,log->name());
  delete log;
  delete svc;
  if(!report.isEmpty()) {
    RDTextFile(report);
  }
  UpdateControls();
}


void GenerateLog::fileScanData()
{
  RDSvc *svc=
    new RDSvc(gen_service_box->currentText(),rda->station(),rda->config(),this);
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
			       rda->station(),rda->config(),svc->name());
  RDLog *log=new RDLog(logname);
  if(gen_music_enabled) {
    if(QFile::exists(svc->
		     importFilename(RDSvc::Music,gen_date_edit->date()))) {
      gen_music_button->setEnabled(log->includeImportMarkers()||
				   (log->linkState(RDLog::SourceMusic)==
				    RDLog::LinkMissing));
      gen_mus_avail_label->setPixmap(*gen_greenball_map);
    }
    else {
      gen_music_button->setDisabled(true);
      gen_mus_avail_label->setPixmap(*gen_redball_map);
    }
  }
  else {
    gen_mus_avail_label->setPixmap(*gen_whiteball_map);
  }
  if(gen_traffic_enabled) {
    if(QFile::exists(svc->
		     importFilename(RDSvc::Traffic,gen_date_edit->date()))) {
      gen_traffic_button->
	setEnabled(((!gen_music_enabled)||
		    (log->linkState(RDLog::SourceMusic)==RDLog::LinkDone))&&
		   (log->includeImportMarkers()||
		    (log->linkState(RDLog::SourceTraffic)==
		     RDLog::LinkMissing)));
      gen_tfc_avail_label->setPixmap(*gen_greenball_map);
    }
    else {
      gen_traffic_button->setDisabled(true);
      gen_tfc_avail_label->setPixmap(*gen_redball_map);
    }
  }
  else {
    gen_tfc_avail_label->setPixmap(*gen_whiteball_map);
  }
  delete log;
  delete svc;
}


void GenerateLog::closeData()
{
  done(0);
}


void GenerateLog::resizeEvent(QResizeEvent *e)
{
  gen_service_box->setGeometry(70,10,sizeHint().width()-80,20);
  gen_service_label->setGeometry(10,10,55,20);
  gen_date_edit->setGeometry(70,38,100,20);
  gen_date_label->setGeometry(10,38,55,20);
  gen_select_button->setGeometry(180,33,50,30);
  gen_create_button->setGeometry(10,70,sizeHint().width()-20,30);
  gen_music_button->setGeometry(10,130,100,30);
  gen_traffic_button->setGeometry(10,170,100,30);
  gen_import_label->setGeometry(120,105,120,14);
  gen_available_label->setGeometry(120,119,60,14);
  gen_merged_label->setGeometry(180,119,60,14);
  gen_mus_avail_label->setGeometry(120,139,60,14);
  gen_mus_merged_label->setGeometry(180,139,60,14);
  gen_tfc_avail_label->setGeometry(120,179,60,14);
  gen_tfc_merged_label->setGeometry(180,179,60,14);
  gen_close_button->
    setGeometry(10,sizeHint().height()-60,sizeHint().width()-20,50);
}


void GenerateLog::UpdateControls()
{
  RDSvc *svc=
    new RDSvc(gen_service_box->currentText(),rda->station(),rda->config(),this);
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date(),
			       rda->station(),rda->config(),svc->name());
  RDLog *log=new RDLog(logname);
  if(log->exists()) {
    if(log->linkQuantity(RDLog::SourceMusic)>0) {
      gen_music_enabled=true;
      if(log->linkState(RDLog::SourceMusic)==RDLog::LinkDone) {
	gen_mus_merged_label->setPixmap(*gen_greenball_map);
      }
      else {
	gen_mus_merged_label->setPixmap(*gen_redball_map);
      }
    }
    else {
      gen_music_enabled=false;
      gen_music_button->setDisabled(true);
      gen_mus_merged_label->setPixmap(*gen_whiteball_map);
    }
    if(log->linkQuantity(RDLog::SourceTraffic)>0) {
      gen_traffic_enabled=true;
      if(log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone) {
	gen_tfc_merged_label->setPixmap(*gen_greenball_map);
      }
      else {
	gen_tfc_merged_label->setPixmap(*gen_redball_map);
      }
    }
    else {
      gen_traffic_enabled=false;
      gen_traffic_button->setDisabled(true);
      gen_tfc_merged_label->setPixmap(*gen_whiteball_map);
    }
  }
  else {
    gen_music_button->setDisabled(true);
    gen_mus_merged_label->setPixmap(*gen_whiteball_map);
    gen_traffic_button->setDisabled(true);
    gen_tfc_merged_label->setPixmap(*gen_whiteball_map);
    gen_music_enabled=false;
    gen_traffic_enabled=false;
  }
  delete log;
  delete svc;
  fileScanData();
}


void GenerateLog::SendNotification(RDNotification::Action action,
				   const QString &logname)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}
