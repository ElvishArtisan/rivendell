// local_notifications.cpp
//
// Process local notifications for ripcd(8)
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>

#include <rdapplication.h>
#include <rdconf.h>

#include "ripcd.h"

void MainObject::RunLocalNotifications(RDNotification *notify)
{
  if((notify->type()==RDNotification::DropboxType)&&
     (notify->id().toString()==rda->config()->stationName())) {
    pid_t pid=RDGetPid(QString(RD_PID_DIR)+"/rdservice.pid");
    if(pid>0) {
      kill(pid,SIGUSR1);
    }
  }
}

