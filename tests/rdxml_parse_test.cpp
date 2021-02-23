// rdxml_parse_test.cpp
//
// Test the Rivendell RDXML parser routines.
//
//   (C) Copyright 2016-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QFile>

#include <rdcart.h>
#include <rdcmd_switch.h>
#include <rdwavedata.h>

#include "rdxml_parse_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString filename="";
  QFile *file=NULL;
  char line[1025];
  QString xml="";

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdxml_parse_test",
		    RDXML_PARSE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdxml_parse_test: unknown option \"%s\"\n",
	      cmd->value(i).toUtf8().constData());
      exit(256);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"rdxml_parse_test: --filename= must be specified\n");
    exit(256);
  }

  //
  // Read File
  //
  file=new QFile(filename);
  if(!file->open(QIODevice::ReadOnly)) {
    fprintf(stderr,"rdxml_parse_test: unable to open \"%s\"\n",
	    filename.toUtf8().constData());
    exit(256);
  }
  while(file->readLine(line,1024)>=0) {
    xml+=line;
  }
  file->close();

  //
  // Parse
  //
  std::vector<RDWaveData> data;
  int n=RDCart::readXml(&data,xml);

  if(n<1) {
    fprintf(stderr,"rdxml_parse_test: no data found\n");
    exit(256);
  }
  printf("*** CART DATA ***\n");
  printf("%s\n",data[0].dump().toUtf8().constData());
  printf("\n");

  for(unsigned i=1;i<data.size();i++) {
    printf("*** CUT %u DATA ***\n",i);
    printf("%s\n",data[i].dump().toUtf8().constData());
    printf("\n");
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
