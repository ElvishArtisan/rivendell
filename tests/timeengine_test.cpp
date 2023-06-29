// timeengine_test.cpp
//
// Test the RDTimeEngine class
//
//   (C) Copyright 2021-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>
#include <QMultiMap>
#include <QStringList>

#include <rdapplication.h>

#include "timeengine_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type;
  QMultiMap<QTime,int> time_ids;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("timeengine_test",
							 "timeengine_test",
							 TIMEENGINE_TEST_USAGE,
							 false,this));
  if(!rda->open(&err_msg,&err_type,false,false)) {
    fprintf(stderr,"timeengine_test: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--time-event") {
      bool ok=false;
      int id;
      QTime time;
      QStringList f0=
	rda->cmdSwitch()->value(i).split(":",QString::SkipEmptyParts);
      if(f0.size()!=4) {
	fprintf(stderr,"timeengine_test: invalid argument format\n");
	exit(1);
      }
      id=f0.at(0).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"timeengine_test: invalid ID value\n");
	exit(1);
      }
      f0.removeFirst();
      time=QTime::fromString(f0.join(":"),"hh:mm:ss");
      if(time.isNull()) {
	fprintf(stderr,"timeengine_test: invalid time \"%s\" specified\n",
		f0.join(":").toUtf8().constData());
	exit(1);
      }
      time_ids.insert(time,id);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"timeengine_test: invalid command\n");
      exit(1);
    }
  }

  //
  // Sanity Checks
  //
  if(time_ids.size()==0) {
    fprintf(stderr,"timeengine_test: nothing to do!\n");
    exit(1);
  }

  d_time_engine=new RDTimeEngine(this);
  connect(d_time_engine,SIGNAL(timeout(int)),this,SLOT(timeoutData(int)));
  for(QMultiMap<QTime,int>::const_iterator it=time_ids.begin();
      it!=time_ids.end();it++) {
    d_time_engine->addEvent(it.value(),it.key());
    printf("Added ID %d at %s\n",it.value(),
	   it.key().toString().toUtf8().constData());
  }

}


void MainObject::timeoutData(int id)
{
  QTime now=QTime::currentTime();

  printf("ID %d fired [scheduled: %s, actual: %s  difference: %d mS\n",id,
	 d_time_engine->event(id).toString("hh:mm:ss.zzz").toUtf8().constData(),
	 now.toString("hh:mm:ss.zzz").toUtf8().constData(),
	 d_time_engine->event(id).msecsTo(now));
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
