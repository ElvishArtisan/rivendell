// livewire_lwrpaudio.cpp
//
// A Rivendell LWRP audio switcher driver for LiveWire networks.
//
//   (C) Copyright 2002-2013,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdescape_string.h>

#include "globals.h"
#include "livewire_lwrpaudio.h"

LiveWireLwrpAudio::LiveWireLwrpAudio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Get Matrix Parameters
  //
  livewire_stationname=rda->station()->name();
  livewire_matrix=matrix->matrix();

  //
  // Load The Node List
  //
  sql=QString("select ")+
    "HOSTNAME,"+
    "TCP_PORT,"+
    "PASSWORD,"+
    "BASE_OUTPUT "+
    "from SWITCHER_NODES where "+
    "(STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\")&&"+
    QString().sprintf("(MATRIX=%d)",livewire_matrix);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    livewire_nodes.push_back(new RDLiveWire(livewire_nodes.size(),this));
    connect(livewire_nodes.back(),SIGNAL(connected(unsigned)),
	    this,SLOT(nodeConnectedData(unsigned)));
    connect(livewire_nodes.back(),
	    SIGNAL(sourceChanged(unsigned,RDLiveWireSource *)),
	    this,
	    SLOT(sourceChangedData(unsigned,RDLiveWireSource *)));
    connect(livewire_nodes.back(),
	    SIGNAL(destinationChanged(unsigned,RDLiveWireDestination *)),
	    this,
	    SLOT(destinationChangedData(unsigned,RDLiveWireDestination *)));
    /*
    connect(livewire_nodes.back(),
	    SIGNAL(gpoConfigChanged(unsigned,unsigned,unsigned)),
	    this,
	    SLOT(gpoConfigChangedData(unsigned,unsigned,unsigned)));
    connect(livewire_nodes.back(),
	    SIGNAL(gpiChanged(unsigned,unsigned,unsigned,bool)),
	    this,
	    SLOT(gpiChangedData(unsigned,unsigned,unsigned,bool)));
    connect(livewire_nodes.back(),
	    SIGNAL(gpoChanged(unsigned,unsigned,unsigned,bool)),
	    this,
	    SLOT(gpoChangedData(unsigned,unsigned,unsigned,bool)));
    */
    connect(livewire_nodes.back(),
	    SIGNAL(watchdogStateChanged(unsigned,const QString &)),
	    this,SLOT(watchdogStateChangedData(unsigned,const QString &)));
    livewire_nodes.back()->connectToHost(q->value(0).toString(),
					 q->value(1).toInt(),
					 q->value(2).toString(),
					 q->value(3).toUInt());
  }
  delete q;
}


LiveWireLwrpAudio::~LiveWireLwrpAudio()
{
  for(unsigned i=0;i<livewire_nodes.size();i++) {
    delete livewire_nodes[i];
  }
  livewire_nodes.clear();
}


RDMatrix::Type LiveWireLwrpAudio::type()
{
  return RDMatrix::LiveWireLwrpAudio;
}


unsigned LiveWireLwrpAudio::gpiQuantity()
{
  return 0;
}


unsigned LiveWireLwrpAudio::gpoQuantity()
{
  return 0;
}


bool LiveWireLwrpAudio::primaryTtyActive()
{
  return false;
}


bool LiveWireLwrpAudio::secondaryTtyActive()
{
  return false;
}


void LiveWireLwrpAudio::processCommand(RDMacro *cmd)
{
  QString label;
  unsigned input;
  unsigned output;
  RDLiveWire *node=NULL;
  RDLiveWire *dest_node=NULL;

  switch(cmd->command()) {
      case RDMacro::ST:
	input=cmd->arg(1).toUInt();
	if(input>RD_LIVEWIRE_MAX_SOURCE) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	output=cmd->arg(2).toUInt();
	if(output>RD_LIVEWIRE_MAX_SOURCE) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}

	//
	// Find the destination node
	//
	for(unsigned i=0;i<livewire_nodes.size();i++) {
	  node=livewire_nodes[i];
	  if((output>=node->baseOutput())&&
	     (output<(node->baseOutput()+node->destinations()))) {
	    dest_node=node;
	  }
	}
	if(dest_node==NULL) {  // No such output!
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	dest_node->setRoute(input,output-dest_node->baseOutput());
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void LiveWireLwrpAudio::nodeConnectedData(unsigned id)
{
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("connection established to LiveWire node at \"%s\"",
		  (const char *)livewire_nodes[id]->hostname()));
}


void LiveWireLwrpAudio::sourceChangedData(unsigned id,RDLiveWireSource *src)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from INPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",livewire_matrix)+
    "(NODE_HOSTNAME=\""+RDEscapeString(livewire_nodes[id]->hostname())+"\")&&"+
    QString().sprintf("(NODE_TCP_PORT=%d)&&",livewire_nodes[id]->tcpPort())+
    QString().sprintf("(NODE_SLOT=%d)",src->slotNumber());
  q=new RDSqlQuery(sql);
  delete q;

  if(src->rtpEnabled()) {
    sql=QString("insert into INPUTS set ")+
      "STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\","+
      QString().sprintf("MATRIX=%d,",livewire_matrix)+
      "NODE_HOSTNAME=\""+RDEscapeString(livewire_nodes[id]->hostname())+"\","+
      QString().sprintf("NODE_TCP_PORT=%d,",livewire_nodes[id]->tcpPort())+
      QString().sprintf("NODE_SLOT=%d,",src->slotNumber())+
      "NAME=\""+RDEscapeString(src->primaryName())+"\","+
      QString().sprintf("NUMBER=%d",src->channelNumber());
    q=new RDSqlQuery(sql);
    delete q;
  }
}


void LiveWireLwrpAudio::destinationChangedData(unsigned id,RDLiveWireDestination *dst)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from OUTPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",livewire_matrix)+
    "(NODE_HOSTNAME=\""+RDEscapeString(livewire_nodes[id]->hostname())+"\")&&"+
    QString().sprintf("(NODE_TCP_PORT=%d)&&",livewire_nodes[id]->tcpPort())+
    QString().sprintf("(NODE_SLOT=%d)",dst->slotNumber());
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("insert into OUTPUTS set ")+
    "STATION_NAME=\""+RDEscapeString(livewire_stationname)+"\","+
    QString().sprintf("MATRIX=%d,",livewire_matrix)+
    "NODE_HOSTNAME=\""+RDEscapeString(livewire_nodes[id]->hostname())+"\","+
    QString().sprintf("NODE_TCP_PORT=%d,",livewire_nodes[id]->tcpPort())+
    QString().sprintf("NODE_SLOT=%d,",dst->slotNumber())+
    "NAME=\""+RDEscapeString(dst->primaryName())+"\","+
    QString().sprintf("NUMBER=%d",livewire_nodes[id]->baseOutput()+dst->slotNumber()-1);
  q=new RDSqlQuery(sql);
  delete q;
}


void LiveWireLwrpAudio::watchdogStateChangedData(unsigned id,const QString &msg)
{
  LogLine(RDConfig::LogNotice,msg);
}
