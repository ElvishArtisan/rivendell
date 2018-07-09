// rdrunprocess.h
//
// Run an external process synchronously
//  (Roughly modeled after the synchronous API in Qt4's QProcess)
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

#ifndef RDRUNPROCESS_H
#define RDRUNPROCESS_H

#include <qobject.h>
#include <qprocess.h>
#include <qstringlist.h>

class RDRunProcess : public QObject
{
  Q_OBJECT;
 public:
  enum ExitStatus {NormalExit=0,CrashExit=1}; 
  RDRunProcess(QObject *parent=0);
  ~RDRunProcess();
  bool start(const QString &cmdstr);
  bool start(const QString &cmd,const QStringList &args);
  bool waitForStarted(int msecs=30000);
  bool waitForFinished(int msecs=30000);
  QByteArray readAllStandardError();
  QByteArray readAllStandardOutput();
  int exitCode() const;
  ExitStatus exitStatus() const;

 private slots:
  void launchFinishedData();
  void processExitedData();

 private:
  QProcess *run_process;
  bool run_started;
  bool run_exited;
};


#endif   // RDRUNPROCESS_H
