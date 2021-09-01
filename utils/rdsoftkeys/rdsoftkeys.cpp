// rdsoftkeys.cpp
//
// A utility for sending RML Commands
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

#include <netdb.h>
#include <arpa/inet.h>

#include <QApplication>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTranslator>

#include <rdprofile.h>
#include <rdpushbutton.h>

#include "rdsoftkeys.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  key_ysize=70;

  //
  // Read Command Options
  //
  RDConfig *config=new RDConfig();
  QString map_filename=config->filename();
  RDCmdSwitch *cmd=new RDCmdSwitch("rdsoftkeys",RDSOFTKEYS_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--map-file") {
      map_filename=cmd->value(i);
    }
  }
  delete cmd;

  setWindowTitle(QString("RDSoftKeys")+" v"+VERSION);

  //
  // Create And Set Icon
  //
  key_icon_map=new QPixmap(rivendell_22x22_xpm);
  setWindowIcon(*key_icon_map);

  //
  // RML Send Socket
  //
  key_socket=new QUdpSocket(this);

  //
  // Create Buttons
  //
  RDPushButton *button;
  QString rmlcmd;
  int n=0;
  QString color_name;
  QColor color;
  QString str1;
  QString str2;
  int h=0;
  int s=0;
  int v=0;

  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(buttonData(int)));
  RDProfile *profile=new RDProfile();
  profile->setSource(map_filename);
  key_columns=
    profile->intValue("SoftKeys","Columns",RDSOFTKEYS_DEFAULT_COLUMNS);
  int col=0;
  unsigned row=0;
  while(!(rmlcmd=profile->stringValue("SoftKeys",QString().
				   sprintf("Command%d",n+1),"")).isEmpty()) {
    for(int i=0;i<rmlcmd.length();i++) {
      if(rmlcmd.at(i)==':') {
	key_macros.push_back(rmlcmd.right(rmlcmd.length()-(i+1)));
	key_addrs.push_back(rmlcmd.left(i));
	button=new RDPushButton(this,config);
	button->setGeometry(10+90*col,10+60*row,80,50);
	button->setWordWrap(true);
	button->setText(
	     profile->stringValue("SoftKeys",QString::asprintf("Legend%d",n+1),
				  QString::asprintf("Button %d",n+1)));
	if(!(color_name=profile->stringValue("SoftKeys",
					QString::asprintf("Color%d",n+1),"")).
	   isEmpty()) {
	  color=QColor(color_name);
	  QPalette pal=QPalette(color,palette().color(QPalette::Background));
	  color.getHsv(&h,&s,&v);
	  if((h>180)&&(h<300)) {
	    v=255;
	  }
	  else {
	    if(v<168) {
	      v=255;
	    }
	    else {
	      v=0;
	    }
	  }
	  s=0;
	  color.setHsv(h,s,v);
	  pal.setColor(QPalette::Active,QPalette::ButtonText,color);
	  pal.setColor(QPalette::Inactive,QPalette::ButtonText,color);
	  button->setPalette(pal);
	}
	mapper->setMapping(button,n);
	connect(button,SIGNAL(clicked()),mapper,SLOT(map()));
	if(++col==key_columns) {
	  col=0;
	  row++;
	  key_ysize+=60;
	}
      }
    }
    n++;
  }

  //
  // If no softkeys have been defined, gracefully exit
  //
  if (!key_macros.size()) {
    QMessageBox::critical(this, tr("RDSoftKeys"),
		tr("No SoftKey definitions found in file")+"\n"+map_filename);
    exit(1);
  }

  if((key_macros.size()%key_columns)==0) {
    key_ysize-=60;
  }

  //
  // Set Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
}


QSize MainWidget::sizeHint() const
{
  int x=0;

  if(key_macros.size()>=key_columns) {
    x=10+90*key_columns;
  }
  else {
    x=10+90*key_macros.size();
  }
  return QSize(x,key_ysize);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::buttonData(int id)
{
  QHostAddress addr;
  struct hostent *hostent=gethostbyname(key_addrs[id].toUtf8().constData());
  if(hostent==NULL) {
    QMessageBox::warning(this,tr("RDSoftKeys"),hstrerror(h_errno));
    return;
  }
  if(hostent->h_addr_list!=NULL) {
    char str[INET_ADDRSTRLEN];
    inet_ntop(hostent->h_addrtype,*hostent->h_addr_list,str,sizeof(str));
    addr.setAddress(str);
  }
  else {
    addr.setAddress(key_addrs[id]);
  }
  key_socket->
    writeDatagram(key_macros[id].toUtf8(),addr,(uint16_t)RD_RML_NOECHO_PORT);
}

  
void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString tr_path;
  QString qt_path;

  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    tr_path=QString(PREFIX)+QString("/share/srlabs/");
    qt_path=QString("/usr/share/qt4/translation/");

    QTranslator qt(0);
    qt.load(qt_path+QString("qt_")+loc,".");
    a.installTranslator(&qt);

    QTranslator libradio(0);
    libradio.load(tr_path+QString("librd_")+loc,".");
    a.installTranslator(&libradio);

    QTranslator tests(0);
    tests.load(tr_path+QString("rdsoftkeys_")+loc,".");
    a.installTranslator(&tests);
  }

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  w->setGeometry(w->geometry().x(),w->geometry().y(),w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
