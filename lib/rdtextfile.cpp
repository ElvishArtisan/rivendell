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

#include <unistd.h>
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
    editor=RD_LINUX_EDITOR;
  }
  else {
    editor=getenv("VISUAL");
  }
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
  return true;
}
