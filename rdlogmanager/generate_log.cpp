// generate_log.cpp
//
// Generate a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: generate_log.cpp,v 1.37.6.2 2014/01/10 21:59:32 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qdatetimeedit.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtimer.h>

#include <rddb.h>
#include <rddatedialog.h>
#include <rdsvc.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rd.h>
#include <rdexception_dialog.h>
#include <rdtextfile.h>
#include <rdcreate_log.h>

#include <generate_log.h>
#include <edit_grid.h>
#include <globals.h>

//
// Icons
//
#include "../icons/whiteball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"


GenerateLog::GenerateLog(QWidget *parent,const char *name,int cmd_switch,QString *cmd_service,QDate *cmd_date)
  : QDialog(parent,name,true)
{
  QStringList services_list;
  bool  cmdservicefit=false;
  cmdswitch=cmd_switch;
  cmdservice = cmd_service;
  cmddate = cmd_date;

  QString str1=tr("Generate Log - User: ");
  setCaption(QString().sprintf("%s%s",(const char *)str1,
			       (const char *)rdripc->user()));

  gen_music_enabled=false;
  gen_traffic_enabled=false;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Create Icons
  //
  gen_whiteball_map=new QPixmap(whiteball_xpm);
  gen_greenball_map=new QPixmap(greenball_xpm);
  gen_redball_map=new QPixmap(redball_xpm);

  //
  // Progress Dialog
  //
  gen_progress_dialog=new QProgressDialog(tr("Generating Log..."),tr("Cancel"),
					  24,this,"gen_progress_dialog",true);
  gen_progress_dialog->setCaption("Progress");
  gen_progress_dialog->setCancelButton(NULL);

  //
  // Service Name
  //
  gen_service_box=new QComboBox(this,"gen_service_box");
  gen_service_box->setGeometry(70,10,sizeHint().width()-80,20);
  connect(gen_service_box,SIGNAL(activated(int)),
	  this,SLOT(serviceActivatedData(int)));
  QLabel *label=new QLabel(gen_service_box,tr("Service:"),
			   this,"gen_service_label");
  label->setGeometry(10,10,55,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec) {
    services_list = rduser->services();
  } else { // RDStation::HostSec
    QString sql="select NAME from SERVICES";
    RDSqlQuery *q=new RDSqlQuery(sql);
    while(q->next()) {
      services_list.append( q->value(0).toString() );
    }
    delete q;
  }
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
  gen_date_edit=new QDateEdit(this,"gen_date_edit");
  gen_date_edit->setGeometry(70,38,100,20);
  label=new QLabel(gen_date_edit,tr("Date:"),
			   this,"gen_date_label");
  label->setGeometry(10,38,55,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);
  if (cmdswitch==0)
  gen_date_edit->setDate(QDate::currentDate().addDays(1));
  else
    gen_date_edit->setDate(*cmddate);

  connect(gen_date_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

  //
  // Date Select Button
  //
  QPushButton *button=new QPushButton(this,"select_button");
  button->setGeometry(180,33,50,30);
  button->setFont(bold_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectDateData()));

  //
  //  Create Log Button
  //
  gen_create_button=new QPushButton(this,"gen_create_button");
  gen_create_button->setGeometry(10,70,sizeHint().width()-20,30);
  gen_create_button->setFont(bold_font);
  gen_create_button->setText(tr("&Create New Log"));
  connect(gen_create_button,SIGNAL(clicked()),this,SLOT(createData()));

  //
  //  Merge Music Log Button
  //
  gen_music_button=new QPushButton(this,"gen_music_button");
  gen_music_button->setGeometry(10,130,100,30);
  gen_music_button->setFont(bold_font);
  gen_music_button->setText(tr("Merge &Music"));
  connect(gen_music_button,SIGNAL(clicked()),this,SLOT(musicData()));

  //
  //  Merge Traffic Log Button
  //
  gen_traffic_button=new QPushButton(this,"gen_traffic_button");
  gen_traffic_button->setGeometry(10,170,100,30);
  gen_traffic_button->setFont(bold_font);
  gen_traffic_button->setText(tr("Merge &Traffic"));
  connect(gen_traffic_button,SIGNAL(clicked()),this,SLOT(trafficData()));

  //
  // Status Lights
  //
  // Headers
  //
  label=new QLabel(tr("Import Data"),this);
  label->setGeometry(120,105,120,14);
  label->setFont(bold_font);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Available"),this);
  label->setGeometry(120,119,60,14);
  label->setFont(small_font);
  label->setAlignment(AlignCenter);

  label=new QLabel(tr("Merged"),this);
  label->setGeometry(180,119,60,14);
  label->setFont(small_font);
  label->setAlignment(AlignCenter);

  //
  // Music Indicators
  //
  gen_mus_avail_label=new QLabel(this,"gen_mus_avail_label");
  gen_mus_avail_label->setPixmap(*gen_whiteball_map);
  gen_mus_avail_label->setGeometry(120,139,60,14);
  gen_mus_avail_label->setFont(small_font);
  gen_mus_avail_label->setAlignment(AlignCenter);

  gen_mus_merged_label=new QLabel(this,"gen_mus_merged_label");
  gen_mus_merged_label->setPixmap(*gen_whiteball_map);
  gen_mus_merged_label->setGeometry(180,139,60,14);
  gen_mus_merged_label->setFont(small_font);
  gen_mus_merged_label->setAlignment(AlignCenter);

  //
  // Traffic Indicators
  //
  gen_tfc_avail_label=new QLabel(this,"gen_tfc_avail_label");
  gen_tfc_avail_label->setPixmap(*gen_whiteball_map);
  gen_tfc_avail_label->setGeometry(120,179,60,14);
  gen_tfc_avail_label->setFont(small_font);
  gen_tfc_avail_label->setAlignment(AlignCenter);

  gen_tfc_merged_label=new QLabel(this,"gen_tfc_merged_label");
  gen_tfc_merged_label->setPixmap(*gen_whiteball_map);
  gen_tfc_merged_label->setGeometry(180,179,60,14);
  gen_tfc_merged_label->setFont(small_font);
  gen_tfc_merged_label->setAlignment(AlignCenter);


  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(10,sizeHint().height()-60,sizeHint().width()-20,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("C&lose"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  UpdateControls();

  //
  // File Scan Timer
  //
  QTimer *timer=new QTimer(this,"file_scan_timer");
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
    new RDDateDialog(current_date.year(),current_date.year()+1,
		    this,"datedialog");
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
  QString str1;
  QString str2;
  unsigned tracks=0;

  //
  // Generate Log
  //
  RDSvc *svc=new RDSvc(gen_service_box->currentText(),this,"svc");
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date());
  RDLog *log=new RDLog(logname);
  if(log->exists()) {
    str1=QString(tr("The log for"));
    str2=QString(tr("already exists.  Recreating it\nwill remove any merged Music or Traffic events.\n\nRecreate?"));
    if(QMessageBox::question(this,tr("Log Exists"),
			     QString().sprintf("%s %s %s",(const char *)str1,
		 (const char *)gen_date_edit->date().toString("MM/dd/yyyy"),
					       (const char *)str2),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete log;
      delete svc;
      return;
    }
    if((tracks=log->completedTracks())>0) {
      str1=QString(tr("This will also delete the"));
      str2=QString(tr("voice tracks associated with this log.\nContinue?"));
      if(QMessageBox::warning(this,tr("Tracks Exist"),
			      QString().sprintf("%s %u %s",
						(const char *)str1,
						tracks,
						(const char *)str2),
			      QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	delete svc;
	return;
      }
    }
  }
  log->removeTracks(rdstation_conf,rduser,log_config);

  //
  // Scheduler
  //
  QString sql;
  RDSqlQuery *q;

  srand(QTime::currentTime().msec());
  sql=RDCreateStackTableSql(gen_service_box->currentText().replace(" ","_")); 

  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    fprintf(stderr,"SQL: %s\n",(const char *)sql);
    fprintf(stderr,"SQL Error: %s\n",
	    (const char *)q->lastError().databaseText());
    delete q;
  }
  delete q;

  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setProgress(int)));
  svc->generateLog(gen_date_edit->date(),
		   RDDateDecode(svc->nameTemplate(),gen_date_edit->date()),
		   RDDateDecode(svc->nameTemplate(),gen_date_edit->date().
				addDays(1)),&unused_report);
  log->updateTracks();
  delete log;
  delete svc;

  //
  // Generate Exception Report
  //
  RDLogEvent *event=
    new RDLogEvent(QString().sprintf("%s_LOG",(const char *)logname));
  event->load();
  if((event->validate(&report,gen_date_edit->date())==0)&&
     unused_report.isEmpty()) {
    QMessageBox::information(this,tr("No Errors"),tr("No exceptions found."));
  }
  else {
    RDTextFile(report+"\n\n"+unused_report);
  }
  delete event;

  UpdateControls();
}


