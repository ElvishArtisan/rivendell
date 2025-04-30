// rdpopup.cpp
//
// A utility for displaying messages on the desktop
//
//   (C) Copyright 2009-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QMessageBox>

#include <rd.h>
#include <rdapplication.h>
#include <rdcmd_switch.h>

#include "rdpopup.h"

int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);

  RDCmdSwitch *cmd=new RDCmdSwitch("rdpopup",RDPOPUP_USAGE);
  delete cmd;

  if(argc<2) {
    fprintf(stderr,"rdpopup: missing argument(s)\n");
    exit(256);
  }

  //
  // Open the config
  //
  RDConfig *config=new RDConfig();
  config->load();
  openlog("rdpopup",LOG_ODELAY,config->syslogFacility());

  //
  // Get Severity Value
  //
  bool ok=false;
  int prio=QString(argv[argc-2]).toInt(&ok);
  if(!ok) {
    fprintf(stderr,"rdpopup: invalid priority value\n");
    exit(256);
  }

  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
  QString msg(argv[argc-1]);

  switch(prio) {
  case 1:
    QMessageBox::information(NULL,dt.toString("MM/dd @ hh:mm"),msg);
    break;

  case 2:
    QMessageBox::warning(NULL,dt.toString("MM/dd @ hh:mm"),msg);
    break;

  case 3:
    QMessageBox::critical(NULL,dt.toString("MM/dd @ hh:mm"),msg);
    break;

  default:
    QMessageBox::information(NULL,dt.toString("MM/dd @ hh:mm"),msg);
    break;
  }

  RDApplication::syslog(config,LOG_INFO,"\"%s\" acknowledged",argv[argc-1]);
  closelog();

  return 0;
}
