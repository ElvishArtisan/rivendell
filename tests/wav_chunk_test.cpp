// wav_chunk_test.cpp
//
// Test Rivendell file uploading.
//
//   (C) Copyright 2010,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <qapplication.h>

#include <rdcmd_switch.h>

#include "wav_chunk_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  file_fd=-1;
  payload_length=0;
  chunk_length=0;

  QString filename;
  char buffer[1024];

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"wav_chunk_test",
  		    WAV_CHUNK_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"wav_chunk_test: invalid option\n");
      exit(256);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"wav_chunk_test: missing filename\n");
    exit(256);
  }

  //
  // Open File
  //
  if((file_fd=open(filename,O_RDONLY))<0) {
    fprintf(stderr,"wav_chunk_test: unable to open file [%s]\n",
	    strerror(errno));
    exit(256);
  }

  //
  // Get File Stats
  //
  struct stat stat;
  memset(&stat,0,sizeof(stat));
  if(fstat(file_fd,&stat)!=0) {
    fprintf(stderr,"wav_chunk_test: unable to stat file [%s]\n",
	    strerror(errno));
    exit(256);
  }
  file_length=stat.st_size;

  //
  // Read Header
  //
  memset(buffer,0,1024);
  if(read(file_fd,buffer,4)!=4) {
    fprintf(stderr,"wav_chunk_test: file truncated\n");
    exit(256);
  }
  if(strncmp(buffer,"RIFF",4)!=0) {
    fprintf(stderr,"wav_chunk_test: not a RIFF file\n");
    exit(256);
  }
  if(read(file_fd,buffer,4)!=4) {
    fprintf(stderr,"wav_chunk_test: file truncated\n");
    exit(256);
  }
  payload_length=((((uint32_t)buffer[3])&0xFF)<<24)+
    ((((uint32_t)buffer[2])&0xFF)<<16)+
    ((((uint32_t)buffer[1])&0xFF)<<8)+
    ((((uint32_t)buffer[0])&0xFF)<<0);
  if(read(file_fd,buffer,4)!=4) {
    fprintf(stderr,"wav_chunk_test: file truncated\n");
    exit(256);
  }
  if(strncmp(buffer,"WAVE",4)!=0) {
    fprintf(stderr,"wav_chunk_test: not a WAVE file\n");
    exit(256);
  }
  printf("        File Length: %u\n",file_length);
  printf("     Payload Length: %u\n",payload_length);
  if(file_length!=(payload_length+8)) {
    printf("WARNING: Payload and file sizes disagree!\n");
  }

  //
  // Enumerate Chunks
  //
  QString name;
  uint32_t start_pos;
  while((start_pos=NextChunk(name,&chunk_length))>0) {
    printf("Chunk: %s  Start: %u [0x%X]  Length: %u [0x%X]\n",(const char *)name,start_pos,start_pos,chunk_length+8,chunk_length+8);
    lseek(file_fd,chunk_length,SEEK_CUR);
  }

  //
  // Clean Up
  //
  close(file_fd);
  exit(0);
}


uint32_t MainObject::NextChunk(QString &name,uint32_t *len)
{
  char buffer[8];
  uint32_t pos;

  if((pos=lseek(file_fd,0,SEEK_CUR))>file_length) {
    printf("WARNING: chunk size points beyond end of file!\n");
    return 0;
  }

  if(read(file_fd,buffer,8)!=8) {
    return 0;
  }
  *len=((((uint32_t)buffer[7])&0xFF)<<24)+
    ((((uint32_t)buffer[6])&0xFF)<<16)+
    ((((uint32_t)buffer[5])&0xFF)<<8)+
    ((((uint32_t)buffer[4])&0xFF)<<0);
  buffer[4]=0;
  name=buffer;

  return pos;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
