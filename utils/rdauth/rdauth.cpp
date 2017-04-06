// rdauth.cpp
//
// Authenticate against a PAM service.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>

#include <rdpam.h>

#include "rdauth.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  //
  // Read Command Options
  //
  QString service_name;
  QString user;
  QString password="";
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdauth",RDAUTH_USAGE);
  if(cmd->keys()==3) {
    password=cmd->key(2);
  }
  else {
    if(cmd->keys()!=2) {
      fprintf(stderr,"rdauth: invalid arguments\n");
      exit(256);
    }
  }
  service_name=cmd->key(0);
  user=cmd->key(1);

  RDPam *pam=new RDPam(service_name);
  if(pam->authenticate(user,password)) {
    exit(0);
  }
  exit(1);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
