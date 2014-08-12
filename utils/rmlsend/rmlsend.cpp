// rmlsend.cpp
//
// A utility for sending RML Commands
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rmlsend.cpp,v 1.7.8.4 2014/01/21 21:59:34 cvs Exp $
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

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qfont.h>
#include <qsocketdevice.h>
#include <qhostaddress.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qtextstream.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>

#include <rdcmd_switch.h>

#ifndef WIN32
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#endif  // WIN32
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <rmlsend.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

//
// Globals
//
RDCmdSwitch *rdcmdswitch=NULL;


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QMainWindow(parent,name)
{
  //
  // Set Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont font("Helvetica",10,QFont::Normal);
  font.setPixelSize(10);
  setFont(font);
  QFont main_font("Helvetica",12,QFont::Bold);
  main_font.setPixelSize(12);
  QFont label_font("Helvetica",10,QFont::Bold);
  label_font.setPixelSize(10);

  //
  // Create And Set Icon
  //
  rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*rivendell_map);

  setCaption(QString("RMLSend")+" v"+VERSION+" - "+tr("Macro Command Utility"));

  host=new QLineEdit(this,"host");
  host->setGeometry(80,10,180,25);
  host->setFont(main_font);
  QLabel *label=new QLabel(host,"Sent To:",this,"host_label");
  label->setGeometry(10,16,65,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight);
  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(rdcmdswitch->key(i)=="--to-host") {
      rdcmdswitch->setProcessed(i,true);
      host->setText(rdcmdswitch->value(i));
    }
  }

  port_box=new QComboBox(this,"port_box");
  port_box->setGeometry(305,10,130,25);
  port_box->setEditable(false);
  label=new QLabel(port_box,"Dest:",this,"port_label");
  label->setGeometry(270,16,30,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight);
  port_box->insertItem(tr("RML"));
  port_box->insertItem(tr("RML (no echo)"));
  port_box->insertItem(tr("Set Port"));
  port_box->setCurrentItem(1);
  connect(port_box,SIGNAL(activated(int)),this,SLOT(destChangedData(int)));

  port_edit=new QLineEdit(this,"port_edit");
  port_edit->setGeometry(sizeHint().width()-60,10,50,25);
  port_edit->setFont(main_font);
  port_edit->setDisabled(true);
  port_edit_label=new QLabel(port_edit,tr("UDP Port:"),this,"port_edit_label");
  port_edit_label->setGeometry(sizeHint().width()-130,16,65,14);
  port_edit_label->setFont(label_font);
  port_edit_label->setAlignment(AlignRight);
  port_edit_label->setDisabled(true);

  command=new QLineEdit(this,"command");
  command->setGeometry(80,40,sizeHint().width()-90,25);
  command->setFont(main_font);
  label=new QLabel(command,tr("Command:"),this,"host_label");
  label->setGeometry(10,46,65,14);
  label->setFont(label_font);
  label->setAlignment(AlignRight);

  response=new QLineEdit(this,"response");
  response->setGeometry(80,70,sizeHint().width()-90,25);
  response->setFont(main_font);
  response_label=new QLabel(response,tr("Response:"),this,"response_label");
  response_label->setGeometry(10,76,65,14);
  response_label->setFont(label_font);
  response_label->setAlignment(AlignRight);

  send=new QPushButton(tr("&Send Command"),this,"send");
  send->setGeometry(10,sizeHint().height()-50,120,40);
  send->setFont(main_font);
  connect(send,SIGNAL(clicked()),this,SLOT(sendCommand()));

  quit=new QPushButton("&Quit",this,"quit");
  quit->setGeometry(sizeHint().width()-80,sizeHint().height()-50,70,40);
  quit->setFont(main_font);
  quit->setDefault(true);
  connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));

  udp_command=new QSocketDevice(QSocketDevice::Datagram);

  udp_response=new QSocketDevice(QSocketDevice::Datagram);
  udp_response->bind(QHostAddress(),RD_RML_REPLY_PORT);
  udp_response->setBlocking(false);

  timer=new QTimer(this,"timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(readResponse()));
  countdown=-1;
  timer->start(100);

  //
  // Populate Data
  //
  bool ok;
  unsigned port;
  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(rdcmdswitch->key(i)=="--to-port") {
      rdcmdswitch->setProcessed(i,true);
      port=rdcmdswitch->value(i).toUInt(&ok);
      if(ok&&(port<0xFFFF)) {
	switch(rdcmdswitch->value(i).toUInt()) {
	    case RD_RML_ECHO_PORT:
	      port_box->setCurrentItem(0);
	      break;
	      
	    case RD_RML_NOECHO_PORT:
	      port_box->setCurrentItem(1);
	      break;
	      
	    default:
	      port_box->setCurrentItem(2);
	      port_edit->setText(rdcmdswitch->value(i));
	      break;
	}
      }
    }
  }
  destChangedData(port_box->currentItem());
}


