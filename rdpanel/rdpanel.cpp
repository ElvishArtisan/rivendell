// rdpanel.cpp
//
// A Dedicated Cart Wall Utility for Rivendell.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QTextCodec>
#include <QTranslator>
#include <QWindowsStyle>

#include <rdapplication.h>
#include <rdpanel.h>
#include <rd.h>
#include <rdcheck_daemons.h>
#include <rddbheartbeat.h>
#include <dbversion.h>

#include <globals.h>

//
// Global Resources
//
RDAudioPort *rdaudioport_conf;
RDCartDialog *panel_cart_dialog;

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

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
    return;
  }
}

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  new RDApplication(RDApplication::Gui,"rdpanel",RDPANEL_USAGE);

  QPixmap *pm;
  QPainter *pd;

  //
  // Fix the Window Size
  //
#ifndef RESIZABLE
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
#endif  // RESIZABLE

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",16,QFont::Bold);
  button_font.setPixelSize(16);

  //
  // Create Icons
  //
  lib_rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*lib_rivendell_map);

  //
  // Ensure that system daemons are running
  //
  RDInitializeDaemons();

  //
  // Master Clock Timer
  //
  panel_master_timer=new QTimer(this);
  connect(panel_master_timer,SIGNAL(timeout()),this,SLOT(masterTimerData()));
  panel_master_timer->start(MASTER_TIMER_INTERVAL);

  //
  // Allocate Global Resources
  //
  panel_skin_pixmap=new QPixmap(rda->panelConf()->skinPath());
  if(panel_skin_pixmap->isNull()||(panel_skin_pixmap->width()<1024)||
     (panel_skin_pixmap->height()<738)) {
    delete panel_skin_pixmap;
    panel_skin_pixmap=NULL;
  }
  else {
    setErasePixmap(*panel_skin_pixmap);
  }

  //
  // CAE Connection
  //
  panel_cae=new RDCae(rda->station(),rda->config(),parent);
  panel_cae->connectHost();

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));

  //
  // Meter Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(METER_INTERVAL);

  //
  // Macro Player
  //
  panel_player=new RDEventPlayer(rda->ripc(),this);

  //
  // Cart Picker
  //
  panel_cart_dialog=
    new RDCartDialog(&panel_filter,&panel_group,&panel_schedcode,this);

  //
  // Sound Panel Array
  //
  if (rda->panelConf()->panels(RDAirPlayConf::StationPanel) || 
      rda->panelConf()->panels(RDAirPlayConf::UserPanel)){
    int card=-1;
    panel_panel=
      new RDSoundPanel(RDPANEL_PANEL_BUTTON_COLUMNS,RDPANEL_PANEL_BUTTON_ROWS,
		       rda->panelConf()->panels(RDAirPlayConf::StationPanel),
		       rda->panelConf()->panels(RDAirPlayConf::UserPanel),
		       rda->panelConf()->flashPanel(),
		       rda->panelConf()->buttonLabelTemplate(),true,panel_player,
		       rda->ripc(),panel_cae,rda->station(),panel_cart_dialog,this);
    panel_panel->setLogfile(rda->config()->airplayLogname());
    panel_panel->setGeometry(10,10,panel_panel->sizeHint().width(),
			 panel_panel->sizeHint().height());
    if(panel_skin_pixmap!=NULL) {
      pm=new QPixmap(1024,738);
      pd=new QPainter(pm);
      pd->drawPixmap(-10,-10,*panel_skin_pixmap);
      pd->end();
      panel_panel->setErasePixmap(*pm);
      delete pd;
      delete pm;
    }
    panel_panel->setPauseEnabled(rda->panelConf()->panelPauseEnabled());
    panel_panel->setCard(0,rda->panelConf()->card(RDAirPlayConf::SoundPanel1Channel));
    panel_panel->setPort(0,rda->panelConf()->port(RDAirPlayConf::SoundPanel1Channel));
    panel_panel->setFocusPolicy(Qt::NoFocus);
    if((card=rda->panelConf()->card(RDAirPlayConf::SoundPanel2Channel))<0) {
      panel_panel->setCard(1,panel_panel->card(RDAirPlayConf::MainLog1Channel));
      panel_panel->setPort(1,panel_panel->port(RDAirPlayConf::MainLog1Channel));
    }
    else {
      panel_panel->setCard(1,card);
      panel_panel->setPort(1,rda->panelConf()->port(RDAirPlayConf::SoundPanel2Channel));
    }
    if((card=rda->panelConf()->card(RDAirPlayConf::SoundPanel3Channel))<0) {
      panel_panel->setCard(2,panel_panel->card(RDAirPlayConf::MainLog2Channel));
      panel_panel->setPort(2,panel_panel->port(RDAirPlayConf::MainLog2Channel));
    }
    else {
      panel_panel->setCard(2,card);
      panel_panel->setPort(2,rda->panelConf()->port(RDAirPlayConf::SoundPanel3Channel));
    }
    if((card=rda->panelConf()->card(RDAirPlayConf::SoundPanel4Channel))<0) {
      panel_panel->setCard(3,panel_panel->card(RDAirPlayConf::SoundPanel1Channel));
      panel_panel->setPort(3,panel_panel->port(RDAirPlayConf::SoundPanel1Channel));
    }
    else {
      panel_panel->setCard(3,card);
      panel_panel->setPort(3,rda->panelConf()->port(RDAirPlayConf::SoundPanel4Channel));
    }
    if((card=rda->panelConf()->card(RDAirPlayConf::SoundPanel5Channel))<0) {
      panel_panel->setCard(4,panel_panel->card(RDAirPlayConf::CueChannel));
      panel_panel->setPort(4,panel_panel->port(RDAirPlayConf::CueChannel));
    }
    else {
      panel_panel->setCard(4,card);
      panel_panel->setPort(4,rda->panelConf()->port(RDAirPlayConf::SoundPanel5Channel));
    }

    //
    // Calculate Valid Ports for Reading Meter Data (No Duplicates)
    //
    for(int i=4;i>=0;i--) {
      meter_data_valid[i]=(panel_panel->card(i)>=0);
      for(int j=0;j<i;j++) {
	if((panel_panel->card(i)==panel_panel->card(j))&&
	   (panel_panel->port(i)==panel_panel->port(j))) {
	  meter_data_valid[i]=false;
	}
      }
    }

    //
    // Set Fader Display Numbers
    //
    int next_output=1;
    for(int i=0;i<PANEL_MAX_OUTPUTS;i++) {
      bool unique=true;
      int output=next_output;
      for(int j=0;j<i;j++) {
	if((panel_panel->card(i)==panel_panel->card(j))&&
	   (panel_panel->port(i)==panel_panel->port(j))) {
	  unique=false;
	  output=panel_panel->outputText(j).toInt();
	}
      }
      if(unique) {
	next_output++;
      }
      panel_panel->setOutputText(i,QString().sprintf("%d",output));
    }

    //
    // Set RML Strings
    //
    panel_panel->
      setRmls(0,rda->panelConf()->startRml(RDAirPlayConf::SoundPanel1Channel),
	      rda->panelConf()->stopRml(RDAirPlayConf::SoundPanel1Channel));
    panel_panel->
      setRmls(1,rda->panelConf()->startRml(RDAirPlayConf::SoundPanel2Channel),
	      rda->panelConf()->stopRml(RDAirPlayConf::SoundPanel2Channel));
    panel_panel->
      setRmls(2,rda->panelConf()->startRml(RDAirPlayConf::SoundPanel3Channel),
	      rda->panelConf()->stopRml(RDAirPlayConf::SoundPanel3Channel));
    panel_panel->
      setRmls(3,rda->panelConf()->startRml(RDAirPlayConf::SoundPanel4Channel),
	      rda->panelConf()->stopRml(RDAirPlayConf::SoundPanel4Channel));
    panel_panel->
      setRmls(4,rda->panelConf()->startRml(RDAirPlayConf::SoundPanel5Channel),
	      rda->panelConf()->stopRml(RDAirPlayConf::SoundPanel5Channel));
    panel_panel->setSvcName(rda->panelConf()->defaultSvc());
    connect(rda->ripc(),SIGNAL(userChanged()),panel_panel,SLOT(changeUser()));
    connect(panel_master_timer,SIGNAL(timeout()),
	    panel_panel,SLOT(tickClock()));
  }

  //
  // Audio Meter
  //
  panel_stereo_meter=new RDStereoMeter(this);
  panel_stereo_meter->
    setGeometry(20,
		sizeHint().height()-panel_stereo_meter->sizeHint().height()-7,
		panel_stereo_meter->sizeHint().width(),
		panel_stereo_meter->sizeHint().height());
  panel_stereo_meter->setMode(RDSegMeter::Peak);
  panel_stereo_meter->setFocusPolicy(Qt::NoFocus);
  if(rda->config()->useStreamMeters()) {
    panel_stereo_meter->hide();
  }

  //
  // Empty Cart
  //
  panel_empty_cart=new RDEmptyCart(this);
  panel_empty_cart->setGeometry(373,sizeHint().height()-52,32,32);
  if(!rda->station()->enableDragdrop()) {
    panel_empty_cart->hide();
  }

  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Signal Handlers
  //
  signal(SIGCHLD,SigHandler);
}


