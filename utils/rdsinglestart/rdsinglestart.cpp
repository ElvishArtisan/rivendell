// rdsinglestart.cpp
//
//  Start a program so as to allow only a single instance.
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <unistd.h>

#include <QApplication>
#include <QProcess>

#include "rdsinglestart.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QStringList args;
  QProcess *proc=NULL;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("rdsinglestart",RDSINGLESTART_USAGE);
  if(cmd->keys()<1) {
    perror("missing argument");
    exit(1);
  }
  for(unsigned i=0;i<(cmd->keys()-1);i++) {
  }
  QStringList f0=cmd->key(cmd->keys()-1).split("/",QString::SkipEmptyParts);
  QString program=f0.last().trimmed();

  //
  // If we're already running, then just raise the window.
  //
  args.clear();
  args.push_back("-l");
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
  bool found=false;
  f0=QString::fromUtf8(proc->readAllStandardOutput()).
    split("\n",QString::SkipEmptyParts);
  for(int i=0;i<f0.size();i++) {
    QStringList f1=f0.at(i).split(" ",QString::SkipEmptyParts);
    if(f1.size()>=4) {
      if(f1.at(3).trimmed().toLower()==program) {
	Raise(f1.at(0));
	found=true;
      }
    }
  }
  delete proc;
  if(found) {
    exit(0);
  }

  //
  // Otherwise, start a new process
  //
  Start(cmd);

  exit(0);
}


void MainObject::Raise(const QString win_id)
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


void MainObject::Start(RDCmdSwitch *cmd)
{
  char *args[cmd->keys()+1];
  memset(args,0,sizeof(char *)*(cmd->keys()+1));

  for(unsigned i=0;i<cmd->keys();i++) {
    args[i]=(char *)malloc(cmd->key(i).toUtf8().size()+1);
    strcpy(args[i],cmd->key(i).toUtf8().constData());
  }
  if(fork()==0) {
    execvp(args[0],args);
    perror("rdsinglestart");
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
