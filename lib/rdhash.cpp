// rdhash.cpp
//
// Functions for generating hashes.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>

#include <openssl/sha.h>

#include "rdhash.h"

QString RDSha1Hash(const QString &filename,bool throttle)
{
  QString ret;
  SHA_CTX ctx;
  int fd=-1;
  int n;
  char data[1024];
  unsigned char md[SHA_DIGEST_LENGTH];

  if((fd=open(filename,O_RDONLY))<0) {
    return ret;
  }
  SHA1_Init(&ctx);
  while((n=read(fd,data,1024))>0) {
    SHA1_Update(&ctx,data,n);
    if(throttle) {
      usleep(1);
    }
  }
  close(fd);
  SHA1_Final(md,&ctx);
  ret="";
  for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
    ret+=QString().sprintf("%02x",0xff&md[i]);
  }

  return ret;
}