void GenerateLog::musicData()
{
  unsigned tracks=0;

  RDSvc *svc=new RDSvc(gen_service_box->currentText(),this,"svc");
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date());
  RDLog *log=new RDLog(logname);
  if(((log->linkState(RDLog::SourceMusic)==RDLog::LinkDone)||
      (log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone))) {
    QString str1=QString(tr("The log for"));
    QString str2=QString(tr("already contains merged music and/or traffic data.\nRemerging it will remove this data.  Remerge?"));
    if(QMessageBox::question(this,tr("Music Exists"),
			     QString().sprintf("%s %s %s",(const char *)str1,
		 (const char *)gen_date_edit->date().toString("MM/dd/yyyy"),
					       (const char *)str2),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete log;
      delete svc;
      return;
    }
    if((tracks=log->completedTracks())>0) {
      str1=QString(tr("This will also delete the"));
      str2=QString(tr("voice tracks associated with this log.\nContinue?"));
      if(QMessageBox::warning(this,tr("Tracks Exist"),
			      QString().sprintf("%s %u %s",
						(const char *)str1,
						tracks,
						(const char *)str2),
			      QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	delete svc;
	return;
      }
    }
    log->removeTracks(rdstation_conf,rduser,log_config);
    svc->clearLogLinks(RDSvc::Traffic,gen_date_edit->date(),logname);
    svc->clearLogLinks(RDSvc::Music,gen_date_edit->date(),logname);
  }
  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setProgress(int)));
  QString report;
  svc->linkLog(RDSvc::Music,gen_date_edit->date(),logname,&report);
  delete log;
  delete svc;
  if(!report.isEmpty()) {
    RDTextFile(report);
  }
  UpdateControls();
}


