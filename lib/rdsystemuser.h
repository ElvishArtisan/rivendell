// rdsystemuser.h
//
// Abstracts a system (non-Rivendell) user.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsystemuser.h,v 1.2 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDSYSTEMUSER_H
#define RDSYSTEMUSER_H

#include <qstring.h>

class RDSystemUser
{
 public:
  RDSystemUser(const QString &username);
  QString username() const;
  bool exists() const;
  uid_t uid() const;
  gid_t gid() const;
  QString fullName() const;
  QString homeDirectory() const;
  QString shell() const;
  bool validatePassword(const QString &pwd);

 private:
  QString system_username;
  bool system_exists;
  uid_t system_uid;
  gid_t system_gid;
  QString system_full_name;
  QString system_home_directory;
  QString system_shell;
  QString system_password;
};


#endif  // RDSYSTEMUSER_H
