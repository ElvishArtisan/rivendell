// rdkernelgpio.cpp
//
// Control Class for the Linux SysFS GPIO Interface
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdkernelgpio.h"

RDKernelGpio::RDKernelGpio(QObject *parent)
  : QObject(parent)
{
  gpio_poll_timer=new QTimer(this);
  connect(gpio_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
}


RDKernelGpio::~RDKernelGpio()
{
  for(unsigned i=0;i<gpio_gpios.size();i++) {
    removeGpio(gpio_gpios[i]);
  }
  delete gpio_poll_timer;
}


bool RDKernelGpio::addGpio(int gpio)
{
  FILE *f=NULL;

  for(unsigned i=0;i<gpio_gpios.size();i++) {
    if(gpio==gpio_gpios[i]) {
      return true;
    }
  }

  if((f=OpenNode("export","w"))==NULL) {
    return false;
  }
  fprintf(f,"%u",gpio);
  fclose(f);
  gpio_gpios.push_back(gpio);
  gpio_states.push_back(value(gpio));

  if(gpio_gpios.size()==1) {
    gpio_poll_timer->start(KERNELGPIO_POLL_INTERVAL);
  }

  return true;
}


bool RDKernelGpio::removeGpio(int gpio)
{
  FILE *f=NULL;

  if((f=OpenNode("unexport","w"))==NULL) {
    return false;
  }
  fprintf(f,"%u",gpio);
  fclose(f);
  
  return true;
}


RDKernelGpio::Direction RDKernelGpio::direction(int gpio,bool *ok) const
{
  RDKernelGpio::Direction ret=RDKernelGpio::In;
  FILE *f=NULL;
  char str[255];

  if((f=OpenNode("direction","r",gpio))!=NULL) {
    fscanf(f,"%s",str);
    if(QString(str)=="out") {
      ret=RDKernelGpio::Out;
    }
    fclose(f);
    if(ok!=NULL) {
      *ok=true;
    }
  }
  else {
    if(ok!=NULL) {
      *ok=false;
    }
  }    

  return ret;
}


bool RDKernelGpio::setDirection(int gpio,RDKernelGpio::Direction dir) const
{
  FILE *f=NULL;

  if((f=OpenNode("direction","w",gpio))!=NULL) {
    switch(dir) {
    case RDKernelGpio::In:
      fprintf(f,"in");
      break;

    case RDKernelGpio::Out:
      fprintf(f,"out");
      break;
    }
    fclose(f);
    return true;
  }
  return false;
}


bool RDKernelGpio::activeLow(int gpio,bool *ok) const
{
  unsigned ret=false;
  FILE *f=NULL;

  if((f=OpenNode("active_low","r",gpio))!=NULL) {
    fscanf(f,"%u",&ret);
    fclose(f);
    if(ok!=NULL) {
      *ok=true;
    }
  }
  else {
    if(ok!=NULL) {
      *ok=false;
    }
  }    

  return (bool)ret;
}


bool RDKernelGpio::setActiveLow(int gpio,bool state) const
{
  FILE *f=NULL;

  if((f=OpenNode("active_low","w",gpio))!=NULL) {
    fprintf(f,"%u",state);
    fclose(f);
    return true;
  }
  return false;
}


bool RDKernelGpio::value(int gpio,bool *ok) const
{
  unsigned ret=false;
  FILE *f=NULL;

  if((f=OpenNode("value","r",gpio))!=NULL) {
    fscanf(f,"%u",&ret);
    fclose(f);
    if(ok!=NULL) {
      *ok=true;
    }
  }
  else {
    if(ok!=NULL) {
      *ok=false;
    }
  }    

  return (bool)ret;
}


bool RDKernelGpio::setValue(int gpio,bool state) const
{
  FILE *f=NULL;

  if((f=OpenNode("value","w",gpio))!=NULL) {
    fprintf(f,"%u",state);
    fclose(f);
    return true;
  }
  return false;
}


void RDKernelGpio::pollData()
{
  bool state=false;

  for(unsigned i=0;i<gpio_gpios.size();i++) {
    if((state=value(gpio_gpios[i]))!=gpio_states[i]) {
      gpio_states[i]=state;
      emit valueChanged(gpio_gpios[i],state);
    }
  }
}


FILE *RDKernelGpio::OpenNode(const QString &name,const char *mode,int gpio) const
{
  if(gpio<0) {
    return fopen((KERNELGPIO_SYS_FILE+"/"+name),mode);
  }
  return fopen((KERNELGPIO_SYS_FILE+QString().sprintf("/gpio%d/",gpio)+name),
	       mode);
}
