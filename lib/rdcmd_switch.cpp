// rdcmd_switch.cpp
//
// Process Rivendell Command-Line Switches
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WIN32
#include <syslog.h>
#endif  // WIN32

#include <stdlib.h>
#include <rdcmd_switch.h>
#include <qmessagebox.h>

RDCmdSwitch::RDCmdSwitch(int argc,char *argv[],const char *modname,
			 const char *usage)
{
  bool debug=false;

  for(int i=1;i<argc;i++) {
    QString value=QString::fromUtf8(argv[i]);
#ifndef WIN32
    if(value=="--version") {
      printf("Rivendell v%s [%s]\n",VERSION,modname);
      exit(0);
    }
#endif  // WIN32
    if(value=="--help") {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    if(value=="-d") {
      debug=true;
    }
    QStringList f0=f0.split("=",value,true);
    if(f0.size()>=2) {
      switch_keys.push_back(f0[0]);
      for(unsigned i=2;i<f0.size();i++) {
	f0[1]+="="+f0[i];
      }
      switch_values.push_back(f0[1]);
      switch_processed.push_back(false);
    }
    else {
      switch_keys.push_back(value);
      switch_values.push_back("");
      switch_processed.push_back(false);
    }
  }

  //
  // Initialize Logging
  //
#ifndef WIN32
  if(debug) {
    openlog(modname,LOG_PERROR,LOG_USER);
  }
  else {
    openlog(modname,0,LOG_USER);
  }
#endif  // WIN32
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
