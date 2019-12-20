// local_audio.cpp
//
// A Rivendell switcher driver for local audio cards.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rdapplication.h>

#include "globals.h"
#include "local_audio.h"

LocalAudio::LocalAudio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  bt_poll_timer=NULL;
  bt_gpi_values=NULL;
  bt_gpo_values=NULL;

  //
  // Get Matrix Parameters
  //
  bt_inputs=matrix->inputs();
  bt_outputs=matrix->outputs();
  bt_card=matrix->card();
  bt_gpis=0;
  bt_gpos=0;

  //
  // Interval OneShot
  //
  bt_gpo_oneshot=new RDOneShot(this);
  connect(bt_gpo_oneshot,SIGNAL(timeout(int)),this,SLOT(gpoOneshotData(int)));

  InitializeHpi(matrix);
}


LocalAudio::~LocalAudio()
{
  if(bt_poll_timer!=NULL) {
    delete bt_poll_timer;
  }
  if(bt_gpi_values!=NULL) {
    delete bt_gpi_values;
  }
  if(bt_gpo_values!=NULL) {
    delete bt_gpo_values;
  }
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
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       ((cmd->arg(1).lower()!="i")&&
	(cmd->arg(1).lower()!="o"))||
       (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>bt_gpos)||
       (cmd->arg(2).toInt()>bt_gpos)||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(1).lower()!="i"))||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).lower()=="i"))||
       (cmd->arg(4).toInt()<0)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(3).toInt()==0) {  // Turn OFF
      if(cmd->arg(4).toInt()==0) {
	if(cmd->arg(1).lower()=="o") {
	  SetGpo(cmd->arg(2).toInt()-1,false);
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,false);
	}
      }
      else {
	if(cmd->echoRequested()) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	}
	return;
      }
    }
    else {
      if(cmd->arg(3).toInt()!=-1) {
	if(cmd->arg(4).toInt()==0) {  // Turn ON
	  if(cmd->arg(1).lower()=="o") {
	    SetGpo(cmd->arg(2).toInt()-1,true);
	    emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,true);
	  }
	}
	else {  // Pulse
	  if(cmd->arg(1).lower()=="o") {
	    SetGpo(cmd->arg(2).toInt()-1,true);
	    emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,true);
	    bt_gpo_oneshot->
	      start(cmd->arg(2).toInt()-1,cmd->arg(4).toInt());
	  }
	}
      }
    }
    if(cmd->echoRequested()) {
      cmd->acknowledge(true);
      emit rmlEcho(cmd);
    }
    break;

  case RDMacro::ST:
    if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>bt_inputs)||
       (cmd->arg(2).toInt()<=0)||(cmd->arg(2).toInt()>bt_outputs)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(1).toInt()==0) {
      for(int i=0;i<bt_inputs;i++) {
	rda->cae()->
	  setPassthroughVolume(bt_card,i,cmd->arg(2).toInt()-1,-10000);
      }
    }
    else {
      rda->cae()->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
				       cmd->arg(2).toInt()-1,0);
      for(int i=0;i<(cmd->arg(1).toInt()-1);i++) {
	rda->cae()->
	  setPassthroughVolume(bt_card,i,cmd->arg(2).toInt()-1,-10000);
      }
      for(int i=cmd->arg(1).toInt();i<bt_inputs;i++) {
	rda->cae()->
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
    rda->cae()->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
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
    rda->cae()->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
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
    rda->cae()->setPassthroughVolume(bt_card,cmd->arg(1).toInt()-1,
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


void LocalAudio::pollData()
{
#ifdef HPI
  if(LogHpi(HPI_Object_GetValue(bt_gpis_param,entity_type_boolean,
				bt_gpis,bt_gpi_values,
				bt_gpis),__LINE__)==0) {
    for(int i=0;i<bt_gpis;i++) {
      if((bt_gpi_values[i]=='T')!=bt_gpi_states[i]) {
	bt_gpi_states[i]=(bt_gpi_values[i]=='T');
	emit gpiChanged(matrixNumber(),i,bt_gpi_states[i]);
      }
    }
  }
#endif  // HPI
}


void LocalAudio::gpoOneshotData(int value)
{
  SetGpo(value,false);
  emit gpoChanged(matrixNumber(),value,false);
}


void LocalAudio::InitializeHpi(RDMatrix *matrix)
{
#ifdef HPI
  struct hpi_control_t cntl;
  hpi_handle_t block;
  size_t value_size=0;
  size_t value_items=0;
  hpi_err_t hpi_err;
  char err_txt[200];

  bt_gpis=0;
  bt_gpos=0;

  //
  // Open Mixer
  //
  if(LogHpi(HPI_MixerOpen(NULL,bt_card,&bt_mixer),__LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  memset(&cntl,0,sizeof(cntl));
  cntl.wSrcNodeType=HPI_SOURCENODE_ADAPTER;
  if((hpi_err=HPI_Object_BlockHandle(bt_mixer,HPI_SOURCENODE_ADAPTER,0,0,0,
				     "GPIO",&block))!=0) {
    HPI_GetErrorText(hpi_err,err_txt);
    rda->syslog(LOG_DEBUG,
		"matrix %d: unable to open HPI block object \"GPIO\" [%s]",
		bt_card,(const char *)err_txt);
    UpdateDb(matrix);
    return;
  }

  //
  // GPIs Handle
  //
  if(LogHpi(HPI_Object_ParameterHandle(bt_mixer,block,"Inputs",&bt_gpis_param),__LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  if(LogHpi(HPI_Object_GetInfo(bt_gpis_param,entity_type_boolean,
			       entity_role_value,NULL,&value_size,&value_items),
	    __LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  bt_gpis=value_items;
  bt_gpi_values=new uint8_t[bt_gpis];
  if(LogHpi(HPI_Object_GetValue(bt_gpis_param,entity_type_boolean,value_items,bt_gpi_values,value_size),__LINE__)!=0) {
    bt_gpis=0;
    UpdateDb(matrix);
    return;
  }
  for(int i=0;i<bt_gpis;i++) {
    bt_gpi_states.push_back(bt_gpi_values[i]=='T');
  }
  bt_poll_timer=new QTimer(this);
  connect(bt_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
  bt_poll_timer->start(LOCALAUDIO_POLL_INTERVAL);

  //
  // GPOs Handle
  //
  if(LogHpi(HPI_Object_ParameterHandle(bt_mixer,block,"Outputs",&bt_gpos_param),__LINE__)!=0) {
    return;
  }
  if(LogHpi(HPI_Object_GetInfo(bt_gpos_param,entity_type_boolean,
			       entity_role_value,NULL,&value_size,&value_items),
	    __LINE__)!=0) {
    bt_gpis=0;
    UpdateDb(matrix);
    return;
  }
  bt_gpos=value_items;
  bt_gpo_values=new uint8_t[bt_gpos];
  if(LogHpi(HPI_Object_GetValue(bt_gpos_param,entity_type_boolean,value_items,bt_gpo_values,value_size),__LINE__)!=0) {
    bt_gpis=0;
    bt_gpos=0;
    UpdateDb(matrix);
    return;
  }

  UpdateDb(matrix);
#endif  // HPI
}


void LocalAudio::SetGpo(int line,bool state)
{
#ifdef HPI
  if(LogHpi(HPI_Object_GetValue(bt_gpos_param,entity_type_boolean,
				bt_gpos,bt_gpo_values,
				bt_gpos),__LINE__)==0) {
    if(state) {
      bt_gpo_values[line]='T';
    }
    else {
      bt_gpo_values[line]='F';
    }
    LogHpi(HPI_Object_SetValue(bt_gpos_param,entity_type_boolean,bt_gpos,
			       bt_gpo_values,bt_gpos),__LINE__);
  }
#endif  // HPI
}


void LocalAudio::UpdateDb(RDMatrix *matrix) const
{
  matrix->setGpis(bt_gpis);
  matrix->setGpos(bt_gpos);
}


#ifdef HPI
hpi_err_t LocalAudio::LogHpi(hpi_err_t err,int lineno)
{
  char err_txt[200];

  if(err!=0) {
    HPI_GetErrorText(err,err_txt);
    rda->syslog(LOG_WARNING,"HPI Error: %s, %s line %d",err_txt,
		__FILE__,lineno);
  }
  return err;
}
#endif  // HPI
