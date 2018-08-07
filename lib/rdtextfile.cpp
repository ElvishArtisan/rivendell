// rdtextfile.cpp
//
// Spawn an external text file viewer.
//
//   (C) Copyright 2002-2006,2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WIN32
#include <unistd.h>
#endif  // WIN32
#include <stdlib.h>
#include <stdio.h>

#include <qfile.h>
#include <qmessagebox.h>
#include <q3process.h>

#include "rdconf.h"
#include "rd.h"
#include "rdtempdirectory.h"

#include <rdtextfile.h>

bool RDTextFile(const QString &data)
{
  char tmpfile[256];
  QString editor;

  if(getenv("VISUAL")==NULL) {
#ifdef WIN32
    editor=RD_WIN32_EDITOR;
#else
    editor=RD_LINUX_EDITOR;
#endif  // WIN32
  }
  else {
    editor=getenv("VISUAL");
  }
#ifdef WIN32
  QString tempfile=RDTempDirectory::basePath()+"\\rd-"+
    QTime::currentTime().toString("hhmmsszzz");
  FILE *f=fopen(tempfile,"w");
  if(f==NULL) {
    QMessageBox::warning(NULL,"File Error","Unable to create temporary file");
    return false;
  }
  fprintf(f,"%s",(const char *)data.utf8());
  fclose(f);
  QStringList args;
  args+=editor;
  args+=tempfile;
  Q3Process *proc=new Q3Process(args);
  proc->launch("");
  delete proc;
#else
  strcpy(tmpfile,RDTempDirectory::basePath()+"/rdreportXXXXXX");
  int fd=mkstemp(tmpfile);
  if(fd<0) {
    QMessageBox::warning(NULL,"File Error","Unable to create temporary file");
    return false;
  }
  write(fd,data.utf8(),data.utf8().length());
  ::close(fd);
  if(fork()==0) {
    system(editor+" "+QString(tmpfile));
    unlink(tmpfile);
    exit(0);
  }
#endif  // WIN32
  return true;
}
