// dropbox_copy_test.cpp
//
// Generate multiple slow file transfers for testing dropboxes
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <QCoreApplication>

#include <rdapplication.h>
#include <rdcmd_switch.h>

#include "dropbox_copy_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool ok=false;
  RDCmdSwitch *cmd=new RDCmdSwitch("dropbox_copy_test",DROPBOX_COPY_TEST_USAGE);

  d_bytes_size=1024;
  d_gap_size=0;
  d_output_dir=QDir(".");

  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--xfer-size") {
      int blocks=cmd->value(i).toInt(&ok);
      if((!ok)||(blocks<=0)) {
	fprintf(stderr,"dropbox_copy_test: invalid argument\n");
	exit(RDApplication::ExitInvalidOption);
      }
      d_bytes_size=1024*blocks;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--gap-size") {
      d_gap_size=cmd->value(i).toInt(&ok);
      if((!ok)||(d_gap_size<0)) {
	fprintf(stderr,"dropbox_copy_test: invalid argument\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--in") {
      d_input_filenames.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--out") {
      d_output_dir=QDir(cmd->value(i));
      if(!d_output_dir.exists()) {
	fprintf(stderr,"dropbox_copy_test: no such directory\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"dropbox_copy_test: unrecognized option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(RDApplication::ExitInvalidOption);
    }
  }

  printf("Transfer Size: %d octets\n",d_bytes_size);
  printf("Gap Size: %d s\n",d_gap_size);
  for(int i=0;i<d_input_filenames.size();i++) {
    CopyFile(d_input_filenames.at(i));
  }

  exit(0);
}


void MainObject::CopyFile(const QString &filename) const
{
  struct stat statbuf;
  int n;
  int src_fd=-1;
  int dst_fd=-1;
  
  uint8_t *data=(uint8_t *)malloc(d_bytes_size);
  for(int i=0;i<d_input_filenames.size();i++) {
    printf("%s ",d_input_filenames.at(i).toUtf8().constData());
    fflush(stdout);
    if((src_fd=open(d_input_filenames.at(i).toUtf8(),O_RDONLY))<0) {
      printf("%s, skipping\n",strerror(errno));
      break;
    }
    memset(&statbuf,0,sizeof(statbuf));
    if(fstat(src_fd,&statbuf)!=0) {
      printf("%s, skipping\n",strerror(errno));
      close(src_fd);
      break;
    }
    QString outfile=d_output_dir.path();
    if(outfile.right(1)!="/") {
      outfile+="/";
    }
    outfile+=d_input_filenames.at(i).split("/",Qt::SkipEmptyParts).last();
    printf("=> %s ",outfile.toUtf8().constData());
    fflush(stdout);
    if((dst_fd=open(outfile.toUtf8(),O_CREAT|O_TRUNC|O_WRONLY,statbuf.st_mode))<0) {
      printf("%s, skipping\n",strerror(errno));
      close(src_fd);
      break;
    }
    while((n=read(src_fd,data,d_bytes_size))>0) {
      if(write(dst_fd,data,n)!=n) {
	printf("write error [%s]\n",strerror(errno));
	close(dst_fd);
	close(src_fd);
	break;
      }
      printf(".");
      fflush(stdout);
      sleep(d_gap_size);
    }
    if(n==0) {
      printf("done.\n");
    }
    else {// Read error
      printf("read error [%s]\n",strerror(errno));
    }
    close(dst_fd);
    close(src_fd);
  }
  free(data);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();

  return a.exec();
}
