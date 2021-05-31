// rdprocess.h
//
// Process container
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

#ifndef RDPROCESS_H
#define RDPROCESS_H

#include <qobject.h>
#include <qprocess.h>

class RDProcess : public QObject
{
  Q_OBJECT;
 public:
  RDProcess(int id,QObject *parent=0);
  ~RDProcess();
  QProcess *process() const;
  QString program() const;
  QStringList arguments() const;
  QString prettyCommandString() const;
  void setProcessEnvironment(const QProcessEnvironment &env);
  void start(const QString &program,const QStringList &args);
  QString errorText() const;
  void *privateData() const;
  void setPrivateData(void *priv);
  QByteArray standardErrorData() const;

 signals:
  void started(int id);
  void finished(int id);

  private slots:
   void startedData();
   void finishedData(int exit_code,QProcess::ExitStatus status);
   void readyReadStandardErrorData();

 private:
   int p_id;
   QString p_program;
   QStringList p_arguments;
   QProcess *p_process;
   QString p_error_text;
   void *p_private_data;
   QByteArray p_standard_error_data;
};


#endif  // RDPROCESS_H
