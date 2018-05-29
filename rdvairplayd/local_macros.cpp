// local_macros.cpp
//
// Local RML Macros for rdvairplayd(8)
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdmacro.h>

#include "rdvairplayd.h"

void MainObject::rmlReceivedData(RDMacro *rml)
{
  QString logname;
  int fade;
  RDLogLine *logline=NULL;
  int index=-1;
  bool all_logs=false;
  int start;
  int end;
  int next_line;

  if(rml->role()!=RDMacro::Cmd) {
    return;
  }

  switch(rml->command()) {
  case RDMacro::LL:    // Load Log
    if((rml->argQuantity()<1)||(rml->argQuantity()>3)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(rml->argQuantity()==1) {   // Clear Log
      air_logs[index]->clear();
      rda->log(RDConfig::LogInfo,QString().sprintf("unloaded log machine %d",
						   rml->arg(0).toInt()));
    }
    else {  // Load Log
      logname=rml->arg(1).toString();
      if(!RDLog::exists(logname)) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  rda->ripc()->sendRml(rml);
	}
	return;
      }
      air_logs[index]->setLogName(RDLog::tableName(logname));
      air_logs[index]->load();
      rda->log(RDConfig::LogInfo,"loaded log \""+logname+"\" into log machine"+
	       QString().sprintf(" %d",rml->arg(0).toInt()));
    }
    if(rml->argQuantity()==3) { // Start Log
      if(rml->arg(2).toInt()<air_logs[index]->size()) {
	if(rml->arg(2).toInt()>=0) {  // Unconditional start
	  if(air_logs[index]->play(rml->arg(2).toInt(),RDLogLine::StartMacro)) {	
	    rda->log(RDConfig::LogInfo,QString().
		     sprintf("started log machine %d at line %d",
			     rml->arg(0).toInt(),rml->arg(2).toInt()));
	  }
	  else {
	    rda->log(RDConfig::LogWarning,QString().
		     sprintf("1 log machine %d failed to start",
			     rml->arg(0).toInt()));
	    if(rml->echoRequested()) {
	      rml->acknowledge(false);
	      rda->ripc()->sendRml(rml);
	    }   
	    return;
  }
	}
	if(rml->arg(2).toInt()==-2) {  // Start if trans type allows
	  // Find first non-running event
	  bool found=false;
	  for(int i=0;i<air_logs[index]->size();i++) {
	    if((logline=air_logs[index]->logLine(i))!=NULL) {
	      if(logline->status()==RDLogLine::Scheduled) {
		found=true;
		i=air_logs[index]->size();
	      }
	    }
	  }
	  if(found) {
	    switch(logline->transType()) {
	    case RDLogLine::Play:
	    case RDLogLine::Segue:
	      if(air_logs[index]->play(0,RDLogLine::StartMacro)) {
		rda->log(RDConfig::LogInfo,QString().
			 sprintf("started log machine %d at line 0",
				 rml->arg(0).toInt()));
	      }
	      else {
		rda->log(RDConfig::LogWarning,QString().
			 sprintf("2 log machine %d failed to start",
				 rml->arg(0).toInt()));
		if(rml->echoRequested()) {
		  rml->acknowledge(false);
		  rda->ripc()->sendRml(rml);
		}
		return;
	      }
	      break;
	      
	    case RDLogLine::Stop:
	    case RDLogLine::NoTrans:
	      break;
	    }
	  }
	}
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::AL:   // Append Log
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    logname=rml->arg(1).toString();
    if(!RDLog::exists(logname)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    air_logs[index]->append(logname);
    rda->log(RDConfig::LogInfo,QString("appended log \"")+logname+
	     QString().sprintf("\" into log machine %d",rml->arg(0).toInt()));
    break;

  case RDMacro::MN:    // Make Next
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((rml->arg(1).toInt()<0)||
       (rml->arg(1).toInt()>=air_logs[index]->size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    air_logs[index]->makeNext(rml->arg(1).toInt());
    rda->log(RDConfig::LogInfo,
	     QString().sprintf("made line %d next in log machine %d",
			       rml->arg(1).toInt(),rml->arg(0).toInt()));
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }
    break;

  case RDMacro::PL:    // Start
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((rml->arg(1).toInt()<0)||
       (rml->arg(1).toInt()>=air_logs[index]->size())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(!air_logs[index]->running()) {
      if(air_logs[index]->play(rml->arg(1).toInt(),RDLogLine::StartMacro)) {
	rda->log(RDConfig::LogInfo,QString().
		 sprintf("started log machine %d at line %d",
			 rml->arg(0).toInt(),rml->arg(2).toInt()));
      }
      else {
	rda->log(RDConfig::LogWarning,QString().
		 sprintf("3 log machine %d failed to start",
			 rml->arg(0).toInt()));
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  rda->ripc()->sendRml(rml);
	}   
	return;
      }
    }
    rda->log(RDConfig::LogInfo,QString().
	     sprintf("started log machine %d at line %d",
		     rml->arg(0).toInt(),rml->arg(1).toInt()));
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::PM:    // Set Mode
    if((rml->argQuantity()!=1)&&(rml->argQuantity()!=2)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(rml->argQuantity()==2) {
      if((index=LogMachineIndex(rml->arg(1).toInt()))<0) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  rda->ripc()->sendRml(rml);
	}
	return;
      }
    }
    start=0;
    end=RD_RDVAIRPLAY_LOG_QUAN;
    if(index>=0) {
      start=index;
      end=index+1;
    }
    switch((RDAirPlayConf::OpMode)rml->arg(0).toInt()) {
    case RDAirPlayConf::LiveAssist:
      for(int i=start;i<end;i++) {
	SetLiveAssistMode(i);
      }
      break;

    case RDAirPlayConf::Manual:
      for(int i=start;i<end;i++) {
	SetManualMode(i);
      }
      break;

    case RDAirPlayConf::Auto:
      for(int i=start;i<end;i++) {
	SetAutoMode(i);
      }
      break;

    default:
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
	return;
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::PN:    // Start Next
    if((rml->argQuantity()<1)||(rml->argQuantity()>3)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(rml->argQuantity()>=2) {
      if((rml->arg(1).toInt()<1)||(rml->arg(1).toInt()>2)) {
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  rda->ripc()->sendRml(rml);
	}
	return;
      }
      if(rml->argQuantity()==3) {
	if((rml->arg(2).toInt()<0)||(rml->arg(2).toInt()>1)) {
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    rda->ripc()->sendRml(rml);
	  }
	  return;
	}
      }
    }
    next_line=air_logs[index]->nextLine();
    if(air_logs[index]->nextLine()>=0) {
      if(rml->argQuantity()==1) {
	if(!air_logs[index]->
	   play(air_logs[index]->nextLine(),RDLogLine::StartMacro)) {
	  rda->log(RDConfig::LogWarning,QString().
		   sprintf("4 log machine %d failed to start",
			   rml->arg(0).toInt()));
	  if(rml->echoRequested()) {
	    rml->acknowledge(false);
	    rda->ripc()->sendRml(rml);
	  }   
	  return;
	}
      }
      else {
	if(rml->argQuantity()==2) {
	  if(!air_logs[index]->play(air_logs[index]->nextLine(),
				    RDLogLine::StartMacro,
				    rml->arg(1).toInt()-1)) {
	    rda->log(RDConfig::LogWarning,QString().
		     sprintf("5 log machine %d failed to start",
			     rml->arg(0).toInt()));
	    if(rml->echoRequested()) {
	      rml->acknowledge(false);
	      rda->ripc()->sendRml(rml);
	    }   
	    return;
	  }
	}
	else {
	  if(!air_logs[index]->
	     play(air_logs[index]->nextLine(),RDLogLine::StartMacro,
		  rml->arg(1).toInt()-1,rml->arg(2).toInt())) {
	    rda->log(RDConfig::LogWarning,QString().
		     sprintf("6 log machine %d failed to start",
			     rml->arg(0).toInt()));
	    if(rml->echoRequested()) {
	      rml->acknowledge(false);
	      rda->ripc()->sendRml(rml);
	    }   
	    return;
	  }
	}
      }
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("started log machine %d at line %d",
		       rml->arg(0).toInt(),next_line));
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::PS:    // Stop
    if((rml->argQuantity()<1)||(rml->argQuantity()>3)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    index=LogMachineIndex(rml->arg(0).toInt(),&all_logs);
    if((index<0)&(!all_logs)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    fade=0;
    if(rml->argQuantity()>1) {
      fade=rml->arg(1).toInt();
    }
    if(all_logs) {
      for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	air_logs[i]->stop(true,0,fade);
      }
      rda->log(RDConfig::LogInfo,"stopped all logs");
    }
    else {
      if(rml->argQuantity()==3) {
	air_logs[index]->stop(false,rml->arg(2).toInt(),fade);
      }
      else {
	air_logs[index]->stop(true,0,fade);
      }
      rda->log(RDConfig::LogInfo,QString().sprintf("stopped log machine %d",
						   rml->arg(0).toInt()));
      break;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::MD:    // Duck Machine
    if(rml->argQuantity()<3 || rml->argQuantity()>4) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    index=LogMachineIndex(rml->arg(0).toInt(),&all_logs);
    if((index<0)&&(!all_logs)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(all_logs) {
      for(int i=0;i<RD_RDVAIRPLAY_LOG_QUAN;i++) {
	air_logs[i]->duckVolume(rml->arg(1).toInt()*100,rml->arg(2).toInt());
      }
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("set volumne of all log machines to %d dBFS",
		       rml->arg(1).toInt()));
    }
    else {
      if(rml->argQuantity()==3) {
	air_logs[index]->
	  duckVolume(rml->arg(1).toInt()*100,rml->arg(2).toInt());
      }
      else {
	air_logs[index]->duckVolume(rml->arg(1).toInt()*100,
				    rml->arg(2).toInt(),rml->arg(3).toInt());
      }
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("set volumne of log machine %d to %d dBFS",
		       rml->arg(0).toInt(),
		       rml->arg(1).toInt()));
      break;
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::PX:    // Add Next
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    index=LogMachineIndex(rml->arg(0).toInt());
    if((index<0)||(rml->arg(1).toUInt()>999999)) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    next_line=air_logs[index]->nextLine();
    if(air_logs[index]->nextLine()>=0) {
      air_logs[index]->insert(air_logs[index]->nextLine(),
			      rml->arg(1).toUInt(),RDLogLine::Play);
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("inserted cart %06u at line %d on log machine %d",
		       rml->arg(1).toUInt(),next_line,rml->arg(0).toInt()));
    }
    else {
      air_logs[index]->insert(air_logs[index]->size(),
			      rml->arg(1).toUInt(),RDLogLine::Play);
      air_logs[index]->makeNext(air_logs[index]->size()-1);
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("inserted cart %06u at line %d on log machine %d",
		       rml->arg(1).toUInt(),air_logs[index]->size()-1,
		       rml->arg(0).toInt()));
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::RL:    // Refresh Log
    if(rml->argQuantity()!=1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(0).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(!air_logs[index]->refresh()) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    else {
      rda->log(RDConfig::LogInfo,QString().sprintf("refreshed log machine %d",
						   rml->arg(0).toInt()));
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  case RDMacro::SN:    // Set default Now & Next Cart
    if(rml->argQuantity()!=3) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((rml->arg(0).toString().lower()!="now")&&
       (rml->arg(0).toString().lower()!="next")) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if((index=LogMachineIndex(rml->arg(1).toInt()))<0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(rml->arg(2).toUInt()>999999) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rda->ripc()->sendRml(rml);
      }
      return;
    }
    if(rml->arg(0).toString().lower()=="now") {
      air_logs[index]->setNowCart(rml->arg(2).toUInt());
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("set default \"now\" cart to %06u on log machine %d",
		       rml->arg(2).toUInt(),rml->arg(1).toInt()));
    }
    else {
      air_logs[index]->setNextCart(rml->arg(2).toUInt());
      rda->log(RDConfig::LogInfo,QString().
	       sprintf("set default \"next\" cart to %06u on log machine %d",
		       rml->arg(2).toUInt(),rml->arg(1).toInt()));
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      rda->ripc()->sendRml(rml);
    }   
    break;

  default:
    break;
  }
}


int MainObject::LogMachineIndex(int log_mach,bool *all) const
{
  if((log_mach<=RD_RDVAIRPLAY_LOG_BASE)||
     (log_mach>RD_RDVAIRPLAY_LOG_BASE+RD_RDVAIRPLAY_LOG_QUAN)) {
    return -1;
  }
  if(all!=NULL) {
    *all=log_mach-RD_RDVAIRPLAY_LOG_BASE==0;
  }
  return log_mach-RD_RDVAIRPLAY_LOG_BASE-1;
}
