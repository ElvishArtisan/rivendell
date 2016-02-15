// unlink_log_test.cpp
//
// Exercise the RDSvc::clearLogLinks() method.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdlog.h>
#include <rdsvc.h>

#include "unlink_log_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  unsigned schema=0;

  test_src=RDSvc::NoSource;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"unlink_log_test",
                   UNLINK_LOG_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--class") {
      if(cmd->value(i).lower()=="music") {
       test_src=RDSvc::Music;
       cmd->setProcessed(i,true);
      }
      if(cmd->value(i).lower()=="traffic") {
       test_src=RDSvc::Traffic;
       cmd->setProcessed(i,true);
      }
      if(!cmd->processed(i)) {
       fprintf(stderr,"unlink_log_test: unrecognized --class value\n");
       exit(256);
      }
    }
    if(cmd->key(i)=="--log-name") {
      test_log_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"unlink_log_test: unrecognized switch \"%s\"\n",
             (const char *)cmd->key(1));
      exit(256);
    }
  }

  if(test_log_name.isEmpty()) {
    fprintf(stderr,"unlink_log_test: missing --log-name\n");
    exit(256);
  }
  if(test_src==RDSvc::NoSource) {
    fprintf(stderr,"unlink_log_test: missing --class\n");
    exit(256);
  }

  //
  // Read Configuration
  //
  rdconfig=new RDConfig();
  rdconfig->load();

  //
  // Open Database
  //
  QString err (tr("unlink_log_test: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }

  //
  // Unlink
  //
  RDLog *log=new RDLog(test_log_name);
  if(!log->exists()) {
    fprintf(stderr,"unlink_log_test: no such log\n");
    exit(256);
  }
  if(log->service().isEmpty()) {
    fprintf(stderr,"unlink_log_test: log belongs to no service\n");
    exit(256);
  }
  printf("Using service: %s\n",(const char *)log->service());
  RDSvc *svc=new RDSvc(log->service(),this);
  if(!svc->exists()) {
    fprintf(stderr,"unlink_log_test: service \"%s\" does not exist\n",
           (const char *)log->service());
    exit(256);
  }
  if(log->linkQuantity((RDLog::Source)test_src)==0) {
    fprintf(stderr,"unlink_log_test: log \"%s\" contains no links\n",
           (const char *)test_log_name);
    exit(256);
  }
  if(log->linkState((RDLog::Source)test_src)==RDLog::LinkMissing) {
    printf("WARNING: log \"%s\" not currently linked!\n",
          (const char *)test_log_name);
  }
  svc->clearLogLinks(test_src,test_log_name);
  log->setLinkState((RDLog::Source)test_src,RDLog::LinkMissing);
  printf("DONE!\n");

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
