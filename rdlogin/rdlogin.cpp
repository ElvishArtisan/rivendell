// rdlogin.cpp
//
// The User Login/Logout Utility for Rivendell.
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogin.cpp,v 1.35.4.2 2014/01/21 21:59:32 cvs Exp $
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
#include <qwindowsstyle.h>
#include <qpainter.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <rd.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdcheck_daemons.h>
#include <rdcmd_switch.h>
#include <rdtextvalidator.h>
#include <rddbheartbeat.h>
#include <rddb.h>
#include <dbversion.h>

#include <rdlogin.h>


//
// Icons
//
#include "../icons/rivendell-22x22.xpm"


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  login_user_width=160;

  QString str;
  QString sql;
  RDSqlQuery *q;
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdlogin","\n");
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }
  delete cmd;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",16,QFont::Bold);
  label_font.setPixelSize(12);
  QFont small_label_font=QFont("Helvetica",12,QFont::Bold);
  small_label_font.setPixelSize(12);
  QFont line_edit_font=QFont("Helvetica",12,QFont::Normal);
  line_edit_font.setPixelSize(12);

  //
  // Create And Set Icon
  //
  login_rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*login_rivendell_map);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Ensure that the system daemons are running
  //
  RDInitializeDaemons();

  //
  // Load Configs
  //
  login_config=new RDConfig();
  login_config->load();

  str=QString(tr("RDLogin - Station:"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)login_config->stationName()));

  //
  // Open Database
  //
  QString err(tr("rdlogin : "));
  login_db = RDInitDb(&schema,&err);
  if(!login_db) {
    QMessageBox::warning(this,tr("Can't Connect"),err);
    exit(0);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,"rdlogin: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }

  //
  // RIPC Connection
  //
  login_ripc=new RDRipc(login_config->stationName());
  connect(login_ripc,SIGNAL(connected(bool)),this,SLOT(connectedData(bool)));
  connect(login_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  login_ripc->connectHost("localhost",RIPCD_TCP_PORT,
			  login_config->password());

  //
  // Station
  //
  login_station=new RDStation(login_config->stationName());

  //
  // User Label
  //
  login_label=new QLabel(this,"login_label");
  login_label->setFont(label_font);
  login_label->setAlignment(AlignCenter);
  login_label->setText(tr("Current User: unknown"));

  //
  // User Name
  //
  login_username_box=new QComboBox(this,"login_username_box");
  login_username_box->setFont(line_edit_font);
  login_username_box->setFocus();
  QFontMetrics fm(line_edit_font);
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
  if(login_user_width>900) {
    login_user_width=900;
  }
  login_username_label=new QLabel(login_username_box,tr("&Username:"),
				  this,"login_username_label");
  login_username_label->setFont(small_label_font);
  login_username_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Password
  //
  login_password_edit=new QLineEdit(this,"login_password_edit");
  login_password_edit->setFont(line_edit_font);
  login_password_edit->setMaxLength(16);
  login_password_edit->setValidator(validator);
  login_password_edit->setEchoMode(QLineEdit::Password);
  login_password_label=new QLabel(login_password_edit,tr("&Password:"),
				  this,"login_password_label");
  login_password_label->setFont(small_label_font);
  login_password_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(login_password_edit,SIGNAL(returnPressed()),this,SLOT(loginData()));

  //
  // Login Button
  //
  login_button=new QPushButton(this,"login_button");
  login_button->setFont(button_font);
  login_button->setText(tr("&Set User"));
  connect(login_button,SIGNAL(clicked()),this,SLOT(loginData()));

  //
  // Logout Button
  //
  logout_button=new QPushButton(this,"logout_button");
  logout_button->setFont(button_font);
  logout_button->setText(tr("&Default\nUser"));
  connect(logout_button,SIGNAL(clicked()),this,SLOT(logoutData()));

  //
  // Cancel Button
  //
  cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  resizeEvent(NULL);
}


MainWidget::~MainWidget()
{
  delete login_db;
  delete login_ripc;
  delete login_station;
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
  QString str;

  str=QString(tr("Current User:"));
  login_label->setText(QString().sprintf("%s %s",(const char *)str,
					 (const char *)login_ripc->user()));
  resizeEvent(NULL);
}


void MainWidget::loginData()
{
  RDUser *user=new RDUser(login_username_box->currentText());
  if(user->checkPassword(login_password_edit->text(),false)) {
    login_ripc->setUser(login_username_box->currentText());
    login_password_edit->clear();
    delete user;
    qApp->processEvents();
    cancelData();
  }
  else {
    QMessageBox::warning(this,tr("RDLogin"),tr("Invalid Password!"));
    delete user;
  }
}


void MainWidget::logoutData()
{
  QString default_name=login_station->defaultName();
  login_ripc->setUser(default_name);
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
  login_db->removeDatabase(login_config->mysqlDbname());
  qApp->quit();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  login_label->setGeometry(0,10,size().width(),21);
  login_username_box->setGeometry(110,40,size().width()-120,19);
  login_username_label->setGeometry(10,40,85,19);
  login_password_edit->setGeometry(110,61,size().width()-120,19);
  login_password_label->setGeometry(10,61,85,19);
  login_button->setGeometry(size().width()-270,size().height()-60,80,50);
  logout_button->setGeometry(size().width()-180,size().height()-60,80,50);
  cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
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
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
