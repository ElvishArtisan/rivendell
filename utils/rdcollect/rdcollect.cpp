// rdcollect.cpp
//
// Collect and combine log exports into a single file.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcollect.cpp,v 1.2.8.1 2012/08/01 19:21:09 cvs Exp $
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
#include <errno.h>

#include <qapplication.h>
#include <qdir.h>

#include <rdcmd_switch.h>
#include <rdconf.h>

#include <rdcollect.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool ok;
  int err;
  std::vector<unsigned> line_index;
  hours_offset=0;
  minutes_offset=3;
  seconds_offset=6;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcollect",RDCOLLECT_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--source-file") {
      source_files.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-file") {
      destination_file=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--hours-offset") {
      hours_offset=cmd->value(i).toUInt(&ok);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--minutes-offset") {
      minutes_offset=cmd->value(i).toUInt(&ok);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--seconds-offset") {
      seconds_offset=cmd->value(i).toUInt(&ok);
      cmd->setProcessed(i,true);
    }
  }
  if(!cmd->allProcessed()) {
    fprintf(stderr,"rdcollect: unknown option\n");
    exit(256);
  }
  if(source_files.size()==0) {
    fprintf(stderr,"rdcollect: missing --source-file argument\n");
    exit(256);
  }
  if(destination_file.isEmpty()) {
    fprintf(stderr,"rdcollect: missing --destination-file argument\n");
    exit(256);
  }

  //
  // Process Data
  //
  QStringList src_lines;
  for(unsigned i=0;i<source_files.size();i++) {
    QStringList src_dirs=GetDirectoryList(source_files[i]);
    LoadSourceFiles(RDGetBasePart(source_files[i]),src_dirs,&src_lines);
  }
  SortLines(&src_lines,&line_index);
  if((err=WriteOutputFile(destination_file,src_lines,&line_index))!=0) {
    fprintf(stderr,"rdollect: %s\n",strerror(err));
    exit(256);
  }
  exit(0);
}


QStringList MainObject::GetDirectoryList(const QString &src_file)
{
  QStringList ret;
  QString base_dir=RDGetPathPart(src_file);
  base_dir=base_dir.left(base_dir.length()-1);
  QDir dir(base_dir);
  dir.setFilter(QDir::Dirs);
  ret.push_back(dir.path());
  AddDirs(base_dir,&ret);
  return ret;
}


void MainObject::LoadSourceFiles(const QString &src_name,
				 const QStringList &dirs,QStringList *lines)
{
  for(unsigned i=0;i<dirs.size();i++) {
    LoadSourceFile(dirs[i]+"/"+src_name,lines);
  }
}


void MainObject::LoadSourceFile(const QString &filename,QStringList *lines)
{
  Q_LONG n;
  QString line;
  QFile file(filename);
  if(!file.open(IO_ReadOnly|IO_Translate)) {
    return;
  }
  while((n=file.readLine(line,1024))>0) {
    lines->push_back(line.left(line.length()-1));
  }
  file.close();
}


void MainObject::SortLines(QStringList *lines,std::vector<unsigned> *index)
{
  std::vector<QTime> start_times;

  //
  // Initialize Index
  //
  for(unsigned i=0;i<lines->size();i++) {
    index->push_back(i);
    start_times.push_back(ReadTime((*lines)[i]));
  }

  //
  // Sort
  //
  bool modified=true;
  while(modified) {
    modified=false;
    for(unsigned i=1;i<lines->size();i++) {
      if(start_times[i-1]>start_times[i]) {
	QTime time=start_times[i-1];
	start_times[i-1]=start_times[i];
	start_times[i]=time;
	unsigned line=index->at(i-1);
	index->at(i-1)=index->at(i);
	index->at(i)=line;
	modified=true;
      }
    }
  }
}


int MainObject::WriteOutputFile(const QString &filename,
				const QStringList &lines,
				std::vector<unsigned> *index)
{
  FILE *f=NULL;
  if((f=fopen(filename,"w"))==NULL) {
    return errno;
  }
  for(unsigned i=0;i<lines.size();i++) {
    fprintf(f,"%s\n",(const char *)lines[index->at(i)]);
  }
  fclose(f);
  return 0;
}


void MainObject::AddDirs(const QString &path,QStringList *dirs)
{
  QDir dir(path);
  dir.setFilter(QDir::Dirs);
  QStringList list=dir.entryList();
  for(unsigned i=0;i<list.size();i++) {
    if((list[i]!=".")&&(list[i]!="..")) {
      dirs->push_back(path+"/"+list[i]);
      AddDirs(path+"/"+list[i],dirs);
    }
  }
}


QTime MainObject::ReadTime(const QString &line)
{
  return QTime(line.mid(hours_offset,2).toInt(),
	       line.mid(minutes_offset,2).toInt(),
	       line.mid(seconds_offset,2).toInt());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
