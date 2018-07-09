// rdrunprocess.cpp
//
// Run an external process synchronously
//  (Loosely modeled after the synchronous API in Qt4's QProcess)
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

#include <unistd.h>

#include <qapplication.h>
#include <qfileinfo.h>

#include "rdrunprocess.h"

RDRunProcess::RDRunProcess(QObject *parent)
  : QObject(parent)
{
  run_started=false;
  run_exited=false;
  run_process=new QProcess(this);
  connect(run_process,SIGNAL(processExited()),this,SLOT(processExitedData()));
}


RDRunProcess::~RDRunProcess()
{
  delete run_process;
}


bool RDRunProcess::start(const QString &cmdstr)
{
  QStringList cmds=cmds.split(" ",cmdstr);
  QString cmd=cmds[0];
  QStringList args;
  for(unsigned i=1;i<cmds.size();i++) {
    args.push_back(cmds[i]);
  }
  return start(cmd,args);
}


bool RDRunProcess::start(const QString &cmd,const QStringList &args)
{
  QStringList cmdstr=args;
  QFileInfo fi(cmd);

  if((!fi.isFile())||(!fi.isExecutable())) {
    return false;
  }
  cmdstr.push_front(cmd);
  run_process->setArguments(cmdstr);
  run_process->start();

  return true;
}


bool RDRunProcess::waitForStarted(int msecs)
{
  while((!run_started)&&(msecs>0)) {
    usleep(10000);
    msecs-=10;
    qApp->processEvents();
  }
  return msecs>0;
}


bool RDRunProcess::waitForFinished(int msecs)
{
  while((!run_exited)&&(msecs>0)) {
    usleep(10000);
    msecs-=10;
    qApp->processEvents();
  }
  return msecs>0;
}


QByteArray RDRunProcess::readAllStandardError()
{
  return run_process->readStderr();
}


QByteArray RDRunProcess::readAllStandardOutput()
{
  return run_process->readStdout();
}


int RDRunProcess::exitCode() const
{
  return run_process->exitStatus();
}


RDRunProcess::ExitStatus RDRunProcess::exitStatus() const
{
  if(run_process->normalExit()) {
    return RDRunProcess::NormalExit;
  }
  return RDRunProcess::CrashExit;
}


void RDRunProcess::launchFinishedData()
{
  run_started=true;
}


void RDRunProcess::processExitedData()
{
  run_exited=true;
}
