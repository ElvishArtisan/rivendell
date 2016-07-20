// rdclilogedit.h
//
// A Command-line log editor for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCLILOGEDIT_H
#define RDCLILOGEDIT_H

#include <qobject.h>
#include <qsocketnotifier.h>

#include <rdconfig.h>
#include <rdcut.h>
#include <rddb.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdripc.h>
#include <rduser.h>

#define RDCLILOGEDIT_USAGE "[options]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();
  void inputActivatedData(int sock);

 private:
  void Help(const QStringList &cmds) const;
  void ListLogs() const;
  void Load(const QString &logname);
  void List();
  void Unload();
  void DispatchCommand(const QString &cmd);
  QString ListLine(RDLogEvent *evt,int line) const;
  void PrintPrompt() const;
  QSocketNotifier *edit_input_notifier; 
  QString edit_accum;
  RDLog *edit_log;
  RDLogEvent *edit_log_event;
  RDUser *edit_user;
  RDRipc *edit_ripc;
  RDConfig *edit_config;
};


#endif  // RDCLILOGEDIT_H
