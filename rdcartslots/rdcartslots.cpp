// rdcartslots.cpp
//
// A Dedicated Cart Slot Utility for Rivendell.
//
//   (C) Copyright 2012-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtranslator.h>

#include <rdescape_string.h>

#include "rdcartslots.h"

//
// Icons
//
#include "../icons/rdcartslots-22x22.xpm"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("RDCartSlots","rdcartslots",RDCARTSLOTS_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDCartSlots - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDCartSlots - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Create Icons
  //
  lib_rivendell_map=new QPixmap(rdcartslots_22x22_xpm);
  setWindowIcon(*lib_rivendell_map);

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
	  this,SLOT(caeConnectedData(bool)));
  rda->cae()->connectHost();

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));

  //
  // Service Picker
  //
  panel_svcs_dialog=new RDListSvcs(tr("RDCartSlots"),this);

  //
  // Macro Player
  //
  panel_player=new RDEventPlayer(rda->ripc(),this);

  //
  // Dialogs
  //
  panel_cart_dialog=new RDCartDialog(&panel_filter,&panel_group,
				     &panel_schedcode,"RDCartSlots",this);
  panel_slot_dialog=new RDSlotDialog(tr("RDCartSlots"),this);
  panel_cue_dialog=new RDCueEditDialog(rda->cae(),rda->station()->cueCard(),
				       rda->station()->cuePort(),
				       tr("RDCartSlots"),this);

  //
  // Cart Slots
  //
  QTimer *timer=new QTimer(this);
  for(int i=0;i<rda->station()->cartSlotColumns();i++) {
    for(int j=0;j<rda->station()->cartSlotRows();j++) {
      panel_slots.
	push_back(new RDCartSlot(panel_slots.size(),rda->ripc(),rda->cae(),
				 rda->station(),rda->config(),panel_svcs_dialog,
				 panel_slot_dialog,panel_cart_dialog,
				 panel_cue_dialog,tr("RDCartSlots"),
				 rda->airplayConf(),this));
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
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

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
  return QSize(10+rda->station()->cartSlotColumns()*
	       (10+panel_slots[0]->size().width()),
	       10+rda->station()->cartSlotRows()*
	       (5+panel_slots[0]->size().height()));
}


void MainWidget::caeConnectedData(bool state)
{
  QList<int> cards;

  QString sql=QString("select CARD from CARTSLOTS where ")+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
   cards.push_back(q->value(0).toInt());
  }
  delete q;

  rda->cae()->enableMetering(&cards);
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
  for(unsigned i=0;i<panel_slots.size();i++) {
    panel_slots[i]->setUser(rda->user());
  }
  SetCaption();
  rda->ripc()->sendOnairFlag();
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  for(int i=1;i<rda->station()->cartSlotColumns();i++) {
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
  setWindowTitle(tr("RDCartSlots")+" v"+VERSION+" - "+tr("Station")+": "+
		 rda->config()->stationName()+"  "+tr("User")+": "+
		 rda->ripc()->user());
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString("/usr/share/qt4/translations/qt_")+QTextCodec::locale(),".");
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

  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
