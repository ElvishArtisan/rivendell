// rdpasswd.h
//
// Set Password Widget for Rivendell.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPASSWD_H
#define RDPASSWD_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "rddialog.h"

class RDPasswd : public RDDialog
{
  Q_OBJECT;
 public:
  RDPasswd(const QString &caption,QWidget *parent=0);
  //  RDPasswd(QString *password,QWidget *parent=0);
  ~RDPasswd();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *passwd);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *passwd_password_1_label;
  QLineEdit *passwd_password_1_edit;
  QLabel *passwd_password_2_label;
  QLineEdit *passwd_password_2_edit;
  QString *passwd_password;
  QPushButton *passwd_ok_button;
  QPushButton *passwd_cancel_button;
};


#endif  // RDPASSWD_H
