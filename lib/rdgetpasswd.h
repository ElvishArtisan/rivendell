// rdgetpasswd.h
//
// Prompt for a password.
//
//   (C) Copyright 2007 Fred Gleason <fredg@salemradiolabs.com>
//
//      $Id: rdgetpasswd.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef GETPASSWD_H
#define GETPASSWD_H

#include <qdialog.h>
#include <qlineedit.h>


class RDGetPasswd : public QDialog
{
  Q_OBJECT
  public:
   RDGetPasswd(QString *passwd,QWidget *parent=0,const char *name=0);
   ~RDGetPasswd();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QLineEdit *pw_password_edit;
   QString *pw_password;
};


#endif  // RDGETPASSWD_H

