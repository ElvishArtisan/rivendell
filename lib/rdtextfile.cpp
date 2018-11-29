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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qfile.h>
#include <qmessagebox.h>

#include "rdapplication.h"
#include "rdconf.h"
#include "rd.h"
#include "rdtempdirectory.h"

#include <rdtextfile.h>

bool RDTextFile(const QString &data,bool delete_on_exit)
{
  char tmpfile[256];
  char editor[256];

  if(getenv("VISUAL")==NULL) {
    strncpy(editor,RD_LINUX_EDITOR,256);
  }
  else {
    strncpy(editor,getenv("VISUAL"),256);
  }
  strcpy(tmpfile,RDTempDirectory::basePath()+"/rdreportXXXXXX");
  int fd=mkstemp(tmpfile);
  if(fd<0) {
    QMessageBox::warning(NULL,"File Error","Unable to create temporary file");
    return false;
  }
  write(fd,data.utf8(),data.utf8().length());
  ::close(fd);
  if(delete_on_exit) {
    rda->addTempFile(tmpfile);
  }

  char *args[]={editor,tmpfile,(char *)NULL};
  if(fork()==0) {
    execvp(editor,args);
    exit(1);
  }
  return true;
}
