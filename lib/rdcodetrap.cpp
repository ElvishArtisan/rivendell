//   rdcodetrap.cpp
//
//   A class for trapping arbitrary character sequences.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcodetrap.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#include <string.h>

#include <rdcodetrap.h>


RDCodeTrap::RDCodeTrap(QObject *parent,const char *name)
  : QObject(parent,name)
{
}


RDCodeTrap::~RDCodeTrap()
{
}


void RDCodeTrap::addTrap(int id,const char *code,int length)
{
  for(unsigned i=0;i<trap_events.size();i++) {
    if(length==trap_events[i].length) {
      if((trap_events[i].id==id)&&
	 (!strncmp(code,trap_events[i].code,length))) {
	return;
      }
    }
  }
  trap_events.push_back(RTrapEvent());
  trap_events.back().id=id;
  trap_events.back().code=new char[length];
  memcpy(trap_events.back().code,code,length);
  trap_events.back().length=length;
  trap_events.back().istate=0;
}


void RDCodeTrap::removeTrap(int id)
{
  for(unsigned i=0;i<trap_events.size();i++) {
    if(trap_events[i].id==id) {
      delete trap_events[i].code;
      vector<RTrapEvent>::iterator it=trap_events.begin()+i;
      trap_events.erase(it,it+1);
      i--;
    }
  }
}


void RDCodeTrap::removeTrap(const char *code,int length)
{
  for(unsigned i=0;i<trap_events.size();i++) {
    if(length==trap_events[i].length) {
      if(!strncmp(code,trap_events[i].code,length)) {
	delete trap_events[i].code;
	vector<RTrapEvent>::iterator it=trap_events.begin()+i;
	trap_events.erase(it,it+1);
	i--;
      }
    }
  }
}


void RDCodeTrap::removeTrap(int id,const char *code,int length)
{
  for(unsigned i=0;i<trap_events.size();i++) {
    if(length==trap_events[i].length) {
      if((trap_events[i].id==id)&&
	 (!strncmp(code,trap_events[i].code,length))) {
	delete trap_events[i].code;
	vector<RTrapEvent>::iterator it=trap_events.begin()+i;
	trap_events.erase(it,it+1);
	i--;
      }
    }
  }
}


void RDCodeTrap::scan(const char *buf,int length)
{
  for(unsigned i=0;i<trap_events.size();i++) {
    for(int j=0;j<length;j++) {
      if(buf[j]==trap_events[i].code[trap_events[i].istate]) {
	trap_events[i].istate++;
      }
      else {
	trap_events[i].istate=0;
      }
      if(trap_events[i].istate==trap_events[i].length) {
	emit trapped(trap_events[i].id);
	trap_events[i].istate=0;
      }
    }
  }
}


void RDCodeTrap::clear()
{
  trap_events.clear();
}
