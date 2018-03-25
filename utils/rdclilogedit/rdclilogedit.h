// rdclilogedit.h
//
// A Command-line log editor for Rivendell
//
//   (C) Copyright 2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdcut.h>
#include <rddb.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdloglock.h>
#include <rdnotification.h>

#define RDCLILOGEDIT_USAGE "[options]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();

 private:
  void Addcart(int line,unsigned cartnum);
  void Addchain(int line,const QString &logname);
  void Addmarker(int line);
  void Addtrack(int line);
  void Deletelog(QString logname);
  void Header() const;
  void Help(const QStringList &cmds) const;
  void List();
  void ListLogs() const;
  void Listservices() const;
  void Load(QString logname);
  void New(const QString &logname);
  void Remove(int line);
  void Save();
  void Saveas(const QString &logname);
  void Setautorefresh(bool state);
  void Setcart(int line,unsigned cartnum);
  void Setcomment(int line,const QString &str);
  void Setdesc(const QString &str);
  void Setenddate(const QDate &date);
  void Setlabel(int line,const QString &str);
  void Setpurgedate(const QDate &date);
  void Setservice(const QString &str);
  void Setstartdate(const QDate &date);
  void Settime(int line,RDLogLine::TimeType type,const QTime &time=QTime());
  void Settrans(int line,RDLogLine::TransType type);
  void Unload();
  bool TryLock(RDLogLock *lock,const QString &logname);
  void OverwriteError(const QString &cmd) const;
  void DispatchCommand(QString cmd);
  QString ListLine(RDLogEvent *evt,int line) const;
  void PrintPrompt() const;
  void SendNotification(RDNotification::Action action,const QString &logname);
  bool edit_quiet_option;
  QString edit_accum;
  bool edit_modified;
  RDLog *edit_log;
  RDLogEvent *edit_log_event;
  QString edit_description;
  QString edit_service;
  QDate edit_start_date;
  QDate edit_end_date;
  QDate edit_purge_date;
  bool edit_auto_refresh;
  RDLogLock *edit_log_lock;
};


#endif  // RDCLILOGEDIT_H
