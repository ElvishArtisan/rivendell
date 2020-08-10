// rdcmd_switch.cpp
//
// Process Rivendell Command-Line Switches
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

#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>

#include <qmessagebox.h>

#include <rdcmd_switch.h>

RDCmdSwitch::RDCmdSwitch(int argc,char *argv[],const char *modname,
			 const char *usage)
{
  switch_debug=false;

  for(int i=1;i<argc;i++) {
    QString value=QString::fromUtf8(argv[i]);
    if(value=="--version") {
      printf("Rivendell v%s [%s]\n",VERSION,modname);
      exit(0);
    }
    if(value=="--help") {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    if(value=="-d") {
      switch_debug=true;
    }
    QStringList f0=value.split("=",QString::KeepEmptyParts);
    if(f0.size()>=2) {
      if(f0.at(0).left(1)=="-") {
	switch_keys.push_back(f0.at(0));
	for(int i=2;i<f0.size();i++) {
	  f0[1]+="="+f0.at(i);
	}
	if(f0.at(1).isEmpty()) {
	  switch_values.push_back("");
	}
	else {
	  switch_values.push_back(f0.at(1));
	}
      }
      else {
	switch_keys.push_back(f0.join("="));
	switch_values.push_back("");
      }
      switch_processed.push_back(false);
    }
    else {
      switch_keys.push_back(value);
      switch_values.push_back("");
      switch_processed.push_back(false);
    }
  }
}


unsigned RDCmdSwitch::keys() const
{
  return switch_keys.size();
}


QString RDCmdSwitch::key(unsigned n) const
{
  return switch_keys[n];
}


QString RDCmdSwitch::value(unsigned n) const
{
  return switch_values[n];
}


bool RDCmdSwitch::processed(unsigned n) const
{
  return switch_processed[n];
}


void RDCmdSwitch::setProcessed(unsigned n,bool state)
{
  switch_processed[n]=state;
}


bool RDCmdSwitch::allProcessed() const
{
  for(unsigned i=0;i<switch_processed.size();i++) {
    if(!switch_processed[i]) {
      return false;
    }
  }
  return true;
}


bool RDCmdSwitch::debugActive() const
{
  return switch_debug;
}
