// rdadmin.cpp
//
// The Administrator Utility for Rivendell.
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

#include <rd.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rddb.h>
#include <rddbheartbeat.h>
#include <rdescape_string.h>

#include "add_station.h"
#include "edit_system.h"
#include "globals.h"
#include "info_dialog.h"
#include "list_feeds.h"
#include "list_groups.h"
#include "list_replicators.h"
#include "list_reports.h"
#include "list_schedcodes.h"
#include "list_svcs.h"
#include "list_stations.h"
#include "list_users.h"
#include "login.h"
#include "rdadmin.h"

//
// Global Classes
//
RDCartDialog *admin_cart_dialog;
bool exiting=false;

void PrintError(const QString &str,bool interactive)
{
  if(interactive) {
    QMessageBox::warning(NULL,QObject::tr("RDAdmin Error"),str);
  }
  else {
    fprintf(stderr,"rdadmin: %s\n",str.toUtf8().constData());
  }
}


MainWidget::MainWidget(RDConfig *config,RDWidget *parent)
  : RDMainWindow("rdadmin",config)
{
  QString str;
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;
  admin_add_host_entry=false;
  
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Open the Database
  //
  rda=new RDApplication("RDAdmin","rdadmin",RDADMIN_USAGE,true,this);
  if(!rda->open(&err_msg,&err_type,false,false)) {
    if(err_type!=RDApplication::ErrorNoHostEntry) {
      QMessageBox::critical(this,"RDAdmin - "+tr("Error"),err_msg);
      exit(RDCoreApplication::ExitNoDb);
    }
  }

  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--add-host-entry") {
      if(geteuid()==0) {
	admin_add_host_entry=true;
	rda->cmdSwitch()->setProcessed(i,true);
      }
      else {
	QMessageBox::critical(this,"RDAdmin - "+tr("Error"),
			      tr("The --add-host-entry option requires root permissions."));
	exit(RDCoreApplication::ExitNoPerms);
      }
    }
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDAdmin - "+tr("Error"),
			    tr("Unrecognized option")+": "+
			    rda->cmdSwitch()->key(i)+" "+
			    rda->cmdSwitch()->value(i));
      exit(RDCoreApplication::ExitInvalidOption);
    }
  }

  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::RdAdmin,22));
  setWindowTitle(QString("RDAdmin v")+VERSION+" - "+
		 tr("Host")+": "+rda->config()->stationName());

  rda->
    ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Cart Dialog
  //
  admin_cart_dialog=new RDCartDialog(&admin_filter,&admin_group,
				     &admin_schedcode,"RDAdmin",true,this);

  //
  // User Labels
  //
  QLabel *name_label=new QLabel(this);
  name_label->setGeometry(0,5,sizeHint().width(),20);
  name_label->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
  name_label->setFont(labelFont());

  QLabel *description_label=new QLabel(this);
  description_label->setGeometry(0,24,sizeHint().width(),14);
  description_label->setAlignment(Qt::AlignVCenter|Qt::AlignCenter);
  name_label->setFont(labelFont());

  //
  // Manage Users Button
  //
  QPushButton *users_button=new QPushButton(this);
  users_button->setGeometry(10,50,80,60);
  users_button->setFont(buttonFont());
  users_button->setText(tr("Manage\nUsers"));
  connect(users_button,SIGNAL(clicked()),this,SLOT(manageUsersData()));

  //
  // Manage Groups Button
  //
  QPushButton *groups_button=new QPushButton(this);
  groups_button->setGeometry(10,120,80,60);
  groups_button->setFont(buttonFont());
  groups_button->setText(tr("Manage\nGroups"));
  connect(groups_button,SIGNAL(clicked()),this,SLOT(manageGroupsData()));
  
  //
  // Manage Services Button
  //
  QPushButton *services_button=new QPushButton(this);
  services_button->setGeometry(100,50,80,60);
  services_button->setFont(buttonFont());
  services_button->setText(tr("Manage\nServices"));
  connect(services_button,SIGNAL(clicked()),this,SLOT(manageServicesData()));
  
  //
  // Manage Stations (Hosts) Button
  //
  QPushButton *stations_button=new QPushButton(this);
  stations_button->setGeometry(100,120,80,60);
  stations_button->setFont(buttonFont());
  stations_button->setText(tr("Manage\nHosts"));
  connect(stations_button,SIGNAL(clicked()),this,SLOT(manageStationsData()));
  
  //
  // Manage Reports
  //
  QPushButton *reports_button=new QPushButton(this);
  reports_button->setGeometry(190,50,80,60);
  reports_button->setFont(buttonFont());
  reports_button->setText(tr("Manage\nReports"));
  connect(reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Manage Podcasts
  //
  QPushButton *podcasts_button=new QPushButton(this);
  podcasts_button->setGeometry(280,50,80,60);
  podcasts_button->setFont(buttonFont());
  podcasts_button->setText(tr("Manage\nFeeds"));
  connect(podcasts_button,SIGNAL(clicked()),this,SLOT(podcastsData()));

  //
  // System Wide Settings Button
  //
  QPushButton *system_button=new QPushButton(this);
  system_button->setGeometry(190,120,80,60);
  system_button->setFont(buttonFont());
  system_button->setText(tr("System\nSettings"));
  connect(system_button,SIGNAL(clicked()),this,SLOT(systemSettingsData()));

  //
  // Manage Scheduler Codes Button
  //
  QPushButton *schedcodes_button=new QPushButton(this);
  schedcodes_button->setGeometry(280,120,80,60);
  schedcodes_button->setFont(buttonFont());
  schedcodes_button->setText(tr("Scheduler\nCodes"));
  connect(schedcodes_button,SIGNAL(clicked()),this,SLOT(manageSchedCodes()));

  //
  // Manage Replicators Button
  //
  QPushButton *repl_button=new QPushButton(this);
  repl_button->setGeometry(100,190,80,60);
  repl_button->setFont(buttonFont());
  repl_button->setText(tr("Manage\nReplicators"));
  connect(repl_button,SIGNAL(clicked()),this,SLOT(manageReplicatorsData()));

  //
  // System Info Button
  //
  QPushButton *info_button=new QPushButton(this);
  info_button->setGeometry(190,190,80,60);
  info_button->setFont(buttonFont());
  info_button->setText(tr("System\nInfo"));
  connect(info_button,SIGNAL(clicked()),this,SLOT(systemInfoData()));

  //
  // Quit Button
  //
  QPushButton *quit_button=new QPushButton(this);
  quit_button->setGeometry(10,sizeHint().height()-70,sizeHint().width()-20,60);
  quit_button->setFont(buttonFont());
  quit_button->setText(tr("Quit"));
  connect(quit_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  loadSettings(true);

  //
  // Log In
  //
  if(admin_add_host_entry) {
    QString hostname=rda->config()->stationName();

    AddStation *d=new AddStation(&hostname,this);
    if(d->exec()) {
      exit(RDCoreApplication::ExitOk);
    }
    else {
      RDStation::remove(hostname);
      exit(RDCoreApplication::ExitNoStation);
    }
    delete d;
    exit(RDCoreApplication::ExitOk);
  }
  Login *login=new Login(&admin_username,&admin_password,this);
  if(!login->exec()) {
    exit(RDCoreApplication::ExitOk);
  }
  rda->user()->setName(admin_username);
  bool config_priv=rda->user()->adminConfig();
  bool rss_priv=rda->user()->adminRss();
  if(!rda->user()->checkPassword(admin_password,false)) {
    QMessageBox::warning(this,"Login Failed","Login Failed!.\n");
    exit(RDCoreApplication::ExitNoPerms);
  }
  else {
    if((!config_priv)&&(!rss_priv)) {
      QMessageBox::warning(this,tr("Insufficient Privileges"),
         tr("This account has insufficient privileges for this operation."));
      exit(RDCoreApplication::ExitNoPerms);
    }
  }
  name_label->setText(tr("User")+": "+rda->user()->name());
  description_label->setText(rda->user()->description());
  users_button->setEnabled(config_priv);
  groups_button->setEnabled(config_priv);
  services_button->setEnabled(config_priv);
  stations_button->setEnabled(config_priv);
  reports_button->setEnabled(config_priv);
  podcasts_button->setEnabled(config_priv||rss_priv);
  system_button->setEnabled(config_priv);
  schedcodes_button->setEnabled(config_priv);
  repl_button->setEnabled(config_priv);
  info_button->setEnabled(config_priv||rss_priv);
}


QSize MainWidget::sizeHint() const
{
  return QSize(370,330);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  quitMainWidget();
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
  EditSystem *edit_system=new EditSystem(this);
  edit_system->exec();
  delete edit_system;
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
  saveSettings();
  exit(RDCoreApplication::ExitOk);
}


void MainWidget::ClearTables()
{
  QString sql="show tables";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    rda->dropTable(q->value(0).toString());
  }
  delete q;
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
