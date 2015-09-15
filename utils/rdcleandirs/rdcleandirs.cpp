// rdcleandirs.cpp
//
//  Remove stale lockfiles from user directories.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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

#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>

#include <rdcmd_switch.h>
#include <rdconf.h>

#include <rdcleandirs.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  struct passwd *pwd=NULL;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcleandirs",RDCLEANDIRS_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
  }
  if(!cmd->allProcessed()) {
    fprintf(stderr,"rdcleandirs: unknown option\n");
    exit(256);
  }

  //
  // Verify that we have root permissions
  //
  if(getuid()!=0) {
    fprintf(stderr,"rdcleandirs: this program requires root permissions\n");
    exit(256);
  }

  //
  // Iterate through the user database
  //
  while((pwd=getpwent())!=NULL) {
    QString user=QString(pwd->pw_name);
    QString base=RDGetBasePart(pwd->pw_shell);
    QString home=QString(pwd->pw_dir);
    if((user!="mysql")&&
       (base!="nologin")&&
       (base!="null")&&
       (base!="false")&&
       (base!="sync")&&
       (base!="shutdown")&&
       (base!="halt")&&
       (base!="reboot")) {
      QFile::remove(home+"/.rdairplaylock");
    }
  }
  endpwent();

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
