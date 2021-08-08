// shutdown.cpp
//
// Shutdown routines for the Rivendell Services Manager
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdservice.h"

void MainObject::Shutdown()
{
  ShutdownDropboxes();

  //
  // Rivendell Daemons
  //
  for(int i=RDSERVICE_LAST_ID-1;i>=0;i--) {
    if(svc_processes[i]!=NULL) {
      svc_processes[i]->process()->terminate();
      if(!svc_processes[i]->process()->waitForFinished()) {
	svc_processes[i]->process()->kill();
      }
    }
    delete svc_processes[i];
    svc_processes.remove(i);
  }
}


void MainObject::ShutdownDropboxes()
{
  QList<int> kill_ids;

  for(QMap<int,RDProcess *>::iterator it=svc_processes.begin();
      it!=svc_processes.end();it++) {
    if(it.key()>=RDSERVICE_FIRST_DROPBOX_ID) {
      kill_ids.push_back(it.key());
    }
  }
  for(int i=0;i<kill_ids.size();i++) {
    svc_processes.value(kill_ids.at(i))->process()->kill();
    svc_processes.value(kill_ids.at(i))->process()->waitForFinished();
    delete svc_processes.value(kill_ids.at(i));
    svc_processes.remove(kill_ids.at(i));
  }
}
