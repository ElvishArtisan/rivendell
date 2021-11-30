// rdfiler.h
//
// Delegate for opening/opening/deleting files
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFILER_H
#define RDFILER_H

//#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QString>

#include <rdapplication.h>

class RDFiler
{
 public:
  RDFiler(uid_t uid,gid_t gid,const QString &root_dir);
  ~RDFiler();
  bool start();
  int open(const QString &pathname,int flags,
	   mode_t mode=S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) const;
  bool unlink(const QString &pathname) const;

 private:
  RDApplication::ExitCode ServiceLoop();
  uid_t d_uid;
  gid_t d_gid;
  QString d_root_directory;
  int d_unix_socket;
  pid_t d_service_pid;
};


#endif  // RDFILER_H
