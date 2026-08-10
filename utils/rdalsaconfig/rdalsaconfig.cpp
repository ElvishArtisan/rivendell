// rdalsaconfig.cpp
//
// A Qt-based application to display info about ALSA cards.
//
//   (C) Copyright 2009-2026 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>

#include "alsaitem.h"
#include "rdalsaconfig.h"

//
// Icons
//
#include "../../icons/rivendell-22x22.xpm"

//
// Globals
//
QString alsa_filename;
bool alsa_autogen=false;
bool alsa_rewrite=false;
bool alsa_manage_daemons=false;
bool alsa_daemon_start_needed=false;

void StopDaemons()
{
  if(alsa_manage_daemons) {
    if(system("systemctl --quiet is-active rivendell")==0) {
      RDCheckExitCode("StopDaemons() system",
		      system("systemctl --quiet stop rivendell"));
      alsa_daemon_start_needed=true;
    }
  }
}


void StartDaemons()
{
  if(alsa_daemon_start_needed) {
    RDCheckExitCode("StartDaemons() system",
		    system("systemctl --quiet start rivendell"));
  }
}


MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString err_msg;

  setWindowTitle(tr("RDAlsaConfig")+" v"+VERSION);

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Open the Database
  //
  rda=new RDApplication("RDAlsaConfig","rdalsaconfig",RDALSACONFIG_USAGE,false,
			this);
  if(!rda->open(&err_msg,NULL,false,true)) {
    QMessageBox::critical(this,"RDAlsaConfig - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // ALSA Sound Devices
  //
  alsa_system_table=new RDTableView(this);
  alsa_system_table->setSelectionMode(QAbstractItemView::MultiSelection);
  alsa_system_label=new QLabel(tr("ALSA Sound Devices"),this);
  alsa_system_label->setFont(bigLabelFont());
  alsa_system_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  alsa_description_label=new QLabel(this);
  alsa_description_label->
    setText(tr("Select the ALSA audio device(s) to dedicate for use with Rivendell. (Devices so dedicated will be unavailable for use with other applications.)"));
  alsa_description_label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
  alsa_description_label->setWordWrap(true);

  //
  // Save Button
  //
  alsa_save_button=new QPushButton(tr("Save"),this);
  alsa_save_button->setFont(buttonFont());
  connect(alsa_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  // Cancel Button
  //
  alsa_cancel_button=new QPushButton(tr("Cancel"),this);
  alsa_cancel_button->setFont(buttonFont());
  connect(alsa_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Available Devices and Configuration
  //
  alsa_system_model=new RDAlsaModel(rda->system()->sampleRate(),this);
  alsa_system_table->setModel(alsa_system_model);
  alsa_system_table->setCurrentIndex(QModelIndex());
  alsa_system_table->resizeColumnsToContents();
  connect(alsa_system_table->selectionModel(),
	SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
	this,
	SLOT(selectionChangedData(const QItemSelection &,const QItemSelection &)));
  LoadSelections(alsa_filename);

  //
  // Daemon Management
  //
  if(alsa_manage_daemons) {
    if(geteuid()!=0) {
      QMessageBox::warning(this,tr("RDAlsaConfig error"),
	     tr("The \"--manage-daemons\" switch requires root permissions."));
      exit(256);
    }
    if(system("systemctl --quiet is-active rivendell")==0) {
      int r=QMessageBox::warning(this,tr("RDAlsaConfig warning"),
	    tr("Rivendell audio will be interrupted while running this program.\nContinue?"),
				     QMessageBox::Yes,QMessageBox::No);
      if(r!=QMessageBox::Yes) {
	exit(256);
      }       
    }
  }
  StopDaemons();
}


QSize MainWidget::sizeHint() const
{
  return QSize(600,400);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::selectionChangedData(const QItemSelection &,
				      const QItemSelection &)
{
  alsa_system_table->setCurrentIndex(QModelIndex());
}


void MainWidget::saveData()
{
  SaveSelections(alsa_filename);

  StartDaemons();

  qApp->quit();
}


void MainWidget::cancelData()
{
  StartDaemons();
  qApp->quit();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  alsa_system_label->setGeometry(10,5,size().width()-20,20);
  alsa_description_label->setGeometry(10,25,size().width()-20,50);
  alsa_system_table->
    setGeometry(10,75-10,size().width()-20,size().height()-130+10);
  alsa_save_button->
    setGeometry(size().width()-140,size().height()-40,60,30);
  alsa_cancel_button->
    setGeometry(size().width()-70,size().height()-40,60,30);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  int r=QMessageBox::question(this,tr("RDAlsaConfig quit"),
			      tr("Save configuration before exiting?"),
			      QMessageBox::Yes,QMessageBox::No,
			      QMessageBox::Cancel);
  switch(r) {
    case QMessageBox::Yes:
      saveData();
      break;

    case QMessageBox::No:
      cancelData();
      break;

    default:
      break;
  }
}


void MainWidget::LoadSelections(const QString &filename)
{
  if(!alsa_system_model->loadSelections(filename)) {
    return;
  }
  for(int i=0;i<alsa_system_model->rowCount();i++) {
    if(alsa_system_model->isEnabled(i)) {
      alsa_system_table->selectionModel()->
	select(QItemSelection(alsa_system_model->index(i,0),
			      alsa_system_model->index(i,1)),
	       QItemSelectionModel::Select);
    }
    else {
      alsa_system_table->selectionModel()->
	select(QItemSelection(alsa_system_model->index(i,0),
			      alsa_system_model->index(i,1)),
	       QItemSelectionModel::Deselect);
    }
  }
}


void MainWidget::SaveSelections(const QString &filename) const
{
  for(int i=0;i<alsa_system_model->rowCount();i++) {
    QItemSelectionModel *sel=alsa_system_table->selectionModel();
    alsa_system_model->setEnabled(i,sel->isRowSelected(i,QModelIndex()));
  }
  alsa_system_model->saveSelections(filename);
}


Autogen::Autogen()
  : QObject()
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("RDAlsaConfig","rdalsaconfig",RDALSACONFIG_USAGE,
			false,this);
  if(!rda->open(&err_msg,NULL,false,true)) {
    fprintf(stderr,"rdalsaconfig: unable to open database [%s]\n",
	    (const char *)err_msg.toUtf8());
    exit(1);
  }

  StopDaemons();

  RDAlsaModel *model=new RDAlsaModel(rda->system()->sampleRate());
  if(alsa_rewrite) {
    if(!model->loadSelections(alsa_filename)) {
      fprintf(stderr,"rdalsaconfig: unable to load file \"%s\"\n",
	      (const char *)alsa_filename.toUtf8());
      StartDaemons();
      exit(1);
    }
  }
  if(alsa_autogen) {
    for(int i=0;i<model->rowCount();i++) {
      model->setEnabled(i,true);
    }
  }
  if(!model->saveSelections(alsa_filename)) {
    fprintf(stderr,"rdalsaconfig: unable to load file \"%s\"\n",
	    (const char *)alsa_filename.toUtf8());
    StartDaemons();
    exit(1);
  }

  StartDaemons();

  exit(0);
}


int main(int argc,char *argv[])
{
  //
  // Load the command-line arguments
  //
  alsa_filename=RD_ASOUNDRC_FILE;
  RDCmdSwitch *cmd=new RDCmdSwitch(argc,argv,"rdalsaconfig",
				   RDALSACONFIG_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--asoundrc-file") {
      alsa_filename=cmd->value(i);
    }
    if(cmd->key(i)=="--autogen") {
      alsa_autogen=true;
    }
    if(cmd->key(i)=="--rewrite") {
      alsa_rewrite=true;
    }
    if(cmd->key(i)=="--manage-daemons") {
      alsa_manage_daemons=true;
    }
  }

  if(alsa_autogen||alsa_rewrite) {
    QCoreApplication a(argc,argv);
    new Autogen();
    return a.exec();
  }

  //
  // Start GUI
  //
  QApplication a(argc,argv);
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
