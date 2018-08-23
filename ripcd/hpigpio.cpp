// hpigpio.cpp
//
// A Rivendell switcher driver for AudioScience HPI GPIO devices
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qtimer.h>

#include <stdlib.h>

#include "globals.h"
#include "hpigpio.h"

HpiGpio::HpiGpio(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
/*
  //
  // Initialize Data Structures
  //
  hpi_istate=0;
  for(int i=0;i<HPIGPIO_GPIO_PINS;i++) {
    hpi_gpi_state[i]=false;
    hpi_gpi_mask[i]=false;
  }
*/
  //
  // Get Matrix Parameters
  //
  hpi_matrix=matrix->matrix();
  hpi_card=matrix->card();
  hpi_gpis=0;
  hpi_gpos=0;

#ifdef HPI
  struct hpi_control_t cntl;
  hpi_handle_t block;
  size_t value_size=0;
  size_t value_items=0;
  uint8_t *value;

  //
  // Open Mixer
  //
  if(LogHpi(HPI_MixerOpen(NULL,hpi_card,&hpi_mixer),__LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  memset(&cntl,0,sizeof(cntl));
  cntl.wSrcNodeType=HPI_SOURCENODE_ADAPTER;
  if(LogHpi(HPI_Object_BlockHandle(hpi_mixer,HPI_SOURCENODE_ADAPTER,0,0,0,
				   "GPIO",&block),__LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }

  //
  // GPIs Handle
  //
  if(LogHpi(HPI_Object_ParameterHandle(hpi_mixer,block,"Inputs",&hpi_gpis_param),__LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  if(LogHpi(HPI_Object_GetInfo(hpi_gpis_param,entity_type_boolean,
			       entity_role_value,NULL,&value_size,&value_items),
	    __LINE__)!=0) {
    UpdateDb(matrix);
    return;
  }
  hpi_gpis=value_items;

  //
  // GPOs Handle
  //
  if(LogHpi(HPI_Object_ParameterHandle(hpi_mixer,block,"Outputs",&hpi_gpos_param),__LINE__)!=0) {
    return;
  }
  if(LogHpi(HPI_Object_GetInfo(hpi_gpos_param,entity_type_boolean,
			       entity_role_value,NULL,&value_size,&value_items),
	    __LINE__)!=0) {
    hpi_gpis=0;
    UpdateDb(matrix);
    return;
  }
  hpi_gpos=value_items;

  UpdateDb(matrix);

  /*
  //
  // Interval OneShots
  //
  hpi_gpi_oneshot=new RDOneShot(this);
  connect(hpi_gpi_oneshot,SIGNAL(timeout(int)),this,SLOT(gpiOneshotData(int)));
  hpi_gpo_oneshot=new RDOneShot(this);
  connect(hpi_gpo_oneshot,SIGNAL(timeout(int)),this,SLOT(gpoOneshotData(int)));

  //
  // The Poll Timer
  //
  QTimer *timer=new QTimer(this,"poll_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(processStatus()));
  timer->start(HPIGPIO_POLL_INTERVAL);
*/
#endif  // HPI
}


HpiGpio::~HpiGpio()
{
#ifdef HPI
/*
  delete hpi_device;
  delete hpi_gpi_oneshot;
  delete hpi_gpo_oneshot;
*/
#endif  // HPI
}


RDMatrix::Type HpiGpio::type()
{
  return RDMatrix::HpiGpio;
}


unsigned HpiGpio::gpiQuantity()
{
  return hpi_gpis;
}


unsigned HpiGpio::gpoQuantity()
{
  return hpi_gpos;
}


bool HpiGpio::primaryTtyActive()
{
  return false;
}


bool HpiGpio::secondaryTtyActive()
{
  return false;
}


void HpiGpio::processCommand(RDMacro *cmd)
{
#ifdef HPI
  /*
  char str[20];

  switch(cmd->command()) {
      case RDMacro::GO:
	if((cmd->argQuantity()!=5)||
	   ((cmd->arg(1).toString().lower()!="i")&&
	    (cmd->arg(1).toString().lower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>hpi_gpos)||
	   (cmd->arg(2).toInt()>hpi_gpos)||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(1).toString().lower()!="i"))||
	   ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	    (cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).toString().lower()=="i"))||
	   (cmd->arg(4).toInt()<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(3).toInt()==0) {  // Turn OFF
	  if(cmd->arg(4).toInt()==0) {
	    if(cmd->arg(1).toString().lower()=="i") {
	      if(hpi_gpi_state[cmd->arg(2).toInt()-1]) {
		emit gpiChanged(hpi_matrix,cmd->arg(2).toInt()-1,false);
		hpi_gpi_state[cmd->arg(2).toInt()-1]=false;
	      }
	      hpi_gpi_mask[cmd->arg(2).toInt()-1]=true;
	    }
	    if(cmd->arg(1).toString().lower()=="o") {
	      sprintf(str,"*%dOR%02dF\r\n",HPIGPIO_UNIT_ID,cmd->arg(2).toInt());
	      hpi_device->writeBlock(str,9);
	      emit gpoChanged(hpi_matrix,cmd->arg(2).toInt()-1,false);
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
	  if(cmd->arg(3).toInt()==-1) {  // Clear input
	    hpi_gpi_mask[cmd->arg(2).toInt()-1]=false;
	    hpi_device->writeBlock("*0SPA\r\n",7);
	  }
	  else { 
	    if(cmd->arg(4).toInt()==0) {  // Turn ON
	      if(cmd->arg(1).toString().lower()=="i") {
		if(!hpi_gpi_state[cmd->arg(2).toInt()-1]) {
		  emit gpiChanged(hpi_matrix,cmd->arg(2).toInt()-1,true);
		  hpi_gpi_state[cmd->arg(2).toInt()-1]=true;
		}
		hpi_gpi_mask[cmd->arg(2).toInt()-1]=true;
	      }
	      if(cmd->arg(1).toString().lower()=="o") {
		sprintf(str,"*%dOR%02dL\r\n",
			HPIGPIO_UNIT_ID,cmd->arg(2).toInt());
		hpi_device->writeBlock(str,9);
		emit gpoChanged(hpi_matrix,cmd->arg(2).toInt()-1,true);
	      }
	    }
	    else {  // Pulse
	      if(cmd->arg(1).toString().lower()=="i") {
		if(!hpi_gpi_state[cmd->arg(2).toInt()-1]) {
		  emit gpiChanged(hpi_matrix,cmd->arg(2).toInt()-1,true);
		  hpi_gpi_state[cmd->arg(2).toInt()-1]=true;
		}
		hpi_gpi_mask[cmd->arg(2).toInt()-1]=true;
		hpi_gpi_oneshot->start(cmd->arg(2).toInt()-1,500);
	      }
	      if(cmd->arg(1).toString().lower()=="o") {
		sprintf(str,"*%dOR%02dP%02d\r\n",
			HPIGPIO_UNIT_ID,cmd->arg(2).toInt(),
			cmd->arg(4).toInt()/100+1);
		hpi_device->writeBlock(str,11);
		emit gpoChanged(hpi_matrix,cmd->arg(2).toInt()-1,true);
		hpi_gpo_oneshot->start(cmd->arg(2).toInt()-1,500);
	      }
	    }
	  }
	}
	if(cmd->echoRequested()) {
	  cmd->acknowledge(true);
	  emit rmlEcho(cmd);
	}
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
  */
#endif  // HPI
}


void HpiGpio::processStatus()
{
#ifdef HPI
  /*
  char buffer[256];
  int n;
  int gpi;

  while((n=hpi_device->readBlock(buffer,255))>0) {
    for(int i=0;i<n;i++) {
      switch(hpi_istate) {
	  case 0:
	    if(buffer[i]=='S') {
	      hpi_istate=1;
	    }
	    break;

	  case 1:
	    if(buffer[i]==(HPIGPIO_UNIT_ID+'0')) {
	      hpi_istate=2;
	    }
	    else {
	      hpi_istate=0;
	    }
	    break;

	  case 2:
	    if(buffer[i]=='P') {
	      hpi_istate=3;
	    }
	    else {
	      hpi_istate=0;
	    }
	    break;

	  case 3:
	    if(buffer[i]==',') {
	      hpi_istate=4;
	    }
	    else {
	      hpi_istate=0;
	    }
	    break;

	  case 4:
	    if(buffer[i]=='A') {
	      hpi_istate=5;
	    }
	    else {
	      hpi_istate=0;
	    }
	    break;

	  case 5:
	  case 7:
	  case 9:
	  case 11:
	  case 13:
	  case 15:
	  case 17:
	  case 19:
	  case 21:
	  case 23:
	  case 25:
	  case 27:
	  case 29:
	  case 31:
	  case 33:
	  case 35:
	    if(buffer[i]==',') {
	      hpi_istate++;
	    }
	    else {
	      hpi_istate=0;
	    }
	    break;

	  case 6:
	  case 8:
	  case 10:
	  case 12:
	  case 14:
	  case 16:
	  case 18:
	  case 20:
	  case 22:
	  case 24:
	  case 26:
	  case 28:
	  case 30:
	  case 32:
	  case 34:
	  case 36:
	    if(buffer[i]=='1') {
	      gpi=(hpi_istate-6)/2;
	      if(hpi_gpi_state[gpi]&&(!hpi_gpi_mask[gpi])) {
		emit gpiChanged(hpi_matrix,gpi,false);
		hpi_gpi_state[gpi]=false;
	      }
	      hpi_istate++;
	    }
	    if(buffer[i]=='0') {
	      gpi=(hpi_istate-6)/2;
	      if((!hpi_gpi_state[gpi])&&(!hpi_gpi_mask[gpi])) {
		emit gpiChanged(hpi_matrix,gpi,true);
		hpi_gpi_state[gpi]=true;
	      }
	      hpi_istate++;
	    }
	    break;

	  default:
	    hpi_istate=0;
      }
    }
  }
  */
#endif  // HPI
}


void HpiGpio::gpiOneshotData(int value)
{
#ifdef HPI
  /*
  hpi_gpi_mask[value]=false;
  hpi_device->writeBlock("*0SPA",5);
  */
#endif  // HPI
}


void HpiGpio::gpoOneshotData(int value)
{
#ifdef HPI
  //  emit gpoChanged(hpi_matrix,value,false);
#endif  // HPI
}


void HpiGpio::UpdateDb(RDMatrix *matrix) const
{
  matrix->setGpis(hpi_gpis);
  matrix->setGpos(hpi_gpos);
}


#ifdef HPI
hpi_err_t HpiGpio::LogHpi(hpi_err_t err,int lineno)
{
  char err_txt[200];

  if(err!=0) {
    HPI_GetErrorText(err,err_txt);
    syslog(LOG_NOTICE,"HPI Error: %s, %s line %d",err_txt,__FILE__,lineno);
  }
  return err;
}
#endif  // HPI
