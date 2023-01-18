// rddbconfig.cpp
//
// A Qt-based application to configure, backup, and restore
// the Rivendell database.
//
//   (C) Copyright 2009-2023 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdescape_string.h>
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

  label_hostname=new QLabel(QString::asprintf("SQL Server: %s",
	 rd_config->mysqlHostname().toUtf8().constData()),this);
  label_hostname->setGeometry(0,5,sizeHint().width(),16);
  label_hostname->setFont(day_font);
  label_hostname->setAlignment(Qt::AlignCenter);

  label_username=new QLabel(QString::asprintf("SQL Usename: %s",
	 rd_config->mysqlUsername().toUtf8().constData()),this);
  label_username->setGeometry(0,20,sizeHint().width(),16);
  label_username->setFont(day_font);
  label_username->setAlignment(Qt::AlignCenter);

  label_dbname=new QLabel(QString::asprintf("SQL Database: %s",
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

  if (QMessageBox::question(this,tr("Database Mismatch"),QString::asprintf("Your database is version %d. Your Rivendell %s installation requires version %d. Would you like to modify your database to this version?",db->schema(),VERSION,RD_VERSION_DATABASE),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
    return;
  }

  int exit_code=-1;
  QString err_msg;
  QStringList args;
  bool result=false;
  args.push_back("--modify");
  if((result=RunProcess(&exit_code,&err_msg,QString("%1/sbin/rddbmgr").arg(RD_PREFIX),args))) {
    if(exit_code!=0) {
      QMessageBox::critical(this,tr("RDDbConfig Error"),err_msg);
    }
    else {
      if(!err_msg.isEmpty()) {
        QMessageBox::information(this,"RDDbConfig - "+tr("Warnings"),
				 QString::asprintf("Modified database to version %d with warnings:\n\n%s",
			    RD_VERSION_DATABASE,err_msg.toUtf8().constData()));
      }
      else {
        QMessageBox::information(this,"RDDbConfig - "+tr("Success"),
		     QString::asprintf("Modified database to version %d",
				       RD_VERSION_DATABASE));
      }
      RDApplication::syslog(rd_config,LOG_INFO,
			    "modified database to version %d",
			    RD_VERSION_DATABASE);

      emit dbChanged();
    }

  }
  else {
    QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),
			  QString("Error starting rddbmgr: ")+" "+err_msg);
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
    label_schema->setText(QString::asprintf("DB Version: %d",db->schema()));
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

  stopDaemons();

  CreateDb *db_create=new CreateDb(hostname,dbname,admin_name,admin_pwd);

  if(db_create->create(this,&err_str,rd_config)) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),err_str);
  }
  else {
    QMessageBox::information(this,tr("Success"),
			  tr("A new database has been successfully created."));
  }

  startDaemons();

  emit updateLabels();

  delete db_create;
}


void MainWidget::backupData()
{
  QString filename;

  if (!db->isOpen()) {
    QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),
			  QString::asprintf("Could not open %s database.",
			  rd_config->mysqlDbname().toUtf8().constData()));
    return;
  }
  filename=QFileDialog::getSaveFileName(this,"RDDbConfig - "+
					tr("Enter the MySQL Backup Filename"),
					RDHomeDir(),
					"MySQL files (*.sql);;All files (*.*)");
  if(!filename.isEmpty()) {
    int exit_code=-1;
    QString err_msg;
    QStringList args;

    args.push_back(QString::asprintf("--user=%s",
			  rd_config->mysqlUsername().toUtf8().constData()));
    args.push_back(QString::asprintf("--password=%s",
			  rd_config->mysqlPassword().toUtf8().constData()));
    args.push_back(QString::asprintf("--host=%s",
			  rd_config->mysqlHostname().toUtf8().constData()));
    args.push_back(rd_config->mysqlDbname());

    if(RunProcess(&exit_code,&err_msg,"mysqldump",args,filename)) {
      if(exit_code==0) {
	QMessageBox::information(this,"RDDbConfig - "+tr("Success"),
		     QString::asprintf("Backed up %s database to %s",
		     rd_config->mysqlDbname().toUtf8().constData(),
		     filename.toUtf8().constData()));
	RDApplication::syslog(rd_config,LOG_INFO,"backed up %s database to %s",
			      rd_config->mysqlDbname().toUtf8().constData(),
			      filename.toUtf8().constData());
      }
      else {
	QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),err_msg);
      }
    }
    else {
      QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),err_msg);
    }
  }
}


