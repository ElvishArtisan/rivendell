// process.cpp
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

#include <qfile.h>

#include "process.h"

Process::Process(int id,QObject *parent)
  : QObject(parent)
{
  p_id=id;
  p_process=new QProcess(this);
  connect(p_process,SIGNAL(started()),this,SLOT(startedData()));
  connect(p_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(finishedData(int,QProcess::ExitStatus)));
}


Process::~Process()
{
  delete p_process;
}


QProcess *Process::process() const
{
  return p_process;
}


QString Process::program() const
{
  return p_program;
}


QStringList Process::arguments() const
{
  return p_arguments;
}


void Process::start(const QString &program,const QStringList &args)
{
  p_program=program;
  p_arguments=args;

  QFile file(p_program);
  if(!file.exists()) {
    p_error_text=tr("no such program")+" \""+p_program+"\"";
  }

  p_process->start(program,args);
}


QString Process::errorText() const
{
  return p_error_text;
}


void Process::startedData()
{
  emit started(p_id);
}


void Process::finishedData(int exit_code,QProcess::ExitStatus status)
{
  p_error_text=tr("ok");

  if(status==QProcess::CrashExit) {
    p_error_text=tr("process crashed");
  }
  else {
    if(exit_code!=0) {
      p_error_text=tr("process returned exit code")+
	QString().sprintf(" %d ",exit_code)+
	"["+p_process->readAllStandardError()+"]";
    }
  }

  emit finished(p_id);
}