QSize MainWidget::sizeHint() const
{
  return QSize(570,150);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::sendCommand()
{
  QString dcl_command;
  unsigned port=0;
  bool ok=true;

  switch((MainWidget::DestMode)port_box->currentItem()) {
      case MainWidget::Rml:
	port=RD_RML_ECHO_PORT;
	break;

      case MainWidget::RmlNoEcho:
	port=RD_RML_NOECHO_PORT;
	break;

      case MainWidget::Manual:
	port=port_edit->text().toInt(&ok);
	if((!ok)||(port>0xFFFF)) {
	  QMessageBox::information(this,tr("RMLSend"),
				   tr("Invalid Port Number!"));
	  return;
	}
	break;
  }
  response->setText("");
#ifndef WIN32 
  struct hostent *hostent=gethostbyname(host->text());
  if(hostent==NULL) {
    QMessageBox::warning(this,tr("RMLSend"),hstrerror(h_errno));
    return;
  }
  if(hostent->h_addr_list!=NULL) {
    char str[INET_ADDRSTRLEN];
    inet_ntop(hostent->h_addrtype,*hostent->h_addr_list,str,sizeof(str));
    host_addr.setAddress(str);
  }
  else {
    host_addr.setAddress(host->text());
  }
#else 
  host_addr.setAddress(host->text());
#endif  // WIN32
  dcl_command=command->text();
  if(!udp_command->writeBlock(dcl_command,dcl_command.length(),
			      host_addr,(Q_UINT16)port)) {
    QMessageBox::warning(this,tr("RMLSend"),tr("Connection Failed!"));
    return;
  }
  else {
    countdown=5;
  }
}


void MainWidget::readResponse()
{
  char buffer[RD_RML_MAX_LENGTH];
  QString output;
  int n;
  QHostAddress peer_addr;

  n=udp_response->readBlock(buffer,RD_RML_MAX_LENGTH);
  if(n<=0) {
    if(countdown==0) {
      response->setText(tr("no response"));
      countdown=-1;
    }
    if(countdown!=-1) {
      countdown--;
    }
    return;
  }
  countdown=-1;
  buffer[n]=0;
  response->setText(buffer);
}


void MainWidget::destChangedData(int id)
{
  switch((MainWidget::DestMode)id) {
      case MainWidget::Rml:
	port_edit->setDisabled(true);
	port_edit_label->setDisabled(true);
	response->setEnabled(true);
	response_label->setEnabled(true);
	timer->start(100);
	break;

      case MainWidget::RmlNoEcho:
	port_edit->setDisabled(true);
	port_edit_label->setDisabled(true);
	response->setText("");
	response->setDisabled(true);
	response_label->setDisabled(true);
	timer->stop();
	break;

      case MainWidget::Manual:
	port_edit->setEnabled(true);
	port_edit_label->setEnabled(true);
	response->setText("");
	response->setDisabled(true);
	response_label->setDisabled(true);
	timer->stop();
	break;
  }
}

#ifndef WIN32
MainObject::MainObject(QObject *parent,const char *name)
{
  input_fd=-1;
  dest_hostname=RMLSEND_DEFAULT_ADDR;
  dest_port=RMLSEND_DEFAULT_PORT;
  rml_ptr=0;

  ReadSwitches();
  ResolveName();
  InitStream();
  ProcessCommands();
  CloseStream();

  exit(0);
}


MainObject::~MainObject()
{
}


void MainObject::ReadSwitches()
{
  bool ok;

  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(rdcmdswitch->key(i)=="--to-host") {
      rdcmdswitch->setProcessed(i,true);
      dest_hostname=rdcmdswitch->value(i);
    }
    if(rdcmdswitch->key(i)=="--to-port") {
      rdcmdswitch->setProcessed(i,true);
      dest_port=rdcmdswitch->value(i).toUInt(&ok);
      if((!ok)||(dest_port>0xFFFF)) {
	fprintf(stderr,"rmlsend: invalid destination port\n");
	exit(256);
      }
    }
    if(rdcmdswitch->key(i)=="--from-file") {
      rdcmdswitch->setProcessed(i,true);
      input_file=rdcmdswitch->value(i);
    }
  }
  if(input_file.isNull()) {
    if(rdcmdswitch->processed(rdcmdswitch->keys()-1)) {
      fprintf(stderr,"rmlsend: missing command argument\n");
      exit(256);
    }
    rdcmdswitch->setProcessed(rdcmdswitch->keys()-1,true);
    rml_cmd=rdcmdswitch->key(rdcmdswitch->keys()-1);
  }

  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(!rdcmdswitch->processed(i)) {
      printf("\nrmlsend %s\n",RMLSEND_USAGE);
      exit(256);
    }
  }
}


