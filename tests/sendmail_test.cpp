// sendmail_test.cpp
//
// Test the Rivendell string encoder routines.
//
//   (C) Copyright 2013,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdio.h>

#include <qapplication.h>
#include <qvariant.h>

#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdsendmail.h>
#include <rdweb.h>

#include "sendmail_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  QString from_addr;
  QString to_addrs;
  QString cc_addrs;
  QString bcc_addrs;
  QString subject;
  QString body;
  QString body_file;
  bool dry_run=false;
  FILE *f=NULL;
  QByteArray raw;
  char data[1024];
  size_t n;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"sendmail_test",
		    SENDMAIL_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--from-addr") {
      from_addr=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--to-addrs") {
      to_addrs=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--cc-addrs") {
      cc_addrs=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--bcc-addrs") {
      bcc_addrs=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--subject") {
      subject=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--body") {
      body=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--body-file") {
      body_file=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--dry-run") {
      dry_run=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"sendmail_test: unknown option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(RDApplication::ExitInvalidOption);
    }
  }

  //
  // Sanity Checks
  //
  if((!body.isEmpty())&&(!body_file.isEmpty())) {
    fprintf(stderr,
	    "sendmail_test: --body and --body-file are mutually exclusive\n");
    exit(RDApplication::ExitInvalidOption);
  }

  //
  // Load Message Body
  //
  if(!body_file.isEmpty()) {
    if((f=fopen(body_file.toUtf8(),"r"))==NULL) {
      perror("sendmail_test");
      exit(256);
    }
    while((n=fread(data,1,1024,f))>0) {
      raw+=QByteArray(data,n);
    }
    fclose(f);
    body=QString::fromUtf8(raw);
  }

  if(!RDSendMail(&err_msg,subject,body,
		 from_addr,to_addrs,cc_addrs,bcc_addrs,dry_run)) {
    fprintf(stderr,"%s\n",err_msg.toUtf8().constData());
    exit(256);
  }

  exit(RDApplication::ExitOk);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
