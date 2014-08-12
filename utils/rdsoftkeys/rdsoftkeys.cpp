// rdsoftkeys.cpp
//
// A utility for sending RML Commands
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsoftkeys.cpp,v 1.7.8.2 2014/01/08 02:08:41 cvs Exp $
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
#ifndef WIN32
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <qtranslator.h>
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qtextcodec.h>
#include <qsignalmapper.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qsettings.h>

#include <rdprofile.h>
#include <rd.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>

#include <rdsoftkeys.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"


MainWidget::MainWidget(QWidget *parent,const char *name)
  : QMainWindow(parent,name)
{
  key_ysize=70;

  //
  // Read Command Options
  //
  RDConfig *config=new RDConfig();
  QString map_filename=config->filename();
  delete config;
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdsoftkeys",RDSOFTKEYS_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--map-file") {
      map_filename=cmd->value(i);
    }
  }
  delete cmd;

  //
  // Generate Font
  //
  QFont font("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont label_font("Helvetica",18,QFont::Bold);
  label_font.setPixelSize(18);

  setCaption(QString("RDSoftKeys")+" v"+VERSION);

  //
  // Create And Set Icon
  //
  key_icon_map=new QPixmap(rivendell_xpm);
  setIcon(*key_icon_map);

  //
  // RML Send Socket
  //
  key_socket=new QSocketDevice(QSocketDevice::Datagram);

  //
  // Create Buttons
  //
  QPushButton *button;
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
  unsigned col=0;
  unsigned row=0;
  while(!(rmlcmd=profile->stringValue("SoftKeys",QString().
				   sprintf("Command%d",n+1),"")).isEmpty()) {
    for(unsigned i=0;i<rmlcmd.length();i++) {
      if(rmlcmd.at(i)==':') {
	key_macros.push_back(rmlcmd.right(rmlcmd.length()-(i+1)));
	key_addrs.push_back(rmlcmd.left(i));
	button=new QPushButton(this);
	button->setGeometry(10+90*col,10+60*row,80,50);
	button->
	  setText(WrapText(button,profile->
			   stringValue("SoftKeys",QString().
				       sprintf("Legend%d",n+1),
				       QString().sprintf("Button %d",n+1))));
	if(!(color_name=profile->stringValue("SoftKeys",
					QString().sprintf("Color%d",n+1),"")).
	   isEmpty()) {
	  color=QColor(color_name);
	  QPalette pal=QPalette(color,backgroundColor());
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
	  pal.setColor(QPalette::Active,QColorGroup::ButtonText,color);
	  pal.setColor(QPalette::Inactive,QColorGroup::ButtonText,color);
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
  if((key_macros.size()%key_columns)==0) {
    key_ysize-=60;
  }

  //
  // Set Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
}


QSize MainWidget::sizeHint() const
{
  unsigned x=0;
  unsigned y=0;

  if(key_macros.size()>=key_columns) {
    x=10+90*key_columns;
    y=10+60*key_macros.size()/key_columns;
  }
  else {
    x=10+90*key_macros.size();
    y=70;
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
#ifndef WIN32 
  struct hostent *hostent=gethostbyname(key_addrs[id]);
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
#else 
  addr.setAddress(key_addrs[id]);
#endif  // WIN32
  key_socket->writeBlock(key_macros[id],key_macros[id].length(),
			 addr,(Q_UINT16)RD_RML_NOECHO_PORT);
}

  
void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


QString MainWidget::WrapText(QWidget *w,const QString &text)
{
  QFontMetrics fm(w->font());
  QString str;
  QString residue = text;
  bool space_found=false;
  int l;
  int lines=0;

  if(!text.isEmpty()) {	  
    while(!residue.isEmpty()) {
      space_found=false;
      for(int i=(int)residue.length();i>=0;i--) {
	      if((i==((int)residue.length()))||(residue.at(i).isSpace())) {
	  if(fm.boundingRect(residue.left(i)).width()<=w->width()-6) {
	    space_found=true;
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
      if(!space_found) {
	l=residue.length();
	for(int i=l;i>=0;i--) {
	  if(fm.boundingRect(residue.left(i)).width()<=(w->width()-6)) {
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
    }
  }
  return text;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  QApplication::setStyle(new QWindowsStyle);
  
  //
  // Load Translations
  //
  QString tr_path;
  QString qt_path;
#ifdef WIN32
  QSettings settings;
  settings.insertSearchPath(QSettings::Windows,"/SalemRadioLabs");
  tr_path=QString().sprintf("%s\\",
			    (const char *)settings.
			    readEntry("/Rivendell/InstallDir"));
  qt_path=tr_path;
#else
  tr_path=QString(PREFIX)+QString("/share/srlabs/");
  qt_path=QString(QTDIR)+QString("/translation/");
#endif  // WIN32
  QTranslator qt(0);
  qt.load(qt_path+QString("qt_")+QTextCodec::locale(),".");
  a.installTranslator(&qt);

  QTranslator libradio(0);
  libradio.load(tr_path+QString("libradio_")+QTextCodec::locale(),".");
  a.installTranslator(&libradio);

  QTranslator tests(0);
  tests.load(tr_path+QString("rdsoftkeys_")+QTextCodec::locale(),".");
  a.installTranslator(&tests);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(w->geometry().x(),w->geometry().y(),w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
