// stringcode_test.cpp
//
// Test the Rivendell string encoder routines.
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdcmd_switch.h>
#include <rdweb.h>

#include "stringcode_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  char teststr[1024];
  QString str;
  bool xml_encode=false;
  bool xml_decode=false;
  bool url_encode=false;
  bool url_decode=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"stringcode_test",
		    STRINGCODE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--xml-encode") {
      xml_encode=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--xml-decode") {
      xml_decode=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--url-encode") {
      url_encode=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--url-decode") {
      url_decode=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"stringcode_test: unknown option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(256);
    }
  }

  if((!xml_encode)&&(!xml_decode)&&(!url_encode)&&(!url_decode)) {
    fprintf(stderr,"stringcode_test: nothing to do!\n");
    exit(256);
  }

  //
  // Get string to test
  //
  printf("Enter string: ");
  fflush(stdout);
  if(fgets(teststr,1024,stdin)==NULL) {
    teststr[0]=0;
  }
  str=QString(teststr).replace("\n","");

  printf("Testing String: |%s|\n",str.toUtf8().constData());
  if(xml_encode) {
    printf(" RDXmlEscape: |%s|\n",RDXmlEscape(str).toUtf8().constData());
  }
  if(xml_decode) {
    printf(" RDXmlUnescape: |%s|\n",RDXmlUnescape(str).toUtf8().constData());
  }
  if(url_encode) {
    printf(" RDUrlEscape: |%s|\n",RDUrlEscape(str).toUtf8().constData());
  }
  if(url_decode) {
    printf(" RDUrlUnescape: |%s|\n",RDUrlUnescape(str).toUtf8().constData());
  }
  printf("\n");

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
