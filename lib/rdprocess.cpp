// rdprocess.cpp
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

#include "rdprocess.h"

RDProcess::RDProcess(int id,QObject *parent)
  : QObject(parent)
{
  p_id=id;
  p_process=new QProcess(this);
  p_private_data=NULL;
  connect(p_process,SIGNAL(started()),this,SLOT(startedData()));
  connect(p_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(finishedData(int,QProcess::ExitStatus)));
  connect(p_process,SIGNAL(readyReadStandardError()),
	  this,SLOT(readyReadStandardErrorData()));
}


RDProcess::~RDProcess()
{
  delete p_process;
}


QProcess *RDProcess::process() const
{
  return p_process;
}


QString RDProcess::program() const
{
  return p_program;
}


QStringList RDProcess::arguments() const
{
  return p_arguments;
}


QString RDProcess::prettyCommandString() const
{
  return (p_program.trimmed()+" "+p_arguments.join(" ")).trimmed();
}


void RDProcess::setProcessEnvironment(const QProcessEnvironment &env)
{
  p_process->setProcessEnvironment(env);
}


void RDProcess::start(const QString &program,const QStringList &args)
{
  p_program=program;
  p_arguments=args;

  QFile file(p_program);
  if(!file.exists()) {
    p_error_text=tr("no such program")+" \""+p_program+"\"";
  }

  p_process->start(program,args);
}


QString RDProcess::errorText() const
{
  return p_error_text;
}


void RDProcess::startedData()
{
  emit started(p_id);
}


void RDProcess::finishedData(int exit_code,QProcess::ExitStatus status)
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


void RDProcess::readyReadStandardErrorData()
{
  p_standard_error_data+=process()->readAllStandardError();
}


void *RDProcess::privateData() const
{
  return p_private_data;
}


void RDProcess::setPrivateData(void *priv)
{
  p_private_data=priv;
}


QByteArray RDProcess::standardErrorData() const
{
  return p_standard_error_data;
}
