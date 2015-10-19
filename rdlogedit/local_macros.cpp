// local_macros.cpp
//
// RML implementation for RDLogEdit
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdlog.h>

#include "globals.h"
#include "rdlogedit.h"
#include "voice_tracker.h"

void MainWidget::RunLocalMacros(RDMacro *rml)
{
  bool ok=false;
  unsigned linenum;
  RDLog *log=NULL;
  RDLogEvent *log_event;
  VoiceTracker *vt;

  if(rml->role()!=RDMacro::Cmd) {
    return;
  }
  switch(rml->command()) {
  case RDMacro::VT:
    if(rml->argQuantity()!=2) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	rdripc->sendRml(rml);
      }
      return;
    }
    if(!log_dialog_open) {
      linenum=rml->arg(1).toUInt(&ok);
      if(ok) {
	log=new RDLog(rml->arg(0).toString());
	log_event=new RDLogEvent(RDLog::tableName(rml->arg(0).toString()));
	log_event->load();
	if(log->exists()&&(linenum<(unsigned)log_event->size())) {
	  log_dialog_open=true;
	  vt=new VoiceTracker(rml->arg(0).toString(),&log_import_path,this);
	  vt->exec(linenum);
	  log_dialog_open=false;
	}
	delete log_event;
	delete log;
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(false);
      rdripc->sendRml(rml);
    }
    break;

  default:
    break;
  }
}