QSize MainWidget::sizeHint() const
{
  return QSize(935,738);
}


void MainWidget::rmlReceivedData(RDMacro *rml)
{
  RunLocalMacros(rml);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::userData()
{
  rda->setUser(rda->ripc()->user());
  SetCaption();
  rda->ripc()->sendOnairFlag();
}


void MainWidget::meterData()
{
#ifdef SHOW_METER_SLOTS
  printf("meterData()\n");
#endif
  double ratio[2]={0.0,0.0};
  short level[2];

  for(int i=0;i<PANEL_MAX_OUTPUTS;i++) {
    if(meter_data_valid[i]) {
      panel_cae->
	outputMeterUpdate(panel_panel->card(i),panel_panel->port(i),level);
      for(int j=0;j<2;j++) {
	ratio[j]+=pow(10.0,((double)level[j])/1000.0);
      }
    }
  }
  panel_stereo_meter->setLeftPeakBar((int)(log10(ratio[0])*1000.0));
  panel_stereo_meter->setRightPeakBar((int)(log10(ratio[1])*1000.0));
}


void MainWidget::masterTimerData()
{
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


void MainWidget::RunLocalMacros(RDMacro *rml)
{
}


void MainWidget::SetCaption()
{
  setCaption(QString("RDPanel")+" v"+VERSION+" - "+tr("Station")+": "+
	     rda->config()->stationName()+", "+tr("User")+": "+
	     rda->ripc()->user());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  /*
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),".");
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
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdpanel_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);
  */
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
