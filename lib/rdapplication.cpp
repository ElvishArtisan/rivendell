// rdapplication.cpp
//
// Base GUI Application Class
//
//   (C) Copyright 2021-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QProcess>

#include "rdapplication.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(const QString &module_name,const QString &cmdname,
			     const QString &usage,bool use_translations,
			     QObject *parent)
  : RDCoreApplication(module_name,cmdname,usage,use_translations,parent)
{
  app_icon_engine=new RDIconEngine();
}


RDApplication::~RDApplication()
{
  delete app_icon_engine;
}


RDIconEngine *RDApplication::iconEngine() const
{
  return app_icon_engine;
}


QString RDApplication::locale()
{
  QString ret;

  if(getenv("LANG")!=NULL) {
    ret=getenv("LANG");
  }
  return ret;
}


void RDApplication::Raise(const QString win_id)
{
  QStringList args;
  QProcess *proc=NULL;

  args.push_back("-i");
  args.push_back("-R");
  args.push_back(win_id);
  proc=new QProcess(this);
  proc->start("wmctrl",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    perror("wmctrl(1) process crashed");
    exit(1);
  }
  if(proc->exitCode()!=0) {
    QString errs=QString::fromUtf8(proc->readAllStandardError());
    if(errs.isEmpty()) {
      fprintf(stderr,"rdsinglestart: wmctrl(1) not found\n");
    }
    else {
      fprintf(stderr,"rdsinglestart: wmctrl(1) process returned error [%s]\n",
	      errs.toUtf8().constData());
    }
    exit(1);
  }
  delete proc;
}
