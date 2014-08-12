//   rdinstancelock.cpp
//
//   An abstract instance-locking class.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdinstancelock.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <qstring.h>
#include <qdir.h>

#include <rdinstancelock.h>


RDInstanceLock::RDInstanceLock(QString path)
{
  lock_path=path;
  lock_locked=false;
}


RDInstanceLock::~RDInstanceLock()
{
  unlock();
}


bool RDInstanceLock::lock()
{
  FILE *file;
  pid_t pid;
  QDir dir;

  if(MakeLock()) {
    lock_locked=true;
    return true;
  }
  if((file=fopen((const char *)lock_path,"r"))==NULL) {
    lock_locked=false;
    return false;
  }
  fscanf(file,"%d",&pid);
  fclose(file);
  dir.setPath(QString().sprintf("/proc/%u",pid));
  if(!dir.exists()) {
    unlink((const char *)lock_path);
    if(MakeLock()) {
      lock_locked=true;
      return true;
    }
  }
  lock_locked=false;
  return false;
}


void RDInstanceLock::unlock()
{
  if(lock_locked) {
    unlink((const char *)lock_path);
  }
  lock_locked=false;
}


bool RDInstanceLock::locked()
{
  return lock_locked;
}


bool RDInstanceLock::MakeLock()
{
  FILE *file;
  int fd=open((const char *)lock_path,O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR);
  if(fd<0) {
    return false;
  }
  file=fdopen(fd,"w");
  fprintf(file,"%u",getpid());
  fclose(file);
  return true;
}
