// rdpadd.cpp
//
// Rivendell PAD Consolidation Server
//
//   (C) Copyright 2018-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <rd.h>

#include "rdpadd.h"

MainObject::MainObject()
  : QObject()
{
  d_config=new RDConfig();
  d_config->load();

  int extended_next=2;
  if(d_config->extendedNextPadEvents()==0) {
    extended_next=1;
  }
  for(int i=0;i<extended_next;i++) {
    d_repeaters.push_back(new Repeater(QString::asprintf("%s-%d",
				       RD_PAD_SOURCE_UNIX_BASE_ADDRESS,i),
				       RD_PAD_CLIENT_TCP_PORT+i,this));
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
