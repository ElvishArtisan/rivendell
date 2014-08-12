// rdalsaconfig.cpp
//
// A Qt-based application to display info about ALSA cards.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdalsaconfig.cpp,v 1.2.8.3 2014/01/21 21:59:33 cvs Exp $
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

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmessagebox.h>

#include <rdcmd_switch.h>

#include <alsaitem.h>
#include <rdalsaconfig.h>

//
// Globals
//
QString alsa_filename;
bool alsa_autogen=false;
bool alsa_manage_daemons=false;
bool alsa_daemon_start_needed=false;

void StopDaemons()
{
  if(alsa_manage_daemons) {
    if(system("/etc/init.d/rivendell status")==0) {
      system("/etc/init.d/rivendell stop");
      alsa_daemon_start_needed=true;
    }
  }
}


void StartDaemons()
{
  if(alsa_daemon_start_needed) {
    system("/etc/init.d/rivendell start");
  }
}


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  setCaption(tr("RDAlsaConfig")+" v"+VERSION);

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

  //
  // Available Devices
  //
  alsa_system_list=new QListBox(this);
  alsa_system_list->setFont(font);
  alsa_system_label=
    new QLabel(alsa_system_list,tr("Available Sound Devices"),this);
  alsa_system_label->setFont(label_font);
  alsa_system_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Up Button
  //
  alsa_up_button=
    new RDTransportButton(RDTransportButton::Up,this,"alsa_up_button");
  connect(alsa_up_button,SIGNAL(clicked()),this,SLOT(upData()));

  //
  // Down Button
  //
  alsa_down_button=
    new RDTransportButton(RDTransportButton::Down,this,"alsa_down_button");
  connect(alsa_down_button,SIGNAL(clicked()),this,SLOT(downData()));

  //
  // Selected Devices
  //
  alsa_config_list=new QListBox(this);
  alsa_config_list->setFont(font);
  alsa_config_label=new QLabel(alsa_config_list,tr("Active Sound Devices"),this);
  alsa_config_label->setFont(label_font);
  alsa_config_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Save Button
  //
  alsa_save_button=new QPushButton(tr("Save"),this);
  alsa_save_button->setFont(label_font);
  connect(alsa_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  // Cancel Button
  //
  alsa_cancel_button=new QPushButton(tr("Cancel"),this);
  alsa_cancel_button->setFont(label_font);
  connect(alsa_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Available Devices and Configuration
  //
  alsa_alsa=new RDAlsa();
  alsa_alsa->load(alsa_filename);
  LoadList(alsa_system_list,alsa_config_list);

  //
  // Daemon Management
  //
  if(alsa_manage_daemons) {
    if(geteuid()!=0) {
      QMessageBox::warning(this,tr("RDAlsaConfig error"),
	     tr("The \"--manage-daemons\" switch requires root permissions."));
      exit(256);
    }
    if(system("/etc/init.d/rivendell status")==0) {
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


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::upData()
{
  MoveItem(alsa_config_list,alsa_system_list);
}


void MainWidget::downData()
{
  if(alsa_config_list->count()>=RD_MAX_CARDS) {
    return;
  }
  MoveItem(alsa_system_list,alsa_config_list);
}


void MainWidget::saveData()
{
  AlsaItem *item=NULL;

  for(int i=0;i<RD_MAX_CARDS;i++) {
    if((item=(AlsaItem *)alsa_config_list->item(i))==NULL) {
      alsa_alsa->setRivendellCard(i,-1);
      alsa_alsa->setRivendellDevice(i,-1);
    }
    else {
      alsa_alsa->setRivendellCard(i,item->card());
      alsa_alsa->setRivendellDevice(i,item->device());
    }
  }
  if(!alsa_alsa->save(alsa_filename)) {
    QMessageBox::warning(this,tr("RDAlsaConfig error"),
			 tr(QString("Unable to save configuration to \"")+
			    alsa_filename+"\""));
    return;
  }
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
  alsa_system_label->setGeometry(20,5,size().width()-20,20);
  alsa_system_list->
    setGeometry(10,25,size().width()-20,(size().height()-120)/2);
  alsa_up_button->setGeometry(size().width()-120,size().height()/2-28,50,30);
  alsa_down_button->setGeometry(size().width()-60,size().height()/2-28,50,30);
  alsa_config_label->setGeometry(20,size().height()/2-10,size().width()/2,20);
  alsa_config_list->setGeometry(10,size().height()/2+10,
			      size().width()-20,(size().height()-120)/2);
  alsa_save_button->
    setGeometry(size().width()-120,size().height()-40,50,30);
  alsa_cancel_button->
    setGeometry(size().width()-60,size().height()-40,50,30);
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


void MainWidget::LoadList(QListBox *system,QListBox *config)
{
  for(unsigned i=0;i<alsa_alsa->cards();i++) {
    for(int j=0;j<alsa_alsa->pcmDevices(i);j++) {
      if(PcmUnused(i,j)) {
	AlsaItem *item=
	  new AlsaItem(alsa_alsa->cardLongName(i)+" - "+
		       alsa_alsa->pcmName(i,j));
	item->setCard(i);
	item->setDevice(j);
	system->insertItem(item);
      }
    }
  }
  system->sort();

  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(alsa_alsa->rivendellCard(i)>=0) {
      AlsaItem *item=
	new AlsaItem(alsa_alsa->cardLongName(alsa_alsa->rivendellCard(i))+" - "+
		     alsa_alsa->pcmName(alsa_alsa->rivendellCard(i),
					alsa_alsa->rivendellDevice(i)));
      item->setCard(alsa_alsa->rivendellCard(i));
      item->setDevice(alsa_alsa->rivendellDevice(i));
      config->insertItem(item);
    }
  }
  config->sort();
}


bool MainWidget::PcmUnused(int card,int device)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if((card==alsa_alsa->rivendellCard(i))&&
       (device==alsa_alsa->rivendellDevice(i))) {
      return false;
    }
  }
  return true;
}


void MainWidget::MoveItem(QListBox *src,QListBox *dest)
{
  AlsaItem *item=(AlsaItem *)src->selectedItem();
  if(item==NULL) {
    return;
  }
  dest->insertItem(new AlsaItem(*item));  // Force a deep copy
  dest->sort();
  delete item;
}


Autogen::Autogen(QObject *parent,const char *name)
{
  StopDaemons();

  //
  // Load Available Devices
  //
  RDAlsa *alsa=new RDAlsa();
  alsa->load(alsa_filename);

  //
  // Build Configuration
  //
  int slot=0;
  for(unsigned i=0;i<alsa->cards();i++) {
    for(int j=0;j<alsa->pcmDevices(i);j++) {
      alsa->setRivendellCard(slot,i);
      alsa->setRivendellDevice(slot,j);
      slot++;
    }
  }

  //
  // Save Configuration
  //
  if(!alsa->save(alsa_filename)) {
    exit(256);
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
    if(cmd->key(i)=="--manage-daemons") {
      alsa_manage_daemons=true;
    }
  }

  //
  // Autogenerate a full configuration
  //
  if(alsa_autogen) {
    QApplication a(argc,argv,false);
    new Autogen(NULL,"main");
    return a.exec();
  }

  //
  // Start GUI
  //
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
