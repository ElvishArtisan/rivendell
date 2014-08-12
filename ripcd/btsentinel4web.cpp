// btsentinel4web.cpp
//
// Rivendell switcher driver for the BroadcastTools Sentinel4Web AES switcher
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: btsentinel4web.cpp,v 1.1.2.1 2014/02/17 02:19:03 cvs Exp $
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
#include <globals.h>
#include <btsentinel4web.h>


BtSentinel4Web::BtSentinel4Web(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  bt_address=matrix->ipAddress(RDMatrix::Primary);
  bt_port=matrix->ipPort(RDMatrix::Primary);

  //
  // Socket
  //
  bt_socket=new QSocket(this);
  connect(bt_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(bt_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
  connect(bt_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  //
  // Watchdog
  //
  bt_watchdog_timer=new QTimer(this);
  connect(bt_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));

  bt_watchdog_reset_timer=new QTimer(this);
  connect(bt_watchdog_reset_timer,SIGNAL(timeout()),this,SLOT(watchdogResetData()));


  bt_socket->connectToHost(bt_address.toString(),bt_port);
  bt_watchdog_timer->start(BTSENTINEL4WEB_WATCHDOG_INTERVAL,true);
}


BtSentinel4Web::~BtSentinel4Web()
{
  delete bt_socket;
}


RDMatrix::Type BtSentinel4Web::type()
{
  return RDMatrix::BtSentinel4Web;
}


unsigned BtSentinel4Web::gpiQuantity()
{
  return 0;
}


unsigned BtSentinel4Web::gpoQuantity()
{
  return 0;
}


bool BtSentinel4Web::primaryTtyActive()
{
  return false;
}


bool BtSentinel4Web::secondaryTtyActive()
{
  return false;
}


void BtSentinel4Web::processCommand(RDMacro *cmd)
{
  unsigned input;
  unsigned output;
  QString msg;

  switch(cmd->command()) {
      case RDMacro::ST:
	input=cmd->arg(1).toUInt();
	if(input>BTSENTINEL4WEB_INPUTS) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	output=cmd->arg(2).toUInt();
	if(output>BTSENTINEL4WEB_OUTPUTS) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(input==0) {
	  msg="*0MA";
	}
	else {
	  msg=QString().sprintf("*0%02u",input);
	}	
	bt_socket->writeBlock(msg,msg.length());
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void BtSentinel4Web::connectedData()
{
  bt_socket->writeBlock("*0U",3);
  ripcd_config->log("ripcd",RDConfig::LogInfo,"connected to BT Sentinel4Web device at "+
		    bt_socket->peerAddress().toString());
}


void BtSentinel4Web::errorData(int err)
{
  watchdogData();
}


void BtSentinel4Web::readyReadData()
{
  char data[1500];

  while(bt_socket->readBlock(data,1500)>0);
  bt_watchdog_timer->stop();
  bt_socket->writeBlock("*0U",3);
  bt_watchdog_timer->start(BTSENTINEL4WEB_WATCHDOG_INTERVAL,true);
}


void BtSentinel4Web::watchdogData()
{
  ripcd_config->log("ripcd",RDConfig::LogWarning,"lost connection to BT Sentinel4Web device at "+
		    bt_socket->peerAddress().toString());
  bt_watchdog_reset_timer->start(BTSENTINEL4WEB_WATCHDOG_INTERVAL,true);
}


void BtSentinel4Web::watchdogResetData()
{
  delete bt_socket;
  bt_socket=new QSocket(this);
  connect(bt_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(bt_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
  connect(bt_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  bt_socket->connectToHost(bt_address.toString(),bt_port);
  bt_watchdog_timer->start(BTSENTINEL4WEB_WATCHDOG_INTERVAL,true);
}
