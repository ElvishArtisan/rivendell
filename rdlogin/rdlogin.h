// rdlogin.h
//
// The User Login/Logout Utility for Rivendell.
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

#ifndef RDLOGIN_H
#define RDLOGIN_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdmainwindow.h>
#include <rduserlistmodel.h>
#include <rdwidget.h>

#define RDLOGIN_USAGE "\n"

class MainWidget : public RDMainWindow
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void closeEvent(QCloseEvent *e);
				 
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
  uint16_t login_ripc_hostport;
  QLabel *login_label;
  QLabel *login_username_label;
  QComboBox *login_username_box;
  RDUserListModel *login_username_model;
  QLineEdit *login_username_edit;
  QLabel *login_password_label;
  QLineEdit *login_password_edit;
  QPushButton *login_button;
  QPushButton *logout_button;
  QPushButton *cancel_button;
  int login_user_width;
  bool login_resize;
};


#endif  // RDLOGIN_H
