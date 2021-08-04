// rddbconfig.cpp
//
// A Qt-based application to configure, backup, and restore
// the Rivendell database.
//
//   (C) Copyright 2009-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <QApplication>
#include <QFileDialog>
#include <QProcess>

#include <dbversion.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rdpaths.h>

#include "../../icons/rivendell-22x22.xpm"

#include "rddbconfig.h"
#include "db.h"
#include "createdb.h"
#include "mysql_login.h"

//
// Globals
//
MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString err_msg;

  db_daemon_start_needed=false;
  db=NULL;

  if(geteuid()!=0) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),
      tr("This application requires root permissions."));
      exit(256);
  }

  //
  // Open rd.conf(5)
  //
  rd_config=new RDConfig();
  rd_config->load();
  rd_config->setModuleName("rddbconfig");


  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));
  setWindowTitle(tr("RDDbConfig")+" v"+VERSION);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Labels
  //
  QLabel *label=new QLabel(tr("Select an operation:"),this);
  label->setGeometry(0,90,sizeHint().width(),16);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignCenter);

  label_hostname=new QLabel(QString().sprintf("SQL Server: %s",
	 rd_config->mysqlHostname().toUtf8().constData()),this);
  label_hostname->setGeometry(0,5,sizeHint().width(),16);
  label_hostname->setFont(day_font);
  label_hostname->setAlignment(Qt::AlignCenter);

  label_username=new QLabel(QString().sprintf("SQL Usename: %s",
	 rd_config->mysqlUsername().toUtf8().constData()),this);
  label_username->setGeometry(0,20,sizeHint().width(),16);
  label_username->setFont(day_font);
  label_username->setAlignment(Qt::AlignCenter);

  label_dbname=new QLabel(QString().sprintf("SQL Database: %s",
	 rd_config->mysqlDbname().toUtf8().constData()),this);
  label_dbname->setGeometry(0,35,sizeHint().width(),16);
  label_dbname->setFont(day_font);
  label_dbname->setAlignment(Qt::AlignCenter);

  label_schema=new QLabel(this);
  label_schema->setGeometry(0,50,sizeHint().width(),16);
  label_schema->setFont(day_font);
  label_schema->setAlignment(Qt::AlignCenter);

  //
  // Create Button
  //
  db_create_button=new QPushButton(tr("Create"),this);
  db_create_button->setFont(buttonFont());
  connect(db_create_button,SIGNAL(clicked()),this,SLOT(createData()));

  //
  // Backup Button
  //
  db_backup_button=new QPushButton(tr("Backup"),this);
  db_backup_button->setFont(buttonFont());
  connect(db_backup_button,SIGNAL(clicked()),this,SLOT(backupData()));

  //
  // Restore Button
  //
  db_restore_button=new QPushButton(tr("Restore"),this);
  db_restore_button->setFont(buttonFont());
  connect(db_restore_button,SIGNAL(clicked()),this,SLOT(restoreData()));

  //
  // Close Button
  //
  db_close_button=new QPushButton(tr("Close"),this);
  db_close_button->setFont(buttonFont());
  connect(db_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Signals
  //
  connect(this,SIGNAL(dbChanged()),this,SLOT(updateLabels()));
  connect(this,SIGNAL(dbMismatch()),this,SLOT(mismatchData()));

  this->show();

  emit dbChanged();
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(280,330);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void MainWidget::mismatchData()
{
  if (!db->schema()) {
    return;
  }

  if (QMessageBox::question(this,tr("Database Mismatch"),QString().sprintf("Your database is version %d. Your Rivendell %s installation requires version %d. Would you like to modify your database to the current version?",db->schema(),VERSION,RD_VERSION_DATABASE),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
    return;
  }

  QProcess modifyProcess(this);
  QStringList args;
  args << QString("--modify");
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  modifyProcess.start(QString("%1/sbin/rddbmgr").arg(RD_PREFIX), args);
  bool r=modifyProcess.waitForFinished(-1);
  QApplication::restoreOverrideCursor();
  if(r) {
    QString stderr=modifyProcess.readAllStandardError();
    if (modifyProcess.exitCode()) {
      QMessageBox::critical(this,tr("RDDbConfig Error"),stderr);
    }
    else {
      if(!stderr.isEmpty()) {
        QMessageBox::information(this,"Database Modified with Warnings",
          QString().sprintf("Modified database to version %d with warnings:\n\n%s",
			    RD_VERSION_DATABASE,stderr.toUtf8().constData()));
      }
      else {
        QMessageBox::information(this,"Database Modified Successfully",
          QString().sprintf("Modified database to version %d",
			    RD_VERSION_DATABASE));
      }
      RDApplication::syslog(rd_config,LOG_INFO,
			    "modified database to version %d",
			    RD_VERSION_DATABASE);

      emit dbChanged();
    }
  }
  else {
    QMessageBox::critical(this,tr("Database Update Error"),
      QString("Error starting rddbmgr: code=%1").arg(modifyProcess.error()));
  }
}

void MainWidget::updateLabels()
{
  QString err_msg="";

  if(db!=NULL) {
    delete db;
  }
  db = new Db(&err_msg,rd_config);

  if (!db->isOpen()) {
    label_schema->setText("DB Version: unknown");
    db_backup_button->setEnabled(false);
    db_restore_button->setEnabled(false);
  }
  else {
    label_schema->setText(QString().sprintf("DB Version: %d",db->schema()));
    db_backup_button->setEnabled(true);
    db_restore_button->setEnabled(true);

    if(db->schema()!=RD_VERSION_DATABASE) {
      emit dbMismatch();
    }
  }
}

void MainWidget::closeData()
{
  exit(0);
}

void MainWidget::createData()
{
  MySqlLogin *mysql_login;
  QString hostname=rd_config->mysqlHostname();
  QString dbname=rd_config->mysqlDbname();
  QString admin_name;
  QString admin_pwd;
  QString err_str;

  if (db->isOpen()) {
    if (QMessageBox::question(this,tr("Create Database"),tr("Creating a new database will erase all of your data. Are you sure you want to create a new database?"),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
      return;
    }
  }

  mysql_login=new MySqlLogin(&admin_name,&admin_pwd,rd_config,this);

  if(mysql_login->exec()) {
    delete mysql_login;
    return;
  }

  delete mysql_login;

  if(admin_name.isEmpty()||admin_pwd.isEmpty()) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),tr("Did not specify username and/or password."));
    return;
  }

  stopDaemons();

  CreateDb *db_create=new CreateDb(hostname,dbname,admin_name,admin_pwd);

  if(db_create->create(this,&err_str,rd_config)) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),err_str);
  }
  else {
    QMessageBox::information(this,tr("Success"),tr("A new database has been successfully created."));
  }

  startDaemons();

  emit updateLabels();

  delete db_create;
}