void GenerateLog::trafficData()
{
  RDSvc *svc=new RDSvc(gen_service_box->currentText(),this,"svc");
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date());
  RDLog *log=new RDLog(logname);
  if((log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone)) {
    QString str1=QString(tr("The log for"));
    QString str2=QString(tr("already contains merged traffic data.  Remerging it\nwill remove this data.  Remerge?"));
    if(QMessageBox::question(this,tr("Traffic Exists"),
			     QString().sprintf("%s %s %s",(const char *)str1,
		 (const char *)gen_date_edit->date().toString("MM/dd/yyyy"),
					       (const char *)str2),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete log;
      delete svc;
      return;
    }
    svc->clearLogLinks(RDSvc::Traffic,gen_date_edit->date(),logname);
  }
  connect(svc,SIGNAL(generationProgress(int)),
	  gen_progress_dialog,SLOT(setProgress(int)));

  QString report;
  svc->linkLog(RDSvc::Traffic,gen_date_edit->date(),logname,&report);
  delete log;
  delete svc;
  if(!report.isEmpty()) {
    RDTextFile(report);
  }
  UpdateControls();
}


void GenerateLog::fileScanData()
{
  RDSvc *svc=new RDSvc(gen_service_box->currentText(),this,"svc");
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date());
  RDLog *log=new RDLog(logname);
  if(gen_music_enabled) {
    if(QFile::exists(svc->
		     importFilename(RDSvc::Music,gen_date_edit->date()))) {
      gen_music_button->setEnabled(true);
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
	setEnabled((!gen_music_enabled)||
		   (log->linkState(RDLog::SourceMusic)==RDLog::LinkDone));
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


void GenerateLog::UpdateControls()
{
  RDSvc *svc=new RDSvc(gen_service_box->currentText(),this,"svc");
  QString logname=RDDateDecode(svc->nameTemplate(),gen_date_edit->date());
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
