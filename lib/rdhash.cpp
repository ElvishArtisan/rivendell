// rdhash.cpp
//
// Functions for generating hashes.
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDateTime>

#include "rdhash.h"

QString __RDSha1Hash_MakePasswordHash(const QString &secret,const QString &salt)
{
  SHA_CTX ctx;
  unsigned char md[SHA_DIGEST_LENGTH];

  SHA1_Init(&ctx);
  SHA1_Update(&ctx,salt.toUtf8(),salt.toUtf8().length());
  SHA1_Update(&ctx,secret.toUtf8(),secret.toUtf8().length());
  SHA1_Final(md,&ctx);
  QString ret=salt;
  for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
    ret+=QString::asprintf("%02x",0xff&md[i]);
  }
  
  return ret;
}


QString RDSha1HashData(const QByteArray &data)
{
  SHA_CTX ctx;
  unsigned char md[SHA_DIGEST_LENGTH];
  QString ret;

  SHA1_Init(&ctx);
  SHA1_Update(&ctx,data,data.length());
  SHA1_Final(md,&ctx);
  for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
    ret+=QString::asprintf("%02x",0xff&md[i]);
  }
  
  return ret;
}


QString RDSha1HashFile(const QString &filename,bool throttle)
{
  QString ret;
  SHA_CTX ctx;
  int fd=-1;
  int n;
  char data[1024];
  unsigned char md[SHA_DIGEST_LENGTH];

  if((fd=open(filename.toUtf8(),O_RDONLY))<0) {
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
    ret+=QString::asprintf("%02x",0xff&md[i]);
  }

  return ret;
}


QString RDSha1HashPassword(const QString &secret)
{

  //
  // Create a salt value
  //
  srand(QDateTime::currentDateTime().toMSecsSinceEpoch());
  QString salt=QString::asprintf("%08x",rand());

  //
  // Generate the hash
  //
  return __RDSha1Hash_MakePasswordHash(secret,salt);
}


bool RDSha1HashCheckPassword(const QString &secret,const QString &hash)
{
  QString salt=secret.left(8);

  return __RDSha1Hash_MakePasswordHash(secret,hash.left(8))==hash;
}
