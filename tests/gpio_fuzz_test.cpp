// gpio_fuzz_test.cpp
//
// Generate a series of Rivendell GPIO events.
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QUdpSocket>

#include <rd.h>
#include <rdcmd_switch.h>
#include <rdcoreapplication.h>

#include "gpio_fuzz_test.h"

MainObject::MainObject(QObject *parent)
{
  bool gpis=false;
  bool gpos=false;
  QHostAddress host_address("127.0.0.1");
  int interval=1000;
  int first_line_number=-1;
  int last_line_number=-1;
  int matrix_number=-1;
  int line_number=-1;
  bool verbose=false;
  bool ok=false;

  QString type;

  RDCmdSwitch *cmd=new RDCmdSwitch("gpio_fuzz_test",GPIO_FUZZ_TEST_USAGE);

  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--gpis") {
      gpis=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--gpos") {
      gpos=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--host-address") {
      if(!host_address.setAddress(cmd->value(i))) {
	fprintf(stderr,"gpio_fuzz_test: invalid --host-address\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--interval") {
      interval=cmd->value(i).toInt(&ok);
      if((!ok)||(interval<=0)) {
	fprintf(stderr,"gpio_fuzz_test: invalid --interval\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--first-line-number") {
      first_line_number=cmd->value(i).toInt(&ok);
      if((!ok)||(first_line_number<=0)) {
	fprintf(stderr,"gpio_fuzz_test: invalid --first-line-number\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--last-line-number") {
      last_line_number=cmd->value(i).toInt(&ok);
      if((!ok)||(last_line_number<=0)) {
	fprintf(stderr,"gpio_fuzz_test: invalid --last-line-number\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--matrix-number") {
      matrix_number=cmd->value(i).toInt(&ok);
      if((!ok)||(matrix_number<0)||(matrix_number>=MAX_MATRICES)) {
	fprintf(stderr,"gpio_fuzz_test: invalid --matrix-number\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verbose") {
      verbose=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"gpio_fuzz_test: unknown option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(RDCoreApplication::ExitInvalidOption);
      cmd->setProcessed(i,true);
    }
  }
  if((!gpis)&&(!gpos)) {
    fprintf(stderr,"gpio_fuzz_test: no GPIO types specified\n");
    exit(RDCoreApplication::ExitInvalidOption);
  }
  if(matrix_number<0) {
    fprintf(stderr,"gpio_fuzz_test: no --matrix-number specified\n");
    exit(RDCoreApplication::ExitInvalidOption);
  }
  if(first_line_number<0) {
    fprintf(stderr,"gpio_fuzz_test: no --first-line-number specified\n");
    exit(RDCoreApplication::ExitInvalidOption);
  }
  if(last_line_number<0) {
    fprintf(stderr,"gpio_fuzz_test: no --last-line-number specified\n");
    exit(RDCoreApplication::ExitInvalidOption);
  }

  //
  // Send Socket
  //
  QUdpSocket *send_socket=new QUdpSocket(this);

  while(1==1) {
    if(gpis) {
      type="I";
    }
    if(gpos) {
      type="O";
    }
    if(gpis&&gpos) {
      if(random()<(RAND_MAX/2)) {
	type="I";
      }
      else {
	type="O";
      }
    }
    line_number=
      first_line_number+((int64_t)(1+last_line_number-first_line_number)*(int64_t)random())/RAND_MAX;
    QString msg=QString::asprintf("GO %d %s %d 1 300!",
				  matrix_number,
				  type.toUtf8().constData(),
				  line_number);
    if(verbose) {
      printf("%s\n",msg.toUtf8().constData());
    }
    send_socket->writeDatagram(msg.toUtf8(),host_address,RD_RML_NOECHO_PORT);
    usleep(1000*interval);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
