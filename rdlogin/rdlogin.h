// rdlogin.h
//
// The User Login/Logout Utility for Rivendell.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogin.h,v 1.14 2010/07/29 19:32:37 cvs Exp $
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


#ifndef RDLOGIN_H
#define RDLOGIN_H

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <rduser.h>
#include <rdstation.h>
#include <rdripc.h>
#include <rdconfig.h>


class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void connectedData(bool state);
  void userData();
  void loginData();
  void logoutData();
  void cancelData();
  void quitMainWidget();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDConfig *login_config;
  QSqlDatabase *login_db;
  RDStation *login_station;
  RDRipc *login_ripc;
  Q_UINT16 login_ripc_hostport;
  QLabel *login_label;
  QLabel *login_username_label;
  QComboBox *login_username_box;
  QLabel *login_password_label;
  QLineEdit *login_password_edit;
  QPixmap *login_rivendell_map;
  QPushButton *login_button;
  QPushButton *logout_button;
  QPushButton *cancel_button;
  int login_user_width;
};


#endif  // RDLOGIN_H
