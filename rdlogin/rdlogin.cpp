// rdlogin.cpp
//
// The User Login/Logout Utility for Rivendell.
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

#include <stdlib.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtranslator.h>

#include <rdapplication.h>
#include <rddb.h>

#include "rdlogin.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  login_resize=false;
  login_user_width=160;

  QString str;
  QString sql;
  RDSqlQuery *q;
  QString err_msg;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

  //
  // Create And Set Icon
  //
  login_rivendell_map=new QPixmap(rivendell_22x22_xpm);
  setWindowIcon(*login_rivendell_map);

  rda=new RDApplication("RDLogin","rdlogin",RDLOGIN_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDLogin - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDLogin - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  setWindowTitle("RDLogin");

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),this,SLOT(connectedData(bool)));
  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,
			  rda->config()->password());

  //
  // User Label
  //
  login_label=new QLabel(this);
  login_label->setFont(sectionLabelFont());
  login_label->setAlignment(Qt::AlignCenter);
  login_label->setText(tr("Current User: unknown"));

  //
  // User Name
  //
  login_username_box=new QComboBox(this);
  login_username_box->setFocus();
  QFontMetrics fm(font());
  sql="select LOGIN_NAME from USERS where ADMIN_CONFIG_PRIV=\"N\"\
       order by LOGIN_NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    login_username_box->insertItem(q->value(0).toString());
    if(fm.width(q->value(0).toString())>login_user_width) {
      login_user_width=fm.width(q->value(0).toString());
    }
  }
  delete q;
  login_username_edit=new QLineEdit(this);
  if(login_user_width>900) {
    login_user_width=900;
  }
  login_username_label=new QLabel(login_username_box,tr("&Username:"),this);
  login_username_label->setFont(labelFont());
  login_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  if(rda->system()->showUserList()) {
    login_username_edit->hide();
  }
  else {
    login_username_box->hide();
  }

  //
  // Password
  //
  login_password_edit=new QLineEdit(this);
  login_password_edit->setMaxLength(RD_MAX_PASSWORD_LENGTH);
  login_password_edit->setEchoMode(QLineEdit::Password);
  login_password_label=new QLabel(login_password_edit,tr("&Password:"),this);
  login_password_label->setFont(labelFont());
  login_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  connect(login_password_edit,SIGNAL(returnPressed()),this,SLOT(loginData()));

  //
  // Login Button
  //
  login_button=new QPushButton(this);
  login_button->setFont(buttonFont());
  login_button->setText(tr("&Set User"));
  connect(login_button,SIGNAL(clicked()),this,SLOT(loginData()));

  //
  // Logout Button
  //
  logout_button=new QPushButton(this);
  logout_button->setFont(buttonFont());
  logout_button->setText(tr("&Default\nUser"));
  connect(logout_button,SIGNAL(clicked()),this,SLOT(logoutData()));

  //
  // Cancel Button
  //
  cancel_button=new QPushButton(this);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  login_resize=true;

  resizeEvent(NULL);
}


MainWidget::~MainWidget()
{
  delete login_label;
  delete login_username_box;
  delete login_password_edit;
}


QSize MainWidget::sizeHint() const
{
  return QSize(120+login_user_width,160);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::connectedData(bool state)
{
}


void MainWidget::userData()
{
  login_label->setText(tr("Current User")+": "+rda->ripc()->user());
  resizeEvent(NULL);
}


void MainWidget::loginData()
{
  RDUser *user;

  if(rda->system()->showUserList()) {
    user=new RDUser(login_username_box->currentText());
  }
  else {
    user=new RDUser(login_username_edit->text());
  }
  if(user->exists()&&user->checkPassword(login_password_edit->text(),false)) {
    if(rda->system()->showUserList()) {
      rda->ripc()->setUser(login_username_box->currentText());
    }
    else {
      rda->ripc()->setUser(login_username_edit->text());
    }
    login_password_edit->clear();
    delete user;
    qApp->processEvents();
    cancelData();
  }
  else {
    QMessageBox::warning(this,tr("RDLogin"),tr("Invalid Username or Password!"));
    delete user;
  }
}


void MainWidget::logoutData()
{
  QString default_name=rda->station()->defaultName();
  rda->ripc()->setUser(default_name);
  login_password_edit->clear();
  for(int i=0;i<login_username_box->count();i++) {
    if(login_username_box->text(i)==default_name) {
      login_username_box->setCurrentItem(i);
      qApp->processEvents();
      cancelData();
      return;
    }
  }
}


void MainWidget::cancelData()
{
  exit(0);
}


void MainWidget::quitMainWidget()
{
  qApp->quit();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(login_resize) {
    login_label->setGeometry(0,10,size().width(),21);
    login_username_box->setGeometry(110,40,size().width()-120,19);
    login_username_edit->setGeometry(110,40,size().width()-120,19);
    login_username_label->setGeometry(10,40,85,19);
    login_password_edit->setGeometry(110,61,size().width()-120,19);
    login_password_label->setGeometry(10,61,85,19);
    login_button->setGeometry(size().width()-270,size().height()-60,80,50);
    logout_button->setGeometry(size().width()-180,size().height()-60,80,50);
    cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
  }
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString("/usr/share/qt4/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdlogin_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
