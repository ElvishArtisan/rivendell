// provisioning_test.cpp
//
// Test Rivendell provisioning methods in RDConfig.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <unistd.h>

#include <QCoreApplication>

#include <rd.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>

#include "provisioning_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString config_filename=RD_CONF_FILE;
  QString test_host_name;

  //
  // Read Command Line
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("provisioning_test",PROVISIONING_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--config-file") {
      config_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--test-host-name") {
      test_host_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"provisioning_test: unknown option\n");
      exit(1);
    }
  }
  if(test_host_name.isEmpty()) {
    char name[HOST_NAME_MAX];
    if(gethostname(name,HOST_NAME_MAX)<0) {
      fprintf(stderr,"provisioning_test: %s\n",strerror(errno));
      exit(1);
    }
    test_host_name=QString::fromUtf8(name);
  }

  //
  // Run Test
  //
  RDConfig *config=new RDConfig(config_filename);
  if(!config->load()) {
    fprintf(stderr,"provisioning_test: failed to load config file\n");
    exit(1);
  }
  printf("  Config File: %s\n",config_filename.toUtf8().constData());
  printf("Test Hostname: %s\n",test_host_name.toUtf8().constData());
  printf("\n");
  printf("provisiongHostShortName: %s\n",
	config->provisioningHostShortName(test_host_name).toUtf8().constData());
  printf("provisioningServiceName: %s\n",
	 config->provisioningServiceName(test_host_name).toUtf8().constData());
  printf("provisioningHostIpAddress: %s\n",
	 config->provisioningHostIpAddress().toString().toUtf8().constData());

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
