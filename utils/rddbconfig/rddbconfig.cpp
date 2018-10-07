// rddbconfig.cpp
//
// A Qt-based application to configure, backup, and restore
// the Rivendell database.
//
//   (C) Copyright 2009-2018 Fred Gleason <fredg@paravelsystems.com>
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

#define LINE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <q3filedialog.h>

#include <rdconfig.h>

#include "../../icons/rivendell-22x22.xpm"

#include <rddbconfig.h>

#include "db.h"
#include "createdb.h"
#include "mysql_login.h"

//
// Globals
//
MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;

  db_daemon_start_needed=false;
  db=NULL;

  //
  // Open rd.conf(5)
  //
  rd_config=new RDConfig();
  rd_config->load();
  rd_config->setModuleName("rddbconfig");

  setWindowTitle(tr("RDDbConfig")+" v"+VERSION);

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont font("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont day_font=QFont("Helvetica",12,QFont::Normal);
  day_font.setPixelSize(12);

  //
  // Labels
  //
  QLabel *label=new QLabel(tr("Select an operation:"),this);
  label->setGeometry(0,90,sizeHint().width(),16);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignCenter);

  label_hostname=new QLabel(QString().sprintf("SQL Server: %s",
    (const char *)rd_config->mysqlHostname()),this);
  label_hostname->setGeometry(0,5,sizeHint().width(),16);
  label_hostname->setFont(day_font);
  label_hostname->setAlignment(Qt::AlignCenter);

  label_username=new QLabel(QString().sprintf("SQL Usename: %s",
    (const char *)rd_config->mysqlUsername()),this);
  label_username->setGeometry(0,20,sizeHint().width(),16);
  label_username->setFont(day_font);
  label_username->setAlignment(Qt::AlignCenter);

  label_dbname=new QLabel(QString().sprintf("SQL Database: %s",
    (const char *)rd_config->mysqlDbname()),this);
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
  db_create_button->setFont(label_font);
  connect(db_create_button,SIGNAL(clicked()),this,SLOT(createData()));

  //
  // Backup Button
  //
  db_backup_button=new QPushButton(tr("Backup"),this);
  db_backup_button->setFont(label_font);
  connect(db_backup_button,SIGNAL(clicked()),this,SLOT(backupData()));

  //
  // Restore Button
  //
  db_restore_button=new QPushButton(tr("Restore"),this);
  db_restore_button->setFont(label_font);
  connect(db_restore_button,SIGNAL(clicked()),this,SLOT(restoreData()));

  //
  // Close Button
  //
  db_close_button=new QPushButton(tr("Close"),this);
  db_close_button->setFont(label_font);
  connect(db_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  updateLabels();
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

void MainWidget::updateLabels()
{
  QString err_msg="";

  if(db!=NULL) {
    delete db;
  }
  db = new Db(&err_msg,rd_config);

  if (!db->isOpen()) {
    QMessageBox::critical(this,tr("Cannot Open Database"),tr("Unable to open database. The connection parameters of the [Mysql] section of rd.conf may be incorrect or you may need to create a new database."));
    label_schema->setVisible(false);
    db_backup_button->setEnabled(false);
    db_restore_button->setEnabled(false);
  }
  else {
    label_schema->setText(QString().sprintf("DB Version: %d",db->schema()));
    label_schema->setVisible(true);
    db_backup_button->setEnabled(true);
    db_restore_button->setEnabled(true);
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
  QString msg="Message";
  QString err_str;

  if(geteuid()!=0) {
    QMessageBox::warning(this,tr("RDDbConfig error"),
      tr("Create requires root permissions."));
      return;
  }

  if (db->isOpen()) {
    if (QMessageBox::question(this,tr("Create Database"),tr("Creating a new database will erase all of your data. Are you sure you want to create a new database?"),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
      return;
    }
  }

  mysql_login=new MySqlLogin(msg,&hostname,&dbname,&admin_name,&admin_pwd);

  mysql_login->exec();

  delete mysql_login;

  if(admin_name.isEmpty()||admin_pwd.isEmpty()||hostname.isEmpty()||dbname.isEmpty()) {
    return;
  }

  stopDaemons();

  CreateDb *db_create=new CreateDb(hostname,dbname,admin_name,admin_pwd);

  if(db_create->create(this,&err_str,rd_config)) {
    fprintf(stderr,"err_str: %s\n",(const char *)err_str);
  }

  delete db_create;

  startDaemons();
}


void MainWidget::backupData()
{
  QString filename;

  if (!db->isOpen()) {
    QMessageBox::critical(this,tr("RDDbConfig Error"),
      QString().sprintf("Could not open %s database.",
      (const char *)rd_config->mysqlDbname()));
    return;
  }

  filename=Q3FileDialog::getSaveFileName("rivendell.sql",
    "MySQL (*.sql)",this,"open file dialog","Enter the MySQL Backup Filename");

  if (!filename.isEmpty()) {
    QProcess backupProcess(this);
    QStringList args;
    args << QString().sprintf("--user=%s",(const char *)rd_config->mysqlUsername())
      << QString().sprintf("--password=%s",(const char *)rd_config->mysqlPassword())
      << QString().sprintf("--host=%s",(const char *)rd_config->mysqlHostname())
      << rd_config->mysqlDbname();
    backupProcess.setStandardOutputFile(filename);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    backupProcess.start("mysqldump", args);
    backupProcess.waitForFinished();
    QApplication::restoreOverrideCursor();
    if (backupProcess.exitCode()) {
      fprintf(stderr,"Exit Code=%d\n",backupProcess.exitCode());
      QMessageBox::critical(this,tr("RDDbConfig error"),
        QString(backupProcess.readAllStandardError()));
    }
    else {
      QMessageBox::information(this,"Database Backed Up Successfully",
        QString().sprintf("Backed up %s database to %s",
          (const char *)rd_config->mysqlDbname(),
          (const char *)filename));
      rd_config->log("rddbconfig",RDConfig::LogInfo,
        QString().sprintf("Backed up %s database to %s",
          (const char *)rd_config->mysqlDbname(),
          (const char *)filename));
    }
  }
}


void MainWidget::restoreData()
{
  QString filename;

  if(geteuid()!=0) {
    QMessageBox::warning(this,tr("RDDbConfig error"),
      tr("Restore requires root permissions."));
    return;
  }

  if (!db->isOpen()) {
    QMessageBox::critical(this,tr("RDDbConfig error"),
      tr("Could not open Rivendell database."));
    return;
  }

  filename=Q3FileDialog::getOpenFileName("",
    "MySQL (*.sql);;All Files(*)",this,"open file dialog",
    "Choose the MySQL Backup File to Restore");

  if(!filename.isEmpty()) {
    if (QMessageBox::question(this,tr("Restore Entire Database"),tr("Are you sure you want to restore your entire Rivendell database?"),(QMessageBox::No|QMessageBox::Yes)) != QMessageBox::Yes) {
      return;
    }
    QProcess restoreProcess(this);
    QStringList args;
    args << QString().sprintf("--user=%s",(const char *)rd_config->mysqlUsername())
      << QString().sprintf("--password=%s",(const char *)rd_config->mysqlPassword())
      << QString().sprintf("--host=%s",(const char *)rd_config->mysqlHostname())
      << rd_config->mysqlDbname();
    restoreProcess.setStandardInputFile(filename);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    stopDaemons();
    restoreProcess.start("mysql", args);
    restoreProcess.waitForFinished();
    QApplication::restoreOverrideCursor();
    if (restoreProcess.exitCode()) {
      QMessageBox::critical(this,tr("RDDbConfig error"),
      QString(restoreProcess.readAllStandardError()));
    }
    else {
      QMessageBox::information(this,"Database Restored Successfully",
        QString().sprintf("Restored %s database from %s",
          (const char *)rd_config->mysqlDbname(),
          (const char *)filename));
      rd_config->log("rddbconfig",RDConfig::LogInfo,
        QString().sprintf("Restored %s database from %s",
          (const char *)rd_config->mysqlDbname(),
          (const char *)filename));
    }
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


void MainWidget::stopDaemons()
{
  if(system("/usr/bin/systemctl status rivendell")==0) {
    system("/usr/bin/systemctl stop rivendell");
    db_daemon_start_needed=true;
  }
}


void MainWidget::startDaemons()
{
  if(db_daemon_start_needed) {
    system("/usr/bin/systemctl start rivendell");
  }
}


int main(int argc,char *argv[])
{
  //
  // Start GUI
  //
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->move(250,250);
  w->show();
  return a.exec();
}