void MainWidget::restoreData()
{
  QString filename;
  int exit_code=-1;
  QString err_msg;

  if (!db->isOpen()) {
    QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),
      tr("Could not open Rivendell database."));
    return;
  }

  if(QMessageBox::question(this,"RDDbConfig - "+tr("Question"),
			   tr("This operation will completely overwrite the existing database, replacing it with the contents being restored.")+"\n\n"+
			   tr("Are you sure?"),
			   QMessageBox::Yes,
			   QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }

  filename=
    QFileDialog::getOpenFileName(this,"RDDbConfig - "+
				 tr("Choose the MySQL Backup File to Restore"),
				 RDHomeDir(),
				 "MySQL files (*.sql);;All files (*.*)");
  qApp->processEvents();
  if(!filename.isEmpty()) {
    db->clearDatabase(rd_config->mysqlDbname());
    QStringList args;
    args.push_back(QString::asprintf("--user=%s",rd_config->mysqlUsername().toUtf8().constData()));
    args.push_back(QString::asprintf("--password=%s",rd_config->mysqlPassword().toUtf8().constData()));
    args.push_back(QString::asprintf("--host=%s",rd_config->mysqlHostname().toUtf8().constData()));
    args.push_back(rd_config->mysqlDbname());
    stopDaemons();
    if(RunProcess(&exit_code,&err_msg,"mysql",args,"",filename)) {
      if(exit_code==0) {
	QMessageBox::information(this,"RDDbConfig - "+tr("Success"),
		     QString::asprintf("Restored %s database from %s",
		     rd_config->mysqlDbname().toUtf8().constData(),
		     filename.toUtf8().constData()));
      RDApplication::syslog(rd_config,LOG_INFO,"restored %s database from %s",
			    rd_config->mysqlDbname().toUtf8().constData(),
			    filename.toUtf8().constData());

      }
      else {
	QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),err_msg);
	return;
      }
    }
    else {
      QMessageBox::critical(this,"RDDbConfig - "+tr("Error"),err_msg);
      return;
    }
    emit updateLabels();

    //
    // Check for local host entry
    //
    int schema=0;
    QString err_msg;
    if(RDOpenDb(&schema,&err_msg,rd_config)&&(schema==RD_VERSION_DATABASE)) {
      QString sql=QString("select ")+
	"`NAME` "+  // 00
	"from `STATIONS` where "+
	"`NAME`='"+RDEscapeString(rd_config->stationName())+"'";
      RDSqlQuery *q=new RDSqlQuery(sql);
      if(!q->first()) {
	if(QMessageBox::question(this,"RDDbConfig - "+
				 tr("Missing Host Entry"),
				 tr("This database is missing a host entry for the local system")+" [\""+rd_config->stationName()+"\"].\n"+
				 tr("Create one?"),
				 QMessageBox::Yes,QMessageBox::No)==
	   QMessageBox::Yes) {
	  QStringList args;
	  args.push_back("--add-host-entry");
	  QProcess *proc=new QProcess(this);
	  proc->start("/usr/bin/rdadmin",args);
	  proc->waitForFinished(-1);
	}
      }
      delete q;
    }
    startDaemons();
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  db_create_button->setGeometry(size().width()/2-80,110,160,50);
  db_backup_button->setGeometry(size().width()/2-80,165,160,50);
  db_restore_button->setGeometry(size().width()/2-80,220,160,50);
  db_close_button->setGeometry(size().width()/2-80,275,160,50);
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


bool MainWidget::RunProcess(int *exit_code,QString *err_msg,const QString &cmd,
			    const QStringList &args,
			    const QString &output_filename,
			    const QString &input_filename)
{
  bool ret=false;
  *exit_code=-1;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QProcess *proc=new QProcess(this);
  if(!output_filename.isEmpty()) {
    proc->setStandardOutputFile(output_filename);
  }
  if(!input_filename.isEmpty()) {
    proc->setStandardInputFile(input_filename);
  }
  proc->start(cmd,args);
  proc->waitForFinished(-1);
  if(proc->exitStatus()==QProcess::NormalExit) {
    *exit_code=proc->exitCode();
    ret=proc->exitCode()==0;
    *err_msg=QString::fromUtf8(proc->readAllStandardError());
  }
  else {
    *exit_code=-1;
    *err_msg="\"+cmd\" "+tr("process crashed");
  }
  QApplication::restoreOverrideCursor();

  return ret;
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
