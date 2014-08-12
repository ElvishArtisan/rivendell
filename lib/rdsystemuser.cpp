// rdsystemuser.cpp
//
// Abstracts a system (non-Rivendell) user.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsystemuser.cpp,v 1.2.8.1 2012/12/13 22:33:45 cvs Exp $
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

#include <syslog.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

#include <security/pam_appl.h>

#include <qstringlist.h>

#include <rdpam.h>
#include <rdsystemuser.h>

RDSystemUser::RDSystemUser(const QString &username)
{
  struct passwd *user=NULL;
  QStringList fields;

  system_username=username;
  system_exists=false;

  if((user=getpwnam(username))==NULL) {
    return;
  }
  system_uid=user->pw_uid;
  system_gid=user->pw_gid;
  system_full_name=user->pw_gecos;
  system_home_directory=user->pw_dir;
  system_shell=user->pw_shell;
}


QString RDSystemUser::username() const
{
  return system_username;
}


bool RDSystemUser::exists() const
{
  return system_exists;
}


uid_t RDSystemUser::uid() const
{
  return system_uid;
}


gid_t RDSystemUser::gid() const
{
  return system_gid;
}


QString RDSystemUser::fullName() const
{
  return system_full_name;
}


QString RDSystemUser::homeDirectory() const
{
  return system_home_directory;
}


QString RDSystemUser::shell() const
{
  return system_shell;
}


bool RDSystemUser::validatePassword(const QString &pwd)
{
  RDPam *pam=new RDPam("login");
  bool ret=pam->authenticate(system_username,pwd);
  delete pam;
  return ret;
}
