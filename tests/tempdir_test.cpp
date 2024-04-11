// tempdir_test.cpp
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

#include <signal.h>

#include <QApplication>

#include <rdcmd_switch.h>

#include "tempdir_test.h"

RDTempDirectory *d_temp_directory=NULL;

void SigHandler(int signo)
{
  exit(0);
}


void __Cleanup()
{
  printf("__Cleanup() - 1\n");
  delete d_temp_directory;
  printf("__Cleanup() - 2\n");
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("tempdir_test",TEMPDIR_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"tempdir_test: unknown option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(256);
    }
  }

  atexit(__Cleanup);
  ::signal(SIGINT,SigHandler);

  QString err_msg;
  d_temp_directory=new RDTempDirectory("tempdir_test");
  if(!d_temp_directory->create(&err_msg)) {
    fprintf(stderr,"tempdir_test: directory creation failed [%s]\n",
	    err_msg.toUtf8().constData());
    exit(1);
  }
  printf("created directory \"%s\"...\n",d_temp_directory->path().toUtf8().constData());

  WriteFile("file1.txt");
  WriteFile("file2.txt");
  WriteFile("file3.txt");
}


void MainObject::WriteFile(const QString &filename)
{
  FILE *f=NULL;
  QString pathname=d_temp_directory->path()+"/"+filename;

  if((f=fopen(pathname.toUtf8(),"w"))==NULL) {
    fprintf(stderr,"tempdir_test: file creation failed [%s]\n",strerror(errno));
    exit(1);
  }
  fprintf(f,"Hello World!\n");
  fclose(f);
  printf("created file \"%s\"...\n",pathname.toUtf8().constData());
}




int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
