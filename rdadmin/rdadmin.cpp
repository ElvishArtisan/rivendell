// rdadmin.cpp
//
// The Administrator Utility for Rivendell.
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <QApplication>
#include <QCoreApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QTextCodec>
#include <QTranslator>
#include <QWindowsStyle>
#include <QWidget>

#include <rd.h>
#include <rdapplication.h>
#include <rdcheck_daemons.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rduser.h>

#include "edit_settings.h"
#include "globals.h"
#include "info_dialog.h"
#include "list_feeds.h"
#include "list_groups.h"
#include "list_replicators.h"
#include "list_reports.h"
#include "list_schedcodes.h"
#include "list_stations.h"
#include "list_svcs.h"
#include "list_users.h"
#include "login.h"
#include "opendb.h"
#include "rdadmin.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

//
// Global Classes
//
RDCartDialog *admin_cart_dialog;
bool exiting=false;
void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
      case SIGCHLD:
	pLocalPid=waitpid(-1,NULL,WNOHANG);
	while(pLocalPid>0) {
	  pLocalPid=waitpid(-1,NULL,WNOHANG);
	}
	signal(SIGCHLD,SigHandler);
	break;
  }
}


void PrintError(const QString &str,bool interactive)
{
  if(interactive) {
    QMessageBox::warning(NULL,QObject::tr("RDAdmin Error"),str);
  }
  else {
    fprintf(stderr,QString().sprintf("rdadmin: %s\n",(const char *)str));
  }
}


MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  new RDApplication(RDApplication::Gui,"rdadmin",RDADMIN_USAGE,true);

  QString str;

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
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);

  //
  // Window Titling
  //
  admin_rivendell_map=new QPixmap(rivendell_xpm);
  setWindowIcon(*admin_rivendell_map);
  str=QString(tr("RDAdmin")+" v"+VERSION+" - Host:");
  setWindowTitle(str+" "+rda->config()->stationName());

  //
  // Open Database
  //
  if(!OpenDb()) {
    exit(256);
  }

  //
  // Check (and possibly start) daemons
  //
  /*
  if(!RDStartDaemons()) {
    QMessageBox::warning(this,tr("Daemons Failed"),
			 tr("Unable to start Rivendell System Daemons!"));
    exit(1);
  }
  */
  //
  // Connect to ripcd(8).
  //
  rda->
    ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Log In
  //
  Login *login=new Login(&admin_username,&admin_password,NULL);
  if(login->exec()!=0) {
    exit(0);
  }
  rda->setUser(admin_username);
  if(!rda->user()->checkPassword(admin_password,false)) {
    QMessageBox::warning(this,"Login Failed","Login Failed!.\n");
    exiting=true;
  }
  else {
    if(!rda->user()->adminConfig()) {
      QMessageBox::warning(this,tr("Insufficient Priviledges"),
         tr("This account has insufficient priviledges for this operation."));
      exiting=true;
    }
  }

  //
  // Cart Dialog
  //
  admin_cart_dialog=
    new RDCartDialog(&admin_filter,&admin_group,&admin_schedcode,this);

  //
  // User Labels
  //
  QLabel *name_label=new QLabel(this);
  name_label->setGeometry(0,5,sizeHint().width(),20);
  name_label->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
  name_label->setFont(font);
  name_label->setText(QString().sprintf("USER: %s",(const char *)rda->user()->name()));

  QLabel *description_label=new QLabel(this);
  description_label->setGeometry(0,24,sizeHint().width(),14);
  description_label->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
  name_label->setFont(font);
  description_label->setText(rda->user()->description());

  //
  // Manage Users Button
  //
  QPushButton *users_button=new QPushButton(this);
  users_button->setGeometry(10,50,80,60);
  users_button->setFont(font);
  users_button->setText(tr("Manage\n&Users"));
  connect(users_button,SIGNAL(clicked()),this,SLOT(manageUsersData()));

  //
  // Manage Groups Button
  //
  QPushButton *groups_button=new QPushButton(this);
  groups_button->setGeometry(10,120,80,60);
  groups_button->setFont(font);
  groups_button->setText(tr("Manage\n&Groups"));
  connect(groups_button,SIGNAL(clicked()),this,SLOT(manageGroupsData()));
  
  //
  // Manage Services Button
  //
  QPushButton *services_button=new QPushButton(this);
  services_button->setGeometry(100,50,80,60);
  services_button->setFont(font);
  services_button->setText(tr("Manage\n&Services"));
  connect(services_button,SIGNAL(clicked()),this,SLOT(manageServicesData()));
  
  //
  // Manage Stations (Hosts) Button
  //
  QPushButton *stations_button=new QPushButton(this);
  stations_button->setGeometry(100,120,80,60);
  stations_button->setFont(font);
  stations_button->setText(tr("Manage\nHo&sts"));
  connect(stations_button,SIGNAL(clicked()),this,SLOT(manageStationsData()));
  
  //
  // Manage Reports
  //
  QPushButton *reports_button=new QPushButton(this);
  reports_button->setGeometry(190,50,80,60);
  reports_button->setFont(font);
  reports_button->setText(tr("Manage\nR&eports"));
  connect(reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Manage Podcasts
  //
  QPushButton *podcasts_button=new QPushButton(this);
  podcasts_button->setGeometry(280,50,80,60);
  podcasts_button->setFont(font);
  podcasts_button->setText(tr("Manage\n&Feeds"));
  connect(podcasts_button,SIGNAL(clicked()),this,SLOT(podcastsData()));

  //
  // System Wide Settings Button
  //
  QPushButton *system_button=new QPushButton(this);
  system_button->setGeometry(190,120,80,60);
  system_button->setFont(font);
  system_button->setText(tr("System\nSettings"));
  connect(system_button,SIGNAL(clicked()),this,SLOT(systemSettingsData()));

  //
  // Manage Scheduler Codes Button
  //
  QPushButton *schedcodes_button=new QPushButton(this);
  schedcodes_button->setGeometry(280,120,80,60);
  schedcodes_button->setFont(font);
  schedcodes_button->setText(tr("Scheduler\nCodes"));
  connect(schedcodes_button,SIGNAL(clicked()),this,SLOT(manageSchedCodes()));

  //
  // Manage Replicators Button
  //
  QPushButton *repl_button=new QPushButton(this);
  repl_button->setGeometry(10,190,80,60);
  repl_button->setFont(font);
  repl_button->setText(tr("Manage\nReplicators"));
  connect(repl_button,SIGNAL(clicked()),this,SLOT(manageReplicatorsData()));

  //
  // System Info Button
  //
  QPushButton *info_button=new QPushButton(this);
  info_button->setGeometry(100,190,80,60);
  info_button->setFont(font);
  info_button->setText(tr("System\nInfo"));
  connect(info_button,SIGNAL(clicked()),this,SLOT(systemInfoData()));

  //
  // Backup Database Button
  //
  QPushButton *backup_button=new QPushButton(this);
  backup_button->setGeometry(190,190,80,60);
  backup_button->setFont(font);
  backup_button->setText(tr("&Backup\nDatabase"));
  connect(backup_button,SIGNAL(clicked()),this,SLOT(backupData()));

  //
  // Restore Database Button
  //
  QPushButton *restore_button=new QPushButton(this);
  restore_button->setGeometry(280,190,80,60);
  restore_button->setFont(font);
  restore_button->setText(tr("&Restore\nDatabase"));
  connect(restore_button,SIGNAL(clicked()),this,SLOT(restoreData()));
  restore_button->setDisabled(true);
  
  //
  // Quit Button
  //
  QPushButton *quit_button=new QPushButton(this);
  quit_button->setGeometry(10,sizeHint().height()-70,sizeHint().width()-20,60);
  quit_button->setFont(font);
  quit_button->setText(tr("&Quit"));
  connect(quit_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  signal(SIGCHLD,SigHandler);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(370,330);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::manageUsersData()
{
  ListUsers *list_users=new ListUsers(rda->user()->name(),this);
  list_users->exec();
  delete list_users;
}


void MainWidget::manageGroupsData()
{
  ListGroups *list_groups=new ListGroups(this);
  list_groups->exec();
  delete list_groups;
}

void MainWidget::manageSchedCodes()
{
  ListSchedCodes *list_schedCodes=new ListSchedCodes(this);
  list_schedCodes->exec();
  delete list_schedCodes;
}

void MainWidget::manageServicesData()
{
  ListSvcs *list_svcs=new ListSvcs(this);
  list_svcs->exec();
  delete list_svcs;
}


void MainWidget::manageStationsData()
{
  ListStations *list_stations=new ListStations(this);
  list_stations->exec();
  delete list_stations;
}


void MainWidget::systemSettingsData()
{
  EditSettings *edit_settings=new EditSettings(this);
  edit_settings->exec();
  delete edit_settings;
}


void MainWidget::backupData()
{
  QString filename;
  QString cmd;
  int status;

  filename=QFileDialog::getSaveFileName(this,"RDAdmin - "+tr("Save Backup"),
					RDGetHomeDir(),
					tr("Rivendell Database Backup (*.sql)"));
  if(filename.isEmpty()) {
    return;
  }
  if(filename.right(4)!=QString(".sql")) {
    filename+=".sql";
  }
  cmd=QString().sprintf("mysqldump -c %s -h %s -u %s -p%s > %s",
			(const char *)rda->config()->mysqlDbname(),
			(const char *)rda->config()->mysqlHostname(),
			(const char *)rda->config()->mysqlUsername(),
			(const char *)rda->config()->mysqlPassword(),
			(const char *)filename);
  status=system((const char *)cmd);
  if(WEXITSTATUS(status)!=0) {
    unlink((const char *)filename);
    QMessageBox::warning(this,tr("Backup Error"),
			 tr("Unable to create backup!"));
    return;
  }
  QMessageBox::information(this,tr("Backup Complete"),
			   tr("Backup completed successfully."));
}


void MainWidget::restoreData()
{
  /*
  QString filename;
  QString cmd;
  int status;
  RDSqlQuery *q;

  if(QMessageBox::warning(NULL,tr("Restore Database"),
			  tr("WARNING: This operation will COMPLETELY\nOVERWRITE the existing Rivendell Database!\nDo you want to continue?"),
			  QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }      
  filename=QFileDialog::getOpenFileName(RDGetHomeDir(),
				    tr("Rivendell Database Backup (*.sql)"),
					this);
  if(filename.isEmpty()) {
    return;
  }
  RDKillDaemons();
  ClearTables();
  cmd=QString().sprintf("cat %s | mysql %s -h %s -u %s -p%s",
			(const char *)filename,
			(const char *)admin_config->mysqlDbname(),
			(const char *)admin_config->mysqlHostname(),
			(const char *)admin_config->mysqlUsername(),
			(const char *)admin_config->mysqlPassword());
  status=system((const char *)cmd);
  if(WEXITSTATUS(status)!=0) {
    QMessageBox::warning(this,tr("Restore Error"),
			 tr("Unable to restore backup!"));
    return;
  }
  q=new RDSqlQuery("select DB from VERSION");
  if(q->first()) {
    ver=q->value(0).toInt();
  }
  delete q;
  admin_skip_backup=true;
  //  UpdateDb(ver);
  QMessageBox::information(this,tr("Restore Complete"),
			   tr("Restore completed successfully."));
  RDStartDaemons();
  */
}


void MainWidget::manageReplicatorsData()
{
  ListReplicators *d=new ListReplicators(this);
  d->exec();
  delete d;
}


void MainWidget::systemInfoData()
{
  InfoDialog *info=new InfoDialog(this);
  info->exec();
  delete info;
}


void MainWidget::reportsData()
{
  ListReports *list_reports=new ListReports(this);
  list_reports->exec();
  delete list_reports;
}


void MainWidget::podcastsData()
{
  ListFeeds *list_feeds=new ListFeeds(this);
  list_feeds->exec();
  delete list_feeds;
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::ClearTables()
{
  RDSqlQuery *q1;

  QString sql="show tables";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("drop table ")+q->value(0).toString();
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}


int gui_main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  //
  // Load Translations
  //
  /*
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	  QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdadmin_")+
	  QTextCodec::locale(),".");
  a.installTranslator(&tr);
  */
  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  if(exiting) {
      exit(0);
  }
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}


int cmdline_main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new RDApplication(RDApplication::Console,"rdadmin",RDADMIN_USAGE,true);
  
  //
  // Open Database
  //
  OpenDb();
  return 0;
}


int main(int argc,char *argv[])
{
  int ret;
  bool found_check_db=false;

  RDCmdSwitch *cmd=new RDCmdSwitch(argc,argv,"rdadmin",RDADMIN_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--check-db") {
      found_check_db=true;
      cmd->setProcessed(i,true);
    }
  }
  if(found_check_db) {
    ret=cmdline_main(argc,argv);
  }
  else {
    ret=gui_main(argc,argv);
  }
  return ret;
}
