// add_user.h
//
// Add a Rivendell User
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_USER_H
#define ADD_USER_H

#include <q3listbox.h>
#include <q3textedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>

#include <rddialog.h>
#include <rduser.h>

class AddUser : public RDDialog
{
  Q_OBJECT
  public:
   AddUser(QString *username,QWidget *parent=0);
   ~AddUser();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QLineEdit *user_name_edit;
   QString *user_name;
};


#endif  // ADD_USER_H
