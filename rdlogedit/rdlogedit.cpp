// rdlogedit.cpp
//
// The Log Editor Utility for Rivendell.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogedit.cpp,v 1.77.4.9.2.1 2014/05/21 18:19:43 cvs Exp $
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

#ifndef WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif  // WIN32
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qsqlpropertymap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <rd.h>
#include <rdconf.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rdcheck_daemons.h>
#include <rdcreate_log.h>
#include <rdadd_log.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdtextfile.h>
#include <rdmixer.h>
#include <dbversion.h>
#include <rdescape_string.h>

#include <rdlogedit.h>
#include <edit_log.h>
#include <globals.h>
#ifndef WIN32
#include <voice_tracker.h>
#endif  // WIN32

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"

//
// Global Resources
//
RDStation *rdstation_conf;
RDUser *rduser;
RDRipc *rdripc;
RDConfig *log_config;
RDLogeditConf *rdlogedit_conf;
RDSystem *rdsystem;
RDCartDialog *log_cart_dialog;
bool import_running=false;
#ifndef WIN32
RDCae *rdcae;


void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
      pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    import_running=false;
    signal(SIGCHLD,SigHandler);
    return;
  }
}
#endif  // WIN32


MainWidget::MainWidget(QWidget *parent,const char *name,WFlags f)
  :QMainWindow(parent,name,f)
{
  QString str1;
  QString str2;
  log_log_list=NULL;
  bool skip_db_check=false;
  unsigned schema=0;
  QString sql;
  RDSqlQuery *q;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdlogedit","\n");
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }
  delete cmd;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Ensure that the system daemons are running
  //
#ifndef WIN32
  RDInitializeDaemons();
