// logobject.h
//
// The Log Manager Utility for Rivendell.
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LOGOBJECT_H
#define LOGOBJECT_H

#include <qobject.h>

#include <rdnotification.h>

class LogObject : public QObject
{
 Q_OBJECT
 public:
 LogObject(const QString &svcname,int start_offset,bool protect_existing,
	   bool gen_log,bool merge_mus,bool merge_tfc,QObject *parent=0);
  
 private slots:
  void userData();

 private:
  void SendNotification(RDNotification::Action action,const QString &logname);
  QString log_service_name;
  int log_start_offset;
  bool log_protect_existing;
  bool log_generate_log;
  bool log_merge_music;
  bool log_merge_traffic;
};


#endif  // LOG_OBJECT_H
