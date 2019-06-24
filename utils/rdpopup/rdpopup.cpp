// rdpopup.cpp
//
// A utility for displaying messages on the desktop
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdlib.h>
#include <syslog.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>

#include "rdpopup.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

QString WordWrap(const QString &str)
{
  QString ret;
  QString residue=str;

  while(residue.length()>TEXT_WIDTH) {
    int cutpt=TEXT_WIDTH;
    for(int i=TEXT_WIDTH;i>=0;i--) {
      if(residue.at(i).isSpace()) {
	cutpt=i;
	break;
      }
    }
    ret+=residue.left(cutpt)+"\n";
    residue=residue.right(residue.length()-cutpt-1);
  }

  ret+=residue;
  return ret;
}

int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);

  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdpopup",RDPOPUP_USAGE);
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
  QString msg=WordWrap(argv[argc-1]);
  QMessageBox *mb;

  switch(prio) {
  case 1:
    mb=new QMessageBox(dt.toString("MM/dd @ hh:mm"),msg,
		       QMessageBox::Information,
		       QMessageBox::Ok,QMessageBox::
		       NoButton,QMessageBox::NoButton);
    break;

  case 2:
    mb=new QMessageBox(dt.toString("MM/dd @ hh:mm"),msg,
		       QMessageBox::Warning,QMessageBox::Ok,
		       QMessageBox::NoButton,QMessageBox::NoButton);
    break;

  case 3:
    mb=new QMessageBox(dt.toString("MM/dd @ hh:mm"),msg,
		       QMessageBox::Critical,QMessageBox::Ok,
		       QMessageBox::NoButton,QMessageBox::NoButton);
    break;

  default:
    mb=new QMessageBox(dt.toString("MM/dd @ hh:mm"),msg,
		       QMessageBox::Information,
		       QMessageBox::Ok,QMessageBox::NoButton,
		       QMessageBox::NoButton);
    break;
  }
  mb->setWindowIcon(QPixmap(rivendell_22x22_xpm));
  QFont font("helvetica",16,QFont::Bold);
  font.setPixelSize(16);
  mb->setFont(font);
  mb->exec();
  delete mb;

  RDApplication::syslog(config,LOG_INFO,"\"%s\" acknowledged",argv[argc-1]);
  closelog();

  return 0;
}
