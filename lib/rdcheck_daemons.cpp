// rdcheck_daemons.cpp
//
// Get the status of the Rivendell system daemons.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcheck_daemons.cpp,v 1.12 2010/07/29 19:32:33 cvs Exp $
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <qstring.h>
#include <qdir.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdcheck_daemons.h>


pid_t GetPid(QString pidfile)
{
  FILE *handle;
  pid_t ret;

  if((handle=fopen((const char *)pidfile,"r"))==NULL) {
    return -1;
  }
  if(fscanf(handle,"%d",&ret)!=1) {
    ret=-1;
  }
  fclose(handle);
  return ret;
}


void RDInitializeDaemons()
{
  if(!RDStartDaemons()) {
    QMessageBox::warning(NULL,"Missing Daemons",
			 "Unable to start Rivendell daemons!");
    exit(1);
  }
}


bool RDCheckDaemon(QString name)
{
    QString cmd;
  
      pid_t daemonPid = GetPid(name);
      if(daemonPid == -1) return false;
  
      cmd=QString().sprintf("ps -p %d | grep %d > /dev/null", daemonPid, daemonPid);
      int grepResult = system(cmd);
      
      // Grep returns 0 if any lines were found
      return (grepResult == 0);
}


bool RDCheckDaemons()
{
  return RDCheckDaemon(RD_CAED_PID)&&
    RDCheckDaemon(RD_RIPCD_PID)&&
    RDCheckDaemon(RD_RDCATCHD_PID);
}


bool RDStartDaemon(QString cmd)
{
  system(cmd);
  return true;
}


bool RDStartDaemons()
{
  if(!RDCheckDaemon(RD_CAED_PID)) {
    RDKillDaemons();
    RDStartDaemon("caed");
    sleep(RD_DAEMON_PAUSE_TIME);
    if(!RDCheckDaemon(RD_CAED_PID)) {
      return false;
    }
  }
  if(!RDCheckDaemon(RD_RIPCD_PID)) {
    RDStartDaemon("ripcd");
    sleep(RD_DAEMON_PAUSE_TIME);
    if(!RDCheckDaemon(RD_RIPCD_PID)) {
      return false;
    }
  }
  if(!RDCheckDaemon(RD_RDCATCHD_PID)) {
    RDStartDaemon("rdcatchd");
    sleep(RD_DAEMON_PAUSE_TIME);
    if(!RDCheckDaemon(RD_RDCATCHD_PID)) {
      return false;
    }
  }
  return true;
}


bool RDKillDaemons()
{
  if(RDCheckDaemon(RD_CAED_PID)) {
    kill(GetPid(RD_CAED_PID),SIGTERM);
  }
  if(RDCheckDaemon(RD_RIPCD_PID)) {
    kill(GetPid(RD_RIPCD_PID),SIGTERM);
  }
  if(RDCheckDaemon(RD_RDCATCHD_PID)) {
    kill(GetPid(RD_RDCATCHD_PID),SIGTERM);
  }
  return true;
}
