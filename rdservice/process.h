// process.h
//
// Process container for the Rivendell Services Manager
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

#ifndef PROCESS_H
#define PROCESS_H

#include <qobject.h>
#include <qprocess.h>

class Process : public QObject
{
  Q_OBJECT;
 public:
  Process(int id,QObject *parent=0);
  ~Process();
  QProcess *process() const;
  QString program() const;
  QStringList arguments() const;
  void start(const QString &program,const QStringList &args);
  QString errorText() const;

 signals:
  void started(int id);
  void finished(int id);

  private slots:
   void startedData();
   void finishedData(int exit_code,QProcess::ExitStatus status);

 private:
   int p_id;
   QString p_program;
   QStringList p_arguments;
   QProcess *p_process;
   QString p_error_text;
};


#endif  // PROCESS_H
