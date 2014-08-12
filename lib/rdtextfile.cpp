// rdtextfile.cpp
//
// Spawn an external text file viewer.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtextfile.cpp,v 1.7 2010/07/29 19:32:34 cvs Exp $
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

#include <qmessagebox.h>
#include <qprocess.h>

#include <rdconf.h>
#include <rd.h>
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
  QString tempfile=QString().sprintf("%s\\rd-%s",(const char *)RDTempDir(),
	           (const char *)QTime::currentTime().toString("hhmmsszzz"));
  FILE *f=fopen(tempfile,"w");
  if(f==NULL) {
    QMessageBox::warning(NULL,"File Error","Unable to create temporary file");
    return false;
  }
  fprintf(f,"%s",(const char *)data);
  fclose(f);
  QStringList args;
  args+=editor;
  args+=tempfile;
  QProcess *proc=new QProcess(args);
  proc->launch("");
  delete proc;
#else
  strcpy(tmpfile,"/tmp/rdreportXXXXXX");
  int fd=mkstemp(tmpfile);
  if(fd<0) {
    QMessageBox::warning(NULL,"File Error","Unable to create temporary file");
    return false;
  }
  else {
    write(fd,data,data.length());
    ::close(fd);
    if(fork()==0) {
      system(QString().sprintf("%s %s",(const char *)editor,tmpfile));
      unlink(tmpfile);
      exit(0);
    }
  }
#endif  // WIN32
  return true;
}
