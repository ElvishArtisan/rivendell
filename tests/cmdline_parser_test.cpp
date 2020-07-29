// cmdline_parser_test.cpp
//
// Test the Rivendell command-line parser routines.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>

#include <rdcmd_switch.h>

#include "cmdline_parser_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"cmdline_parser_test",
		    CMDLINE_PARSER_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    printf("  key[%d]: %s\n",i,cmd->key(i).utf8().constData());
    printf("value[%d]: %s\n",i,cmd->value(i).toUtf8().constData());
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
