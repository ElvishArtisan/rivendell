// upload_test.cpp
//
// Test Rivendell file uploading.
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QDebug>

#include <rdapplication.h>

#include "notification_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  rda=new RDApplication("notification_test","notification_test",NOTIFICATION_TEST_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"notification_test: %s\n",(const char *)err_msg);
    exit(1);
  }

  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdrepld: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Run the Test
  //
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
          this,SLOT(notificationReceivedData(RDNotification *)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::notificationReceivedData(RDNotification *notify)
{
  printf("%s\n",(const char *)notify->write());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
