// rdapplication.cpp
//
// Base GUI Application Class
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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
			     const QString &usage,QObject *parent)
  : RDCoreApplication(module_name,cmdname,usage,parent)
{
  app_icon_engine=new RDIconEngine();
}


RDApplication::~RDApplication()
{
  delete app_icon_engine;
}


bool RDApplication::makeSingleInstance(QString *err_msg)
{
  //
  // If we're already running, then just raise the window.
  //
  QStringList args;
  args.clear();
  args.push_back("-l");
  QProcess *proc=new QProcess(this);
  proc->start("wmctrl",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    *err_msg=tr("wmctrl(1) process crashed");
    delete proc;
    return false;
  }
  if(proc->exitCode()!=0) {
    QString errs=QString::fromUtf8(proc->readAllStandardError());
    if(errs.isEmpty()) {
      *err_msg=tr("wmctrl(1) not found\n");
    }
    else {
      *err_msg=QString::asprintf("wmctrl(1) process returned error [%s]",
				 errs.toUtf8().constData());
    }
    delete proc;
    return false;
  }
  bool found=false;
  QStringList f0=QString::fromUtf8(proc->readAllStandardOutput()).
    split("\n",QString::SkipEmptyParts);
  for(int i=0;i<f0.size();i++) {
    QStringList f1=f0.at(i).split(" ",QString::SkipEmptyParts);
    if(f1.size()>=6) {
      if((f1.at(3).trimmed().toLower()==commandName())&&
	 (f1.at(4)==(QString("v")+VERSION))&&
	 (f1.at(5)=="-")) {
	Raise(f1.at(0));
	found=true;
      }
    }
  }
  delete proc;
  if(found) {
    exit(0);
  }
  return true;
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
