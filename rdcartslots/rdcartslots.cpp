// rdcartslots.cpp
//
// A Dedicated Cart Slot Utility for Rivendell.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcartslots.cpp,v 1.8.2.13 2014/02/11 23:46:29 cvs Exp $
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

#include <qmessagebox.h>
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <rdcartslots.h>
#include <rd.h>
#include <rdcheck_daemons.h>
#include <rddbheartbeat.h>
#include <rdcmd_switch.h>
#include <dbversion.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Force a reasonable default font.
  //
  QFont mfont("helvetica",12,QFont::Normal);
  mfont.setPixelSize(12);
  qApp->setFont(mfont);

  //
  // Load Local Configs
  //
  panel_config=new RDConfig();
  panel_config->load();

  //
  // Load the command-line arguments
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcartslots",
				   RDCARTSLOTS_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }

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
  // Open Database
  //
  QString err;
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    QMessageBox::warning(this,tr("Can't Connect"),err);
    exit(0);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdcastmanager: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }
  new RDDbHeartbeat(panel_config->mysqlHeartbeatInterval(),this);

  //
  // Allocate Global Resources
  //
  panel_station=new RDStation(panel_config->stationName());
  panel_system=new RDSystem();

  //
  // RDAirPlay Configuration
  //
  panel_airplay_conf=new RDAirPlayConf(panel_config->stationName(),"RDAIRPLAY");

  //
  // CAE Connection
  //
  panel_cae=new RDCae(panel_station,panel_config,parent);
  panel_cae->connectHost();

  //
  // RIPC Connection
  //
  panel_ripc=new RDRipc(panel_config->stationName());
  connect(panel_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(panel_ripc,SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));

  //
  // User
  //
  panel_user=NULL;

  //
  // Service Picker
  //
  panel_svcs_dialog=new RDListSvcs(tr("RDCartSlots"),this);

  //
  // Macro Player
  //
  panel_player=new RDEventPlayer(panel_ripc,this);

  //
  // Dialogs
  //
  panel_cart_dialog=new RDCartDialog(&panel_filter,&panel_group,
				     &panel_schedcode,panel_cae,panel_ripc,
				     panel_station,panel_system,panel_config,
				     this);
  panel_slot_dialog=new RDSlotDialog(tr("RDCartSlots"),this);
  panel_cue_dialog=new RDCueEditDialog(panel_cae,panel_station->cueCard(),
				       panel_station->cuePort(),
				       tr("RDCartSlots"),this);

  //
  // Cart Slots
  //
  QTimer *timer=new QTimer(this);
  for(int i=0;i<panel_station->cartSlotColumns();i++) {
    for(int j=0;j<panel_station->cartSlotRows();j++) {
      panel_slots.
	push_back(new RDCartSlot(panel_slots.size(),panel_ripc,panel_cae,
				 panel_station,panel_config,panel_svcs_dialog,
				 panel_slot_dialog,panel_cart_dialog,
				 panel_cue_dialog,tr("RDCartSlots"),
				 panel_airplay_conf,this));
      panel_slots.back()->
	setGeometry(10+i*(panel_slots.back()->sizeHint().width()+10),
		    10+j*(panel_slots.back()->sizeHint().height()+5),
		  panel_slots.back()->sizeHint().width(),
		  panel_slots.back()->sizeHint().height());
      connect(timer,SIGNAL(timeout()),
	      panel_slots.back(),SLOT(updateMeters()));
    }
  }
  timer->start(METER_INTERVAL);
  panel_ripc->connectHost("localhost",RIPCD_TCP_PORT,panel_config->password());

  //
  // Fix the Window Size
  //
#ifndef RESIZABLE
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
#endif  // RESIZABLE
}


QSize MainWidget::sizeHint() const
{
  return QSize(10+panel_station->cartSlotColumns()*
	       (10+panel_slots[0]->size().width()),
	       10+panel_station->cartSlotRows()*
	       (5+panel_slots[0]->size().height()));
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
  if(panel_user!=NULL) {
    delete panel_user;
  }
  panel_user=new RDUser(panel_ripc->user());
  for(unsigned i=0;i<panel_slots.size();i++) {
    panel_slots[i]->setUser(panel_user);
  }
  SetCaption();
  panel_ripc->sendOnairFlag();
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  for(int i=1;i<panel_station->cartSlotColumns();i++) {
    p->fillRect(i*(panel_slots[0]->size().width()+10),10,
		5,size().height()-15,Qt::black);
  }
  delete p;
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  for(unsigned i=0;i<panel_slots.size();i++) {
    delete panel_slots[i];  // So temporary carts get cleaned up properly
  }
  exit(0);
}


void MainWidget::SetCaption()
{
  QString service=tr("[None]");
  setCaption(tr("RDCartSlots")+" v"+VERSION+" - "+tr("Station")+": "+
	     panel_config->stationName()+"  "+tr("User")+": "+
	     panel_ripc->user());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
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
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdcartslots_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
