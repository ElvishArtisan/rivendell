// help.cpp
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

#include "rdclilogedit.h"

void MainObject::Help(const QStringList &cmds) const
{
  bool processed=false;

  if(cmds.size()==1) {
    printf("\n");
    printf("The following commands are available:\n");
    printf("?, addcart, addchain, addmarker, addtrack, bye, exit, help, list, listlogs,\n");
    printf("load, quit, remove, save, saveas, setcart, setcomment, setlabel, settime\n");
    printf("settrans, unload\n");
    printf("\n");
    printf("Enter \"? <cmd-name>\" for specific help.\n");
    printf("\n");
    processed=true;
  }
  else {
    QString verb=cmds[1].lower();
    if(verb=="addcart") {
      printf("\n");
      printf("  addcart <line> <cart-num>\n");
      printf("\n");
      printf("Add a new cart event before line <line> using cart <cart-num>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="addmarker") {
      printf("\n");
      printf("  addmarker <line>\n");
      printf("\n");
      printf("Add a new marker event before line <line>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="addchain") {
      printf("\n");
      printf("  addchain <line> <log-name>\n");
      printf("\n");
      printf("Add a new chain-to event before line <line> pointing to <log-name>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="addtrack") {
      printf("\n");
      printf("  addtrack <line>\n");
      printf("\n");
      printf("Add a new track event before line <line>.\n");
      printf("\n");
      processed=true;
    }
    if((verb=="bye")||(verb=="exit")||(verb=="quit")) {
      printf("\n");
      printf("  %s\n",(const char *)cmds[1]);
      printf("\n");
      printf("Exit the program.\n");
      printf("\n");
      processed=true;
    }
    if((verb=="?")||(verb=="help")) {
      printf("\n");
      printf("  %s <cmd-name>\n",(const char *)cmds[1]);
      printf("\n");
      printf("Print help about command <cmd-name>\n");
      printf("\n");
      processed=true;
    }
    if(verb=="listlogs") {
      printf("\n");
      printf("  listlogs\n");
      printf("\n");
      printf("Print the list of Rivendell logs.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="load") {
      printf("\n");
      printf("  load <log-name>\n");
      printf("\n");
      printf("Load the <log-name> log into the edit buffer.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="remove") {
      printf("\n");
      printf("  remove <line>\n");
      printf("\n");
      printf("Remove the log event at line <line>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="save") {
      printf("\n");
      printf("  save\n");
      printf("\n");
      printf("Save the contents of the edit buffer.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="saveas") {
      printf("\n");
      printf("  saveas <log-name>\n");
      printf("\n");
      printf("Save the contents of the edit buffer to new log <log-name>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="setcart") {
      printf("\n");
      printf("  setcart <line> <cart-num>\n");
      printf("\n");
      printf("Set the cart event at line <line> to use cart <cart-num>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="setcomment") {
      printf("\n");
      printf("  setcomment <line> <str>\n");
      printf("\n");
      printf("Set the marker or track event's \"Comment\" field at line <line> to <str>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="setlabel") {
      printf("\n");
      printf("  setlabel <line> <str>\n");
      printf("\n");
      printf("Set the chain-to or marker event's \"Label\" field at line <line> to <str>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="settime") {
      printf("\n");
      printf("  settime <line> hard|none <time>\n");
      printf("\n");
      printf("Set the start time type and value of the event at line <line>.\n");
      printf("The <time> parameter is in format \"HH:MM:SS\", and is optional when\n");
      printf("when setting \"none\".\n");
      printf("\n");
      processed=true;
    }
    if(verb=="settrans") {
      printf("\n");
      printf("  settrans <line> play|segue|stop\n");
      printf("\n");
      printf("Set the transition type of the event at line <line>.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="unload") {
      printf("\n");
      printf("  unload\n");
      printf("\n");
      printf("Unload and clear the contents of the edit buffer.\n");
      printf("\n");
      processed=true;
    }
    if(verb=="list") {
      printf("\n");
      printf("  list\n");
      printf("\n");
      printf("Print the contents of the edit buffer.\n");
      printf("\n");
      processed=true;
    }
  }

  if(!processed) {
    printf("\n");
    printf("help: no such command\n");
    printf("\n");
  }
}
