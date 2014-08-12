//   rdgpio.cpp
//
//   A driver for General-Purpose I/O devices.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdgpio.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <qobject.h>

#include <rdgpio.h>


RDGpio::RDGpio(QObject *parent,const char *name)
  : QObject(parent,name)
{
  Clear();

  //
  // Input Timer
  //
  gpio_input_timer=new QTimer(this,"input_timer");
  connect(gpio_input_timer,SIGNAL(timeout()),this,SLOT(inputTimerData()));
  gpio_revert_mapper=NULL;
  for(int i=0;i<GPIO_MAX_LINES;i++) {
    gpio_revert_timer[i]=NULL;
  }
  for(int i=0;i<KEY_MAX;i++) {
    gpio_key_map[i]=-1;
  }
}


QString RDGpio::device() const
{
  return gpio_device;
}


void RDGpio::setDevice(QString dev)
{
  gpio_device=dev;
}


QString RDGpio::description() const
{
  return gpio_description;
}


RDGpio::Mode RDGpio::mode()
{
  struct gpio_info info;

  switch(gpio_api) {
      case RDGpio::ApiGpio:
	ioctl(gpio_fd,GPIO_GETINFO,&info);
	return (RDGpio::Mode)info.mode;

      case RDGpio::ApiInput:
	return RDGpio::Auto;
  }
  return RDGpio::Auto;
}


void RDGpio::setMode(RDGpio::Mode mode)
{
  unsigned umode=(unsigned)mode;

  switch(gpio_api) {
      case RDGpio::ApiGpio:
	ioctl(gpio_fd,GPIO_SETMODE,&umode);
	RemapTimers();
	break;

      default:
	break;
  }
}


bool RDGpio::open()
{
  int ver;
  
  if(gpio_open) {
    return false;
  }
  if((gpio_fd=::open((const char *)gpio_device,O_RDONLY|O_NONBLOCK))<0) {
    return false;
  }
  if(ioctl(gpio_fd,GPIO_GETINFO,&gpio_info)==0) {
    gpio_api=RDGpio::ApiGpio;
    InitGpio();
    RemapTimers();
  }
  else {
    if(ioctl(gpio_fd,EVIOCGVERSION,&ver)==0) {
      gpio_api=RDGpio::ApiInput;
      InitInput();
    }
    else {
      ::close(gpio_fd);
      return false;
    }
  }
  gpio_open=true;
  gpio_input_timer->start(GPIO_CLOCK_INTERVAL);
  return true;
}


void RDGpio::close()
{
  if(!gpio_open) {
    return;
  }
  gpio_input_timer->stop();
  ::close(gpio_fd);
  gpio_open=false;
  if(gpio_revert_mapper!=NULL) {
    delete gpio_revert_mapper;
    gpio_revert_mapper=NULL;
  }
  for(int i=0;i<outputs();i++) {
    if(gpio_revert_timer[i]!=NULL) {
      delete gpio_revert_timer[i];
      gpio_revert_timer[i]=NULL;
    }
  }
}


int RDGpio::inputs() const
{
  if(!gpio_open) {
    return 0;
  }
  return gpio_info.inputs;
}


int RDGpio::outputs() const
{
  if(!gpio_open) {
    return 0;
  }
  return gpio_info.outputs;
}


unsigned RDGpio::inputMask()
{
  struct gpio_mask mask;
  struct input_event input;
  static unsigned input_mask=0;

  if(!gpio_open) {
    return 0;
  }
  switch(gpio_api) {
      case RDGpio::ApiGpio:
	memset(&mask,0,sizeof(struct gpio_mask));
	ioctl(gpio_fd,GPIO_GET_INPUTS,&mask);
	return mask.mask[0];

      case RDGpio::Input:
	while(read(gpio_fd,&input,sizeof(input))>0) {
	  if((input.type==EV_KEY)&&(gpio_key_map[input.code]>=0)) {
	    if(input.value==0) {
	      input_mask&=~(1<<gpio_key_map[input.code]);
	    }
	    else {
	      input_mask|=(1<<gpio_key_map[input.code]);
	    }
	  }
	}
	return input_mask;
  }
  return 0;
}


bool RDGpio::inputState(int line)
{
  if(!gpio_open) {
    return false;
  }
  if((inputMask()&(1<<line))==0) {
    return false;
  }
  return true;
}


unsigned RDGpio::outputMask() const
{
  struct gpio_mask mask;

  if(!gpio_open) {
    return 0;
  }
  switch(gpio_api) {
      case RDGpio::ApiGpio:
	memset(&mask,0,sizeof(struct gpio_mask));
	ioctl(gpio_fd,GPIO_GET_OUTPUTS,&mask);
	return mask.mask[0];

      case RDGpio::ApiInput:
	return 0;
  }
  return 0;
}


void RDGpio::gpoSet(int line,unsigned interval)
{
  struct gpio_line out;

  if(!gpio_open) {
    return;
  }
  switch(gpio_api) {
      case RDGpio::ApiGpio:
	out.line=line;
	out.state=1;
	ioctl(gpio_fd,GPIO_SET_OUTPUT,&out);
	SetReversion(line,interval);
	break;

      case RDGpio::ApiInput:
	break;
  }
}