void MainWidget::backupData()
{
  QString filename;

  if (!db->isOpen()) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),
      QString().sprintf("Could not open %s database.",
			rd_config->mysqlDbname().toUtf8().constData()));
    return;
  }
  filename=QFileDialog::getSaveFileName(this,"RDDbConfig - "+
					tr("Enter the MySQL Backup Filename"),
					RDHomeDir(),
					"MySQL files (*.sql);;All files (*.*)");

  if (!filename.isEmpty()) {
    QProcess backupProcess(this);
    QStringList args;
    args << QString().sprintf("--user=%s",rd_config->mysqlUsername().toUtf8().constData())
	 << QString().sprintf("--password=%s",rd_config->mysqlPassword().toUtf8().constData())
	 << QString().sprintf("--host=%s",rd_config->mysqlHostname().toUtf8().constData())
      << rd_config->mysqlDbname();
    backupProcess.setStandardOutputFile(filename);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    backupProcess.start("mysqldump", args);
    backupProcess.waitForFinished(-1);
    QApplication::restoreOverrideCursor();
    if (backupProcess.exitCode()) {
      QMessageBox::critical(this,tr("RDDbConfig Error"),
        QString(backupProcess.readAllStandardError()));
    }
    else {
      QMessageBox::information(this,"Database Backed Up Successfully",
        QString().sprintf("Backed up %s database to %s",
			  rd_config->mysqlDbname().toUtf8().constData(),
			  filename.toUtf8().constData()));
      RDApplication::syslog(rd_config,LOG_INFO,"backed up %s database to %s",
			    rd_config->mysqlDbname().toUtf8().constData(),
			    filename.toUtf8().constData());
    }
  }
}


