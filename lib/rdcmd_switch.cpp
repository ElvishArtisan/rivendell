// rdcmd_switch.cpp
//
// Process Rivendell Command-Line Switches
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcmd_switch.cpp,v 1.13.8.1 2012/05/10 23:12:42 cvs Exp $
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
  unsigned l=0;
  bool handled=false;
  bool debug=false;

  for(int i=1;i<argc;i++) {
#ifndef WIN32
    if(!strcmp(argv[i],"--version")) {
      printf("Rivendell v%s [%s]\n",VERSION,modname);
      exit(0);
    }
#endif  // WIN32
    if(!strcmp(argv[i],"--help")) {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    if(!strcmp(argv[i],"-d")) {
      debug=true;
    }
    l=strlen(argv[i]);
    handled=false;
    for(unsigned j=0;j<l;j++) {
      if(argv[i][j]=='=') {
	switch_keys.push_back(QString(argv[i]).left(j));
	switch_values.push_back(QString(argv[i]).right(l-(j+1)));
	switch_processed.push_back(false);
	j=l;
	handled=true;
      }
    }
    if(!handled) {
      switch_keys.push_back(QString(argv[i]));
      switch_values.push_back(QString(""));
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
