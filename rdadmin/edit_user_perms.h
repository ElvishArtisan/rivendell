// edit_user_perms.h
//
// Edit Rivendell Service Permissions
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_USER_PERMS_H
#define EDIT_USER_PERMS_H

#include <QDialog>

#include <rdlistselector.h>
#include <rduser.h>

class EditUserPerms : public QDialog
{
  Q_OBJECT
 public:
  EditUserPerms(RDUser *user,QWidget *parent=0);
  ~EditUserPerms();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 private:
  RDListSelector *user_host_sel;
  RDUser *user_user;
};


#endif