void MainObject::ResolveName()
{
  struct hostent *hostent=gethostbyname(dest_hostname);
  if(hostent==NULL) {
    fprintf(stderr,"rmlsend: %s\n",hstrerror(h_errno));
    exit(256);
  }
  dest_addr=new QHostAddress();
  if(hostent->h_addr_list!=NULL) {
    char str[INET_ADDRSTRLEN];
    inet_ntop(hostent->h_addrtype,*hostent->h_addr_list,str,sizeof(str));
    dest_addr->setAddress(str);
  }
  else {
    dest_addr->setAddress(dest_hostname);
  }
}


void MainObject::InitStream()
{
  if(!rml_cmd.isNull()) {
    return;
  }
  if(input_file=="-") {
    input_fd=0;
  }
  else {
    if((input_fd=open(input_file,O_RDONLY))<0) {
      perror("rmlsend");
      exit(256);
    }
  }
}


void MainObject::CloseStream()
{
  if(input_fd>0) {
    close(input_fd);
  }
}


bool MainObject::GetNextChar(char *c)
{

  if(rml_cmd.isNull()) {
    if(read(input_fd,c,1)<=0) {
      return true;
    }
    return false;
  }
  if(rml_ptr>=rml_cmd.length()) {
    return true;
  }
  *c=rml_cmd.at(rml_ptr++).latin1();
  return false;
}


void MainObject::ProcessCommands()
{
  char c;
  QSocketDevice *udp_command=new QSocketDevice(QSocketDevice::Datagram);
  char rml[RD_RML_MAX_LENGTH];
  unsigned ptr=0;
  bool active=false;

  while(!GetNextChar(&c)) {
    if(active) {
      if(c=='!') {
	rml[ptr++]=c;
	rml[ptr]=0;
	udp_command->writeBlock(rml,ptr,*dest_addr,dest_port);
	ptr=0;
	active=false;
      }
      else {
	rml[ptr++]=c;
      }
      if(ptr==RD_RML_MAX_LENGTH) {
	fprintf(stderr,"rmlsend: rml command too long\n");
	CloseStream();
	exit(256);
      }
    }
    else {
      if(isalpha(c)) {
	rml[ptr++]=c;
	active=true;
      }
    }
  }
}
#endif  // WIN32

int main(int argc,char *argv[])
{
  bool cli_mode=false;

  rdcmdswitch=new RDCmdSwitch(argc,argv,"rmlsend",RMLSEND_USAGE);
  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(rdcmdswitch->key(i)=="--from-file") {
      cli_mode=true;
    }
  }
  if(rdcmdswitch->keys()>0) {
    if(rdcmdswitch->key(rdcmdswitch->keys()-1).left(2)!=QString("--")) {
      cli_mode=true;
    }
  }
#ifdef WIN32
  cli_mode=false;
#endif  // WIN32

  if(cli_mode) {
#ifndef WIN32
    QApplication a(argc,argv,false);
    new MainObject(NULL,"main");
    return a.exec();
#endif  // WIN32
  }
  else {
    QApplication a(argc,argv,true);
    
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
    tr_path=QString(PREFIX)+QString("/share/rivendell/");
    qt_path=QString(QTDIR)+QString("/translation/");
#endif  // WIN32
    QTranslator qt(0);
    qt.load(qt_path+QString("qt_")+QTextCodec::locale(),".");
    a.installTranslator(&qt);
    
    QTranslator rd(0);
    rd.load(tr_path+QString("librd_")+QTextCodec::locale(),".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(tr_path+QString("librdhpi_")+QTextCodec::locale(),".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(tr_path+QString("rmlsend_")+QTextCodec::locale(),".");
    a.installTranslator(&tr);

    //
    // Start Event Loop
    //
    MainWidget *w=new MainWidget(NULL,"main");
    a.setMainWidget(w);
    w->show();
    return a.exec();
  }
}