#endif  // WIN32

  //
  // Load Local Configs
  //
  log_config=new RDConfig();
  log_config->load();
  str1=QString("RDLogEdit")+"v"+VERSION+" - "+tr("Host");
  str2=tr("User")+": ["+tr("Unknown")+"]";
  setCaption(QString().sprintf("%s: %s, %s",(const char *)str1,
			       (const char *)log_config->stationName(),
			       (const char *)str2));
  log_import_path=RDGetHomeDir();

  //
  // Open Database
  //
  QString err;
  log_db=RDInitDb(&schema,&err);
  if(!log_db) {
    QMessageBox::warning(this,tr("Can't Connect"),err);
    exit(0);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
#ifdef WIN32
	    QMessageBox::warning(this,tr("RDLogEdit -- Database Skew"),
				 tr("This version of RDLogEdit is incompatible with the version installed on the server.\nSee your system administrator for an update!"));
#else
    fprintf(stderr,
	    "rdlogedit: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
#endif  // WIN32
    exit(256);
  }

  //
  // Allocate Global Resources
  //
  rdstation_conf=new RDStation(log_config->stationName());

  //
  // CAE Connection
  //
#ifndef WIN32
  rdcae=new RDCae(rdstation_conf,log_config,parent,name);
  rdcae->connectHost();
#endif  // WIN32

  //
  // RIPC Connection
  //
#ifndef WIN32
  rdripc=new RDRipc(log_config->stationName());
  connect(rdripc,SIGNAL(connected(bool)),this,SLOT(connectedData(bool)));
  connect(rdripc,SIGNAL(userChanged()),this,SLOT(userData()));
  rdripc->connectHost("localhost",RIPCD_TCP_PORT,log_config->password());
#else
  rdripc=NULL;
#endif  // WIN32

  //
  // System Configuration
  //
  rdsystem=new RDSystem();

  //
  // RDLogEdit Configuration
  //
  rdlogedit_conf=new RDLogeditConf(log_config->stationName());

  //
  // Cart Picker
  //
#ifdef WIN32
  log_cart_dialog=new RDCartDialog(&log_filter,&log_group,&log_schedcode,
				   NULL,NULL,rdstation_conf,rdsystem,
				   log_config,this);
#else
  log_cart_dialog=new RDCartDialog(&log_filter,&log_group,&log_schedcode,
				   rdcae,rdripc,rdstation_conf,rdsystem,
				   log_config,this);
#endif

  //
  // User
  //
#ifndef WIN32
  rduser=NULL;

  //
  // Load Audio Assignments
  //
  RDSetMixerPorts(log_config->stationName(),rdcae);
#else 
  rduser=new RDUser(RD_USER_LOGIN_NAME);
#endif  // WIN32

  // 
  // Create Fonts
  //
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Create Icons
  //
  log_rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*log_rivendell_map);
  log_greencheckmark_map=new QPixmap(greencheckmark_xpm);
  log_redx_map=new QPixmap(redx_xpm);
  log_whiteball_map=new QPixmap(whiteball_xpm);
  log_greenball_map=new QPixmap(greenball_xpm);
  log_redball_map=new QPixmap(redball_xpm);

  //
  // Service Selector
  //
  log_service_box=new QComboBox(this);
  log_service_box->setFont(default_font);
  connect(log_service_box,SIGNAL(activated(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  log_service_box->insertItem(tr("ALL"));
  sql="select NAME from SERVICES order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    log_service_box->insertItem(q->value(0).toString());
  }
  delete q;
  log_service_label=new QLabel(log_service_box,tr("Service")+":",this);
  log_service_label->setFont(button_font);
  log_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Filter
  //
  log_filter_edit=new QLineEdit(this);
  log_filter_edit->setFont(default_font);
  connect(log_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  log_filter_label=new QLabel(log_filter_edit,tr("Filter")+":",this);
  log_filter_label->setFont(button_font);
  log_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  log_filter_button=new QPushButton(tr("Clear"),this);
  log_filter_button->setFont(button_font);
  connect(log_filter_button,SIGNAL(clicked()),this,SLOT(filterClearedData()));

  //
  // Log List
  //
  log_log_list=new QListView(this,"log_log_list");
  log_log_list->setFont(default_font);
  log_log_list->setAllColumnsShowFocus(true);
  log_log_list->setItemMargin(5);
  connect(log_log_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(logDoubleclickedData(QListViewItem *,const QPoint &,int)));
  log_log_list->addColumn("");
  log_log_list->setColumnAlignment(0,Qt::AlignCenter);
  log_log_list->addColumn(tr("LOG NAME"));
  log_log_list->setColumnAlignment(1,Qt::AlignHCenter);
  log_log_list->addColumn(tr("DESCRIPTION"));
  log_log_list->setColumnAlignment(2,Qt::AlignLeft);
  log_log_list->addColumn(tr("SERVICE"));
  log_log_list->setColumnAlignment(3,Qt::AlignLeft);
  log_log_list->addColumn(tr("MUSIC"));
  log_log_list->setColumnAlignment(4,Qt::AlignCenter);
  log_log_list->addColumn(tr("TRAFFIC"));
  log_log_list->setColumnAlignment(5,Qt::AlignCenter);
  log_log_list->addColumn(tr("TRACKS"));
  log_log_list->setColumnAlignment(6,Qt::AlignHCenter);
  log_log_list->addColumn(tr("VALID FROM"));
  log_log_list->setColumnAlignment(7,Qt::AlignHCenter);
  log_log_list->addColumn(tr("VALID TO"));
  log_log_list->setColumnAlignment(8,Qt::AlignHCenter);
  log_log_list->addColumn(tr("AUTO REFRESH"));
  log_log_list->setColumnAlignment(9,Qt::AlignHCenter);
  log_log_list->addColumn(tr("ORIGIN"));
  log_log_list->setColumnAlignment(10,Qt::AlignLeft);
  log_log_list->addColumn(tr("LAST LINKED"));
  log_log_list->setColumnAlignment(11,Qt::AlignLeft);
  log_log_list->addColumn(tr("LAST MODIFIED"));
  log_log_list->setColumnAlignment(12,Qt::AlignLeft);

  RefreshList();

  //
  // Add Button
  //
  log_add_button=new QPushButton(this,"log_add_button");
  log_add_button->setFont(button_font);
  log_add_button->setText(tr("&Add"));
  connect(log_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  log_edit_button=new QPushButton(this,"log_edit_button");
  log_edit_button->setFont(button_font);
  log_edit_button->setText(tr("&Edit"));
  connect(log_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  log_delete_button=new QPushButton(this,"log_delete_button");
  log_delete_button->setFont(button_font);
  log_delete_button->setText(tr("&Delete"));
  connect(log_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Tracker Button
  //
  log_track_button=new QPushButton(this,"log_track_button");
  log_track_button->setFont(button_font);
  log_track_button->setText(tr("Voice\n&Tracker"));
  connect(log_track_button,SIGNAL(clicked()),this,SLOT(trackData()));
#ifdef WIN32
  log_track_button->hide();
#endif

  //
  // Log Report Button
  //
  log_report_button=new QPushButton(this,"log_report_button");
  log_report_button->setFont(button_font);
  log_report_button->setText(tr("Log\nReport"));
  connect(log_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  // Close Button
  //
  log_close_button=new QPushButton(this,"log_close_button");
  log_close_button->setFont(button_font);
  log_close_button->setText(tr("&Close"));
  connect(log_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

#ifndef WIN32
  // 
  // Setup Signal Handling 
  //
  ::signal(SIGCHLD,SigHandler);
#endif  // WIN32
}


QSize MainWidget::sizeHint() const
{
  return QSize(640,480);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::connectedData(bool state)
{
}


void MainWidget::userData()
{
  QString str1;
  QString str2;

  str1=QString("RDLogEdit")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setCaption(str1+": "+log_config->stationName()+", "+str2+": "+
	     rdripc->user());
  if(rduser!=NULL) {
    delete rduser;
  }
  rduser=new RDUser(rdripc->user());

  //
  // Set Control Perms
  //
  log_add_button->setEnabled(rduser->createLog());
  log_delete_button->setEnabled(rduser->deleteLog());
  log_track_button->setEnabled(rduser->voicetrackLog());

  // Update the list of logs if applicable.
  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec) {
    RefreshList();
  }
}


void MainWidget::addData()
{
  QString logname;
  QString svcname;
  RDSqlQuery *q;
  QString sql;
  std::vector<QString> newlogs;
  RDAddLog *log;

  if(rduser->createLog()) {
    if (rdstation_conf->broadcastSecurity() == RDStation::UserSec) {
      log=new RDAddLog(&logname,&svcname,NULL,tr("Add Log"),this,"add_log",
                       rduser);
    } else { // RDStation::HostSec
      log=new RDAddLog(&logname,&svcname,NULL,tr("Add Log"),this,"add_log");
    }
    if(log->exec()!=0) {
      delete log;
      return;
    }
    delete log;
    sql=QString().sprintf("INSERT INTO LOGS SET NAME=\"%s\",TYPE=0,\
                           DESCRIPTION=\"%s log\",ORIGIN_USER=\"%s\",\
                           ORIGIN_DATETIME=NOW(),LINK_DATETIME=NOW(),\
                           SERVICE=\"%s\"",
			  (const char *)logname,
			  (const char *)logname,
#ifdef WIN32
			  RD_USER_LOGIN_NAME,
#else
			  (const char *)rdripc->user(),
#endif  // WIN32
			  (const char *)svcname);
    q=new RDSqlQuery(sql);
    if(!q->isActive()) {
      QMessageBox::warning(this,tr("Log Exists"),tr("Log Already Exists!"));
      delete q;
      return;
    }
    delete q;
    RDCreateLogTable(RDLog::tableName(logname));
    EditLog *editlog=new EditLog(logname,&log_clipboard,&newlogs,this);
    editlog->exec();
    delete editlog;
    ListListViewItem *item=new ListListViewItem(log_log_list);
    item->setText(1,logname);
    RefreshItem(item);
    log_log_list->setSelected(item,true);
    log_log_list->ensureItemVisible((QListViewItem *)item);
    for(unsigned i=0;i<newlogs.size();i++) {
      item=new ListListViewItem(log_log_list);
      item->setText(1,newlogs[i]);
      RefreshItem(item);
    }
  }
}


void MainWidget::editData()
{
  ListListViewItem *item=(ListListViewItem *)log_log_list->selectedItem();
  std::vector<QString> newlogs;

  if(item==NULL) {
    return;
  }
  EditLog *log=new EditLog(item->text(1),&log_clipboard,&newlogs,this);
  log->exec();
  delete log;
  RefreshItem(item);
  for(unsigned i=0;i<newlogs.size();i++) {
    item=new ListListViewItem(log_log_list);
    item->setText(1,newlogs[i]);
    RefreshItem(item);
  }
}


void MainWidget::deleteData()
{
  QString filename;
  QString str1;
  QString str2;
  unsigned tracks=0;
  QListViewItem *item=log_log_list->selectedItem();

  if(item==NULL) {
    return;
  }
  if(rduser->deleteLog()) {
    if(QMessageBox::question(this,tr("Delete Log"),
     tr(QString().sprintf("Are you sure you want to delete the \"%s\" log?",
			  (const char *)item->text(1))),
			     QMessageBox::Yes,
			     QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
    RDLog *log=new RDLog(item->text(1));
    if((tracks=log->completedTracks())>0) {
      str1=QString(tr("This will also delete the"));
      str2=QString(tr("voice tracks associated with this log.\nContinue?"));
      if(QMessageBox::question(this,tr("Tracks Exist"),
			       QString().sprintf("%s %u %s",
						 (const char *)str1,
						 tracks,
						 (const char *)str2),
			       QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete log;
	return;
      }
    }
    if(!log->remove(rdstation_conf,rduser,log_config)) {
      QMessageBox::warning(this,tr("RDLogEdit"),
			   tr("Unable to delete log, audio deletion error!"));
      delete log;
      return;
    }
    delete log;
    delete item;
  }
}


void MainWidget::trackData()
{
#ifndef WIN32
  ListListViewItem *item=(ListListViewItem *)log_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  VoiceTracker *dialog=new VoiceTracker(item->text(1),&log_import_path);
  dialog->exec();
  delete dialog;
  RefreshItem(item);
#endif  // WIN32
}


void MainWidget::reportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  //
  // Generate Header
  //
  report="                                                     Rivendell Log Listing\n";
  report+=QString().
    sprintf("Generated: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"));
  report+="\n";
  report+="Rdy -Log Name-------------------- -Description----------------- -Service------------ Mus Tfc Tracks- Start Date -End Date- -Mod Date-\n";

  //
  // Report Body
  //
  sql="select NAME,DESCRIPTION,SERVICE,MUSIC_LINKS,MUSIC_LINKED,\
       TRAFFIC_LINKS,TRAFFIC_LINKED,COMPLETED_TRACKS,SCHEDULED_TRACKS,\
       START_DATE,END_DATE,MODIFIED_DATETIME from LOGS order by NAME";
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
    report+=QString().sprintf("%-29s ",
			      (const char *)q->value(0).toString().left(29));
    
    //
    // Description
    //
    report+=QString().sprintf("%-29s ",
			      (const char *)q->value(1).toString().left(29));

    //
    // Service
    //
    report+=QString().sprintf("%-20s ",
			      (const char *)q->value(2).toString().left(20));

    //
    // Music Linked
    //
    if(q->value(3).toInt()>0) {
      report+=QString().sprintf(" %s  ",
				(const char *)q->value(4).toString());
    }
    else {
      report+="n/a ";
    }

    //
    // Traffic Linked
    //
    if(q->value(5).toInt()>0) {
      report+=QString().sprintf(" %s  ",
				(const char *)q->value(6).toString());
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
      report+=QString().sprintf("%s ",
		(const char *)q->value(9).toDate().toString("MM/dd/yyyy"));
    }
    
    //
    // End Date
    //
    if(q->value(10).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=QString().sprintf("%s ",
	(const char *)q->value(10).toDate().toString("MM/dd/yyyy"));
    }
    
    //
    // Last Modified Date
    //
    report+=QString().sprintf("%s",
	      (const char *)q->value(11).toDate().toString("MM/dd/yyyy"));

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
  RefreshList();
}


void MainWidget::filterClearedData()
{
  log_filter_edit->clear();
  filterChangedData("");
}


void MainWidget::logDoubleclickedData(QListViewItem *,const QPoint &,int)
{
  editData();
}


void MainWidget::quitMainWidget()
{
  log_db->removeDatabase(log_config->mysqlDbname());
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(log_log_list==NULL) {
    return;
  }
  log_service_label->setGeometry(10,10,70,20);
  log_service_box->setGeometry(85,10,140,20);
  log_filter_label->setGeometry(230,10,50,20);
  log_filter_edit->setGeometry(285,10,size().width()-360,20);
  log_filter_button->setGeometry(size().width()-60,8,50,25);
  log_log_list->setGeometry(10,37,size().width()-20,size().height()-107);
  log_add_button->setGeometry(10,size().height()-55,80,50);
  log_edit_button->setGeometry(100,size().height()-55,80,50);
  log_delete_button->setGeometry(190,size().height()-55,80,50);
  log_track_button->setGeometry(300,size().height()-55,80,50);
  log_report_button->setGeometry(400,size().height()-55,80,50);
  log_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
}


void MainWidget::RefreshItem(ListListViewItem *item)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("select DESCRIPTION,SERVICE,START_DATE,END_DATE,\
                         ORIGIN_USER,ORIGIN_DATETIME,COMPLETED_TRACKS,\
                         SCHEDULED_TRACKS,MUSIC_LINKS,MUSIC_LINKED,\
                         TRAFFIC_LINKS,TRAFFIC_LINKED,LINK_DATETIME,\
                         MODIFIED_DATETIME,AUTO_REFRESH from LOGS\
                         where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&(NAME=\"%s\")",
			(const char *)item->text(1));
  q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(2,q->value(0).toString());
    item->setText(3,q->value(1).toString());
    if((q->value(6).toInt()==q->value(7).toInt())&&
       ((q->value(8).toInt()==0)||(q->value(9).toString()=="Y"))&&
       ((q->value(10).toInt()==0)||(q->value(11).toString()=="Y"))) {
      item->setPixmap(0,*log_greencheckmark_map);
    }
    else {
      item->setPixmap(0,*log_redx_map);
    }
    if(q->value(8).toInt()==0) {
      item->setPixmap(4,*log_whiteball_map);
    }
    else {
      if(q->value(9).toString()=="Y") {
	item->setPixmap(4,*log_greenball_map);
      }
      else {
	item->setPixmap(4,*log_redball_map);
      }
    }
    if(q->value(10).toInt()==0) {
      item->setPixmap(5,*log_whiteball_map);
    }
    else {
      if(q->value(11).toString()=="Y") {
	item->setPixmap(5,*log_greenball_map);
      }
      else {
	item->setPixmap(5,*log_redball_map);
      }
    }
    item->setTracks(q->value(6).toInt());
    item->setTotalTracks(q->value(7).toInt());
    item->setTrackColumn(6);
    if(!q->value(2).toDate().isNull()) {
      item->setText(7,q->value(2).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(7,tr("Always"));
    }
    if(!q->value(3).toDate().isNull()) {
      item->setText(8,q->value(3).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(8,tr("TFN"));
    }
    item->setText(9,q->value(14).toString());
    item->setText(10,q->value(4).toString()+QString(" - ")+
      q->value(5).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
    item->
      setText(11,q->value(12).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
    item->
      setText(12,q->value(13).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
  }
  delete q;
}


void MainWidget::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  ListListViewItem *item;

  log_log_list->clear(); // Note: clear here, in case user has no perms.

  sql="select NAME from LOGS where (TYPE=0)&&(LOG_EXISTS=\"Y\")";

  if(log_service_box->currentItem()!=0) {
    sql+="&&(SERVICE=\""+RDEscapeString(log_service_box->currentText())+"\")";
  }
  QString filter=log_filter_edit->text();
  if(!filter.isEmpty()) {
    sql+="&&((NAME like \"%%"+RDEscapeString(filter)+"%%\")||";
    sql+="(DESCRIPTION like \"%%"+RDEscapeString(filter)+"%%\")||";
    sql+="(SERVICE like \"%%"+RDEscapeString(filter)+"%%\"))";
  }

  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec
      && rduser != NULL) {
    QStringList services_list;
    QString sql_where;

    services_list = rduser->services();
    if(services_list.size()==0) {
      return;
    }

    sql_where=" and (";
    for ( QStringList::Iterator it = services_list.begin(); 
          it != services_list.end(); ++it ) {
      sql_where+=QString().sprintf("SERVICE=\"%s\"||",
                             (const char *)*it);
    }
    sql_where=sql_where.left(sql_where.length()-2);
    sql_where+=")";

    sql=sql+sql_where;
  } // else no filter for RDStation::HostSec

  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new ListListViewItem(log_log_list);
    item->setText(1,q->value(0).toString());
    RefreshItem(item);
  }
  delete q;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString tr_path;
  QString qt_path;
#ifdef WIN32
  QSettings settings;
  settings.insertSearchPath(QSettings::Windows,"/SalemRadioLabs");
  tr_path=QString().sprintf("%s\\",
			    (const char *)settings.
			    readEntry("/Rivendell/InstallDir"));
  qt_path=tr_path;
#else
  tr_path=QString(PREFIX)+QString("/share/rivendell/");
  qt_path=QString(QTDIR)+QString("/translation/");
#endif  // WIN32
  QTranslator qt(0);
  qt.load(qt_path+QString("qt_")+QTextCodec::locale(),".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(tr_path+QString("librd_")+QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(tr_path+QString("librdhpi_")+QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(tr_path+QString("rdlogedit_")+QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(w->geometry().x(),w->geometry().y()),
		 w->sizeHint()));
  w->show();
  return a.exec();
}
