// local_audio.cpp
//
// A Rivendell switcher driver for the BroadcastTools 10x1
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: local_audio.cpp,v 1.9 2010/08/03 23:39:26 cvs Exp $
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

#include <rd.h>

#include <globals.h>
#include <local_audio.h>


LocalAudio::LocalAudio(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  //
  // Get Matrix Parameters
  //
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();
  bt_card=matrix->card();
}


LocalAudio::~LocalAudio()
{
}


RDMatrix::Type LocalAudio::type()
{
  return RDMatrix::LocalAudioAdapter;
}


unsigned LocalAudio::gpiQuantity()
{
  return 0;
}


unsigned LocalAudio::gpoQuantity()
{
  return 0;
}


bool LocalAudio::primaryTtyActive()
{
  return false;
}


bool LocalAudio::secondaryTtyActive()
{
  return false;
}


void LocalAudio::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<=0)||(cmd->arg(2).toInt()>bt_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(1).toInt()==0) {
	  for(int i=0;i<bt_inputs;i++) {
	    rdcae->
	      setPassthroughVolume(bt_card,i,cmd->arg(2).toInt()-1,-10000);
	  }
	}
	else {
	  rdcae->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
				      cmd->arg(2).toInt()-1,0);
	  for(int i=0;i<(cmd->arg(1).toInt()-1);i++) {
	    rdcae->
	      setPassthroughVolume(bt_card,i,cmd->arg(2).toInt()-1,-10000);
	  }
	  for(int i=cmd->arg(1).toInt();i<bt_inputs;i++) {
	    rdcae->
	      setPassthroughVolume(bt_card,i,cmd->arg(2).toInt()-1,-10000);
	  }
	}
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SA:
	if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<=0)||(cmd->arg(2).toInt()>bt_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	rdcae->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
				    cmd->arg(2).toInt()-1,0);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SR:
	if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<=0)||(cmd->arg(2).toInt()>bt_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	rdcae->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
				    cmd->arg(2).toInt()-1,RD_MUTE_DEPTH);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SX:
	if((cmd->arg(1).toInt()<=0)||(cmd->arg(1).toInt()>bt_inputs)||
	   (cmd->arg(2).toInt()<=0)||(cmd->arg(2).toInt()>bt_outputs)||
	   (cmd->arg(3).toInt()<RD_MUTE_DEPTH)||(cmd->arg(3).toInt()>0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	rdcae->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
				    cmd->arg(2).toInt()-1,cmd->arg(3).toInt());
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}