void RDGpio::gpoReset(int line,unsigned interval)
{
  struct gpio_line out;

  if(!gpio_open) {
    return;
  }
  switch(gpio_api) {
      case RDGpio::ApiGpio:
	out.line=line;
	out.state=0;
	ioctl(gpio_fd,GPIO_SET_OUTPUT,&out);
	SetReversion(line,interval);
	break;

      case RDGpio::ApiInput:
	break;
  }
}


void RDGpio::inputTimerData()
{
  unsigned input_mask;
  unsigned output_mask;
  unsigned mask;

  if((input_mask=inputMask())!=gpio_input_mask) {
    for(int i=0;i<inputs();i++) {
      mask=1<<i;
      if((gpio_input_mask&mask)!=(input_mask&mask)) {
	if((input_mask&mask)==0) {
	  emit inputChanged(i,false);
	}
	else {
	  emit inputChanged(i,true);
	}
      }
    }
    gpio_input_mask=input_mask;
  }
  if((output_mask=outputMask())!=gpio_output_mask) {
    for(int i=0;i<outputs();i++) {
      mask=1<<i;
      if((gpio_output_mask&mask)!=(output_mask&mask)) {
	if((output_mask&mask)==0) {
	  emit outputChanged(i,false);
	}
	else {
	  emit outputChanged(i,true);
	}
      }
    }
    gpio_output_mask=output_mask;
  }
}
  
  void RDGpio::revertData(int id)
{
  if((outputMask()&(1<<id))==0) {
    gpoSet(id);
  }
  else {
    gpoReset(id);
  }
}


void RDGpio::RemapTimers()
{
  struct gpio_info info;

  //
  // Free Old Timers
  //
  if(gpio_revert_mapper!=NULL) {
    delete gpio_revert_mapper;
    gpio_revert_mapper=NULL;
  }
  for(int i=0;i<gpio_info.outputs;i++) {
    if(gpio_revert_timer[i]!=NULL) {
      delete gpio_revert_timer[i];
      gpio_revert_timer[i]=NULL;
    }
  }

  //
  // Create New Timers
  //
  ioctl(gpio_fd,GPIO_GETINFO,&info);
  gpio_revert_mapper=new QSignalMapper(this,"gpio_revert_mapper");
  connect(gpio_revert_mapper,SIGNAL(mapped(int)),this,SLOT(revertData(int)));
  for(int i=0;i<info.outputs;i++) {
    gpio_revert_timer[i]=new QTimer(this);
    gpio_revert_mapper->setMapping(gpio_revert_timer[i],i);
    connect(gpio_revert_timer[i],SIGNAL(timeout()),
	    gpio_revert_mapper,SLOT(map()));
  }
}


void RDGpio::SetReversion(int line,unsigned interval)
{
  if(interval==0) {
    if(gpio_revert_timer[line]->isActive()) {
      gpio_revert_timer[line]->stop();
    }
    return;
  }
  if(gpio_revert_timer[line]->isActive()) {
    gpio_revert_timer[line]->changeInterval(interval);
  }
  else {
    gpio_revert_timer[line]->start(interval,true);
  }
}


void RDGpio::Clear()
{
  gpio_open=false;
  gpio_description="Unknown Device";
  memset(&gpio_info,0,sizeof(struct gpio_info));
  gpio_input_mask=0;
  gpio_api=RDGpio::ApiGpio;
  for(int i=0;i<KEY_MAX;i++) {
    gpio_key_map[i]=-1;
  }
}


void RDGpio::InitGpio()
{
  gpio_description=gpio_info.name;

  //
  // Enable Input Filters
  //
  if((gpio_info.caps&GPIO_CAP_FILTER)!=0) {
    gpio_mask mask;
    memset(&mask,0xFF,sizeof(struct gpio_mask));
    ioctl(gpio_fd,GPIO_SET_FILTERS,&mask);
  }
}


void RDGpio::InitInput()
{
  unsigned char bitmask[EV_MAX/8+1];
  unsigned char keymask[KEY_MAX/8+1];
  int i=0;
  char desc[256];

  memset(&gpio_info,0,sizeof(struct gpio_info));

  if(ioctl(gpio_fd,EVIOCGNAME(256),desc)>=0) {
    gpio_description=desc;
  }
  if(ioctl(gpio_fd,EVIOCGBIT(0,EV_MAX),bitmask)<0) {
    return;
  }
  if((bitmask[0]&(1<<EV_KEY))==0) {
    return;
  }
  ioctl(gpio_fd,EVIOCGBIT(EV_KEY,KEY_MAX),keymask);
  while((i<KEY_MAX)&&(gpio_info.inputs<GPIO_MAX_LINES)) {
    if((keymask[i/8]&(1<<(i%8)))!=0) {
      gpio_key_map[i]=gpio_info.inputs++;
    }
    i++;
  }
  gpio_info.mode=RDGpio::Auto;
  gpio_info.outputs=0;
}