void MainWidget::restoreData()
{
  QString filename;

  if (!db->isOpen()) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),
      tr("Could not open Rivendell database."));
    return;
  }
  filename=
    QFileDialog::getOpenFileName(this,"RDDbConfig - "+
				 tr("Choose the MySQL Backup File to Restore"),
				 RDHomeDir(),
				 "MySQL files (*.sql);;All files (*.*)");

  if(!filename.isEmpty()) {
    if (QMessageBox::question(this,tr("Restore Entire Database"),tr("Are you sure you want to restore your entire Rivendell database?"),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
      return;
    }

    db->clearDatabase(rd_config->mysqlDbname());

    QProcess restoreProcess(this);
    QStringList args;
    args << QString().sprintf("--user=%s",rd_config->mysqlUsername().toUtf8().constData())
	 << QString().sprintf("--password=%s",rd_config->mysqlPassword().toUtf8().constData())
	 << QString().sprintf("--host=%s",rd_config->mysqlHostname().toUtf8().constData())
      << rd_config->mysqlDbname();
    restoreProcess.setStandardInputFile(filename);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    stopDaemons();
    restoreProcess.start("mysql", args);
    restoreProcess.waitForFinished(-1);
    QApplication::restoreOverrideCursor();
    if (restoreProcess.exitCode()) {
      QMessageBox::critical(this,tr("RDDbConfig Error"),
      QString(restoreProcess.readAllStandardError()));
    }
    else {
      QMessageBox::information(this,"Database Restored Successfully",
        QString().sprintf("Restored %s database from %s",
			  rd_config->mysqlDbname().toUtf8().constData(),
			  filename.toUtf8().constData()));
      RDApplication::syslog(rd_config,LOG_INFO,"restored %s database from %s",
			    rd_config->mysqlDbname().toUtf8().constData(),
			    filename.toUtf8().constData());
    }
    emit updateLabels();
    startDaemons();
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  db_create_button->
    setGeometry(size().width()/2-80,110,160,50);
  db_backup_button->
    setGeometry(size().width()/2-80,165,160,50);
  db_restore_button->
    setGeometry(size().width()/2-80,220,160,50);
  db_close_button->
    setGeometry(size().width()/2-80,275,160,50);
}


int MainWidget::statusDaemons(QString service)
{
  QProcess statusProcess(this);
  QStringList args;
  args << "status" << service;
  statusProcess.start("systemctl", args);
  statusProcess.waitForFinished();
  return statusProcess.exitCode();
}

void MainWidget::stopDaemons()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if(!statusDaemons("rivendell")) {
    QProcess stopProcess(this);
    QStringList args;
    args << "stop" << "rivendell";
    stopProcess.start("systemctl", args);
    stopProcess.waitForFinished();
    if (!stopProcess.exitCode()) {
      db_daemon_start_needed=true;
    }
  }
  QApplication::restoreOverrideCursor();
}


void MainWidget::startDaemons()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if(statusDaemons("rivendell")) {
    QProcess startProcess(this);
    QStringList args;
    args << "start" << "rivendell";
    startProcess.start("systemctl", args);
    startProcess.waitForFinished();
  }
  QApplication::restoreOverrideCursor();
}


int main(int argc,char *argv[])
{
  //
  // Start GUI
  //
  RDConfig *config=new RDConfig();
  config->load();
  QApplication a(argc,argv);
  new MainWidget(config);
  return a.exec();
}
