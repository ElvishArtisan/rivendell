// rdclilogedit.cpp
//
// A command-line log editor for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdweb.h>

#include "rdclilogedit.h"

void MainObject::DispatchCommand(QString cmd)
{ 
  bool processed=false;
  int line;
  QTime time;
  bool ok=false;
  bool overwrite=!edit_modified;
  QStringList cmds;
  QString verb;

  cmd=cmd.stripWhiteSpace();
  if(cmd.right(1)=="!") {
    overwrite=true;
    cmd=cmd.left(cmd.length()-1).stripWhiteSpace();
  }
  cmds=cmds.split(" ",cmd);
  verb=cmds[0].lower();

  //
  // No loaded log needed for these
  //
  if((verb=="exit")||(verb=="quit")||(verb=="bye")) {
    if(overwrite) {
      exit(0);
    }
    else {
      OverwriteError(verb);
    }
    processed=true;
  }

  if((verb=="help")||(verb=="?")) {
    Help(cmds);
    processed=true;
  }

  if(verb=="listlogs") {
    ListLogs();
    processed=true;
  }

  if(verb=="load") {
    if(overwrite) {
      if(cmds.size()==2) {
	Load(cmds[1]);
      }
      else {
	fprintf(stderr,"load: invalid command arguments\n");
      }
    }
    else {
      OverwriteError("load");
    }
    processed=true;
  }

  if(verb=="new") {
    if(overwrite) {
      if(cmds.size()==2) {
	New(cmds[1]);
      }
      else {
	fprintf(stderr,"new: invalid command arguments\n");
      }
    }
    else {
      OverwriteError("new");
    }
    processed=true;
  }

  //
  // These need a log loaded
  //
  if((processed)||(edit_log_event!=NULL)) {
    if(verb=="addcart") {
      if(edit_user->addtoLog()) {
	if(cmds.size()==3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)) {
	    unsigned cartnum=cmds[2].toUInt(&ok);
	    if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	      Addcart(line,cartnum);
	    }
	    else {
	      fprintf(stderr,"addcart: invalid cart number\n");
	    }
	  }
	  else {
	    fprintf(stderr,"addcart: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"addcart: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,"addcart: insufficient privileges [Add Log Items]\n");
      }
      processed=true;
    }

    if(verb=="addchain") {
      if(edit_user->addtoLog()) {
	if(cmds.size()==3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)) {
	    Addchain(line,cmds[2]);
	  }
	  else {
	    fprintf(stderr,"addchain: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"addchain: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,"addchain: insufficient privileges [Add Log Items]\n");
      }
      processed=true;
    }
    
    if(verb=="addmarker") {
      if(edit_user->addtoLog()) {
	if(cmds.size()==2) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)) {
	    Addmarker(line);
	  }
	  else {
	    fprintf(stderr,"addmarker: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"addmarker: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,"addmarker: insufficient privileges [Add Log Items]\n");
      }
      processed=true;
    }
    
    if(verb=="addtrack") {
      if(edit_user->addtoLog()) {
	if(cmds.size()==2) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)) {
	    Addtrack(line);
	  }
	  else {
	    fprintf(stderr,"addtrack: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"addtrack: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,"addtrack: insufficient privileges [Add Log Items]\n");
      }
      processed=true;
    }
    
    if(verb=="list") {
      List();
      processed=true;
    }

    if(verb=="remove") {
      if(edit_user->removefromLog()) {
	if(cmds.size()==2) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    Remove(line);
	  }
	  else {
	    fprintf(stderr,"remove: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"remove: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,"remove: insufficient privileges [Delete Log Items]\n");
      }
      processed=true;
    }

    if(verb=="save") {
      if(edit_user->arrangeLog()) {
	Save();
      }
      else {
	fprintf(stderr,"save: insufficient privileges [Rearrange Log Items]\n");
      }
      processed=true;
    }

    if(verb=="saveas") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()==2) {
	if(cmds[1].length()>64) {
	  fprintf(stderr,"saveas: log name too long\n");
	}
	Saveas(cmds[1]);
	}
	else {
	  fprintf(stderr,"saveas: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"saveas: insufficient privileges [Rearrange Log Items]\n");
      }
      processed=true;
    }

    if(verb=="setcart") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()==3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    unsigned cartnum=cmds[2].toUInt(&ok);
	    if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	      Setcart(line,cartnum);
	    }
	    else {
	      fprintf(stderr,"setcart: invalid cart number\n");
	    }
	  }
	  else {
	    fprintf(stderr,"setcart: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"setcart: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"setcart: insufficient privileges [Rearrange Log Items]\n");
      }
      processed=true;
    }

    if(verb=="setcomment") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()>=3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    cmds.remove(cmds.begin());
	    cmds.remove(cmds.begin());
	    Setcomment(line,cmds.join(" "));
	  }
	  else {
	    fprintf(stderr,"setcomment: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"setcomment: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"setcomment: insufficient privileges [Rearrange Log Items]\n");
      }  
      processed=true;
    }

    if(verb=="setlabel") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()==3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    Setlabel(line,cmds[2]);
	  }
	  else {
	    fprintf(stderr,"setlabel: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"setlabel: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"setlabel: insufficient privileges [Rearrange Log Items]\n");
      }  
      processed=true;
    }

    if(verb=="settime") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()>=3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    RDLogLine::TimeType ttype=RDLogLine::NoTime;
	    if(cmds[2].lower()=="hard") {
	      ttype=RDLogLine::Hard;
	    }
	    if(cmds[2].lower()=="none") {
	      ttype=RDLogLine::Relative;
	    }
	    switch(ttype) {
	    case RDLogLine::Hard:
	      if(cmds.size()>=4) {
		time=RDGetWebTime(cmds[3],&ok);
		if(ok) {
		  Settime(line,ttype,time);
		}
		else {
		  fprintf(stderr,"settime: invalid time value\n");
		}
	      }
	      else {
		fprintf(stderr,"settime: missing time value\n");
	      }
	      break;

	    case RDLogLine::Relative:
	      Settime(line,ttype);
	      break;

	    case RDLogLine::NoTime:
	      fprintf(stderr,"settime: invalid time type\n");
	      break;
	    }
	  }
	  else {
	    fprintf(stderr,"settime: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"settime: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"settime: insufficient privileges [Rearrange Log Items]\n");
      }  
      processed=true;
    }

    if(verb=="settrans") {
      if(edit_user->arrangeLog()) {
	if(cmds.size()==3) {
	  line=cmds[1].toInt(&ok);
	  if(ok&&(line>=0)&&(line<edit_log_event->size())) {
	    RDLogLine::TransType trans=RDLogLine::NoTrans;
	    if(cmds[2].lower()=="play") {
	      trans=RDLogLine::Play;
	    }
	    if(cmds[2].lower()=="segue") {
	      trans=RDLogLine::Segue;
	    }
	    if(cmds[2].lower()=="stop") {
	      trans=RDLogLine::Stop;
	    }
	    if(trans!=RDLogLine::NoTrans) {
	      Settrans(line,trans);
	    }
	    else {
	      fprintf(stderr,"settrans: invalid transition type\n");
	    }
	  }
	  else {
	    fprintf(stderr,"settrans: invalid line number\n");
	  }
	}
	else {
	  fprintf(stderr,"settrans: invalid command arguments\n");
	}
      }
      else {
	fprintf(stderr,
		"settrans: insufficient privileges [Rearrange Log Items]\n");
      }  
      processed=true;
    }

    if(verb=="unload") {
      if(overwrite) {
	Unload();
      }
      else {
	OverwriteError("unload");
      }
      processed=true;
    }
  }
  else {
    fprintf(stderr,"%s: no log loaded\n",(const char *)verb);
    processed=true;
  }

  if(!processed) {
    fprintf(stderr,"invalid command\n");
  }
  PrintPrompt();
}
