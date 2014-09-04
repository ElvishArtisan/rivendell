// nownext.cpp
//
// Rivendell Now & Next Implementation
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: nownext.cpp,v 1.4.8.3 2014/01/13 16:51:21 cvs Exp $
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

#include <rdescape_string.h>
#include <rdnownext.h>

#include <globals.h>
#include <log_play.h>
#include <rdairplay.h>

void LogPlay::SendNowNext()
{
  QTime end_time;
  QTime time;
  int now_line=-1;
  RDLogLine *logline[2];
  RDLogLine *ll;
  RDLogLine *default_now_logline=NULL;
  RDLogLine *default_next_logline=NULL;

  //
  // Get NOW PLAYING Event
  //
  if(play_nownext_address.isNull()&&play_nownext_rml.isEmpty()&&
     (play_rlm_hosts->size()==0)) {
    return;
  }
  QString cmd=play_nownext_string;
  int lines[TRANSPORT_QUANTITY];
  int running=runningEvents(lines,false);
  for(int i=0;i<running;i++) {
    if((time=logLine(lines[i])->startTime(RDLogLine::Actual).
	addMSecs(logLine(lines[i])->effectiveLength()))>end_time) {
      end_time=time;
      now_line=lines[i];
    }
  }
  if((now_line>=0)&&(logLine(now_line)->nowNextEnabled())) {
    logline[0]=logLine(now_line);
  }
  else {
    if(play_now_cartnum==0) {
      logline[0]=NULL;
    }
    else {
      default_now_logline=new RDLogLine(play_now_cartnum);
      logline[0]=default_now_logline;
    }
  }

  //
  // Get NEXT Event
  //
  logline[1]=NULL;
  for(int i=nextLine();i<size();i++) {
    if((ll=logLine(i))!=NULL) {
      if((ll->status()==RDLogLine::Scheduled)&&
	 logLine(i)->nowNextEnabled()&&(!logLine(i)->asyncronous())) {
	logline[1]=logLine(i);
	i=size();
      }
    }
  }
  if((logline[1]==NULL)&&(play_next_cartnum!=0)) {
    default_next_logline=new RDLogLine(play_next_cartnum);
    logline[1]=default_next_logline;
  }

  //
  // Process and Send It
  //
  unsigned nowcart=0;
  unsigned nextcart=0;
  if(logline[0]!=NULL) {
    if(!logline[0]->asyncronous()) {
      nowcart=logline[0]->cartNumber();
    }
  }
  if(logline[1]!=NULL) {
    nextcart=logline[1]->cartNumber();
  }
  if((nowcart==play_prevnow_cartnum)&&(nextcart==play_prevnext_cartnum)) {
    return;
  }
  if(logline[0]==NULL) {
    play_prevnow_cartnum=0;
  }
  else {
    play_prevnow_cartnum=logline[0]->cartNumber();
  }
  if(logline[1]==NULL) {
    play_prevnext_cartnum=0;
  }
  else {
    play_prevnext_cartnum=logline[1]->cartNumber();
  }
  QString svcname=play_svc_name;
  if(svcname.isEmpty()) {
    svcname=play_defaultsvc_name;
  }
  for(unsigned i=0;i<play_rlm_hosts->size();i++) {
    play_rlm_hosts->at(i)->
      sendEvent(svcname,logName().left(logName().length()-4),play_id,logline,
		play_onair_flag,play_op_mode);
  }
  RDResolveNowNext(&cmd,logline,0);
  play_nownext_socket->
    writeBlock(cmd,cmd.length(),play_nownext_address,play_nownext_port);

  cmd=play_nownext_rml;
  RDResolveNowNext(&cmd,logline,0);
  rdevent_player->exec(cmd);

  //
  // Clean up
  //
  if(default_now_logline!=NULL) {
    delete default_now_logline;
  }
  if(default_next_logline!=NULL) {
    delete default_next_logline;
  }
}
