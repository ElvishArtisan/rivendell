// rmlsend.cpp
//
// A utility for sending RML Commands
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qfont.h>
#include <q3socketdevice.h>
#include <qhostaddress.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <q3textstream.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <QPixmap>

#include <rdcmd_switch.h>

#include "rmlsend.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

//
// Globals
//
RDCmdSwitch *rdcmdswitch=NULL;

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
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
  rivendell_map=new QPixmap(rivendell_22x22_xpm);
  setWindowIcon(*rivendell_map);

  setWindowTitle(QString("RMLSend")+" v"+VERSION+" - "+
		 tr("Macro Command Utility"));

  host=new QLineEdit(this);
  host->setGeometry(80,10,180,25);
  host->setFont(main_font);
  QLabel *label=new QLabel(host,"Sent To:",this);
  label->setGeometry(10,16,65,14);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight);
  for(unsigned i=0;i<rdcmdswitch->keys();i++) {
    if(rdcmdswitch->key(i)=="--to-host") {
      rdcmdswitch->setProcessed(i,true);
      host->setText(rdcmdswitch->value(i));
    }
  }

  port_box=new QComboBox(this);
  port_box->setGeometry(305,10,130,25);
  port_box->setEditable(false);
  label=new QLabel(port_box,"Dest:",this);
  label->setGeometry(270,16,30,14);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight);
  port_box->insertItem(tr("RML"));
  port_box->insertItem(tr("RML (no echo)"));
  port_box->insertItem(tr("Set Port"));
  port_box->setCurrentItem(1);
  connect(port_box,SIGNAL(activated(int)),this,SLOT(destChangedData(int)));

  port_edit=new QLineEdit(this);
  port_edit->setGeometry(sizeHint().width()-60,10,50,25);
  port_edit->setFont(main_font);
  port_edit->setDisabled(true);
  port_edit_label=new QLabel(port_edit,tr("UDP Port:"),this);
  port_edit_label->setGeometry(sizeHint().width()-130,16,65,14);
  port_edit_label->setFont(label_font);
  port_edit_label->setAlignment(Qt::AlignRight);
  port_edit_label->setDisabled(true);

  command=new QLineEdit(this);
  command->setGeometry(80,40,sizeHint().width()-90,25);
  command->setFont(main_font);
  label=new QLabel(command,tr("Command:"),this);
  label->setGeometry(10,46,65,14);
  label->setFont(label_font);
  label->setAlignment(Qt::AlignRight);

  response=new QLineEdit(this);
  response->setGeometry(80,70,sizeHint().width()-90,25);
  response->setFont(main_font);
  response_label=new QLabel(response,tr("Response:"),this);
  response_label->setGeometry(10,76,65,14);
  response_label->setFont(label_font);
  response_label->setAlignment(Qt::AlignRight);

  send=new QPushButton(tr("&Send Command"),this);
  send->setGeometry(10,sizeHint().height()-50,120,40);
  send->setFont(main_font);
  connect(send,SIGNAL(clicked()),this,SLOT(sendCommand()));

  quit=new QPushButton("&Quit",this,"quit");
  quit->setGeometry(sizeHint().width()-80,sizeHint().height()-50,70,40);
  quit->setFont(main_font);
  quit->setDefault(true);
  connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));

  udp_command=new Q3SocketDevice(Q3SocketDevice::Datagram);

  udp_response=new Q3SocketDevice(Q3SocketDevice::Datagram);
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
      QMessageBox::information(this,tr("RMLSend"),tr("Invalid Port Number!"));
      return;
    }
    break;
  }
  response->setText("");
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
  dcl_command=command->text();
  if(!udp_command->writeBlock(dcl_command.utf8(),dcl_command.utf8().length(),
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

MainObject::MainObject(QObject *parent,const char *name)
{
  input_fd=-1;
  input_stream=NULL;
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
  FILE *f=NULL;

  if(!rml_cmd.isEmpty()) {
    return;
  }
  if(input_file=="-") {
    f=stdin;
  }
  else {
    if((f=fopen(input_file,"r"))==NULL) {
      perror("rmlsend");
      exit(1);
    }
  }
  input_stream=new Q3TextStream(f,QIODevice::ReadOnly);
  input_stream->setEncoding(Q3TextStream::UnicodeUTF8);
}


void MainObject::CloseStream()
{
  if(input_fd>0) {
    close(input_fd);
  }
}


bool MainObject::GetNextChar(QChar *c)
{
  if(input_stream!=NULL) {
    if(input_stream->atEnd()) {
      return true;
    }
    *input_stream >> *c; 
   return false;
  }
  if(rml_ptr>=rml_cmd.length()) {
    return true;
  }
  *c=rml_cmd.at(rml_ptr++);
  return false;
}


void MainObject::ProcessCommands()
{
  QChar c;
  Q3SocketDevice *udp_command=new Q3SocketDevice(Q3SocketDevice::Datagram);
  QString rml="";
  bool active=false;

  while(!GetNextChar(&c)) {
    if(active) {
      if(c=='!') {
	rml+=c;
	udp_command->writeBlock(rml.utf8(),rml.utf8().length(),
				*dest_addr,dest_port);
	rml="";
	active=false;
      }
      else {
	rml+=c;
      }
    }
    else {
      if(c.isLetterOrNumber()) {
	rml+=c;
	active=true;
      }
    }
  }
}


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
  if(cli_mode) {
    QApplication a(argc,argv,false);
    new MainObject();
    return a.exec();
  }
  else {
    QApplication::setStyle(new QWindowsStyle);
    QApplication a(argc,argv);
    
    //
    // Load Translations
    //
    QString tr_path;
    QString qt_path;

    tr_path=QString(PREFIX)+QString("/share/rivendell/");
    qt_path=QString("/usr/share/qt4/translation/");

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
    MainWidget *w=new MainWidget();
    a.setMainWidget(w);
    w->show();
    return a.exec();
  }
}
