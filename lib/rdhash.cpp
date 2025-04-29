// rdhash.cpp
//
// Functions for generating and verifying hashes.
//
//   (C) Copyright 2017-2025 Fred Gleason <fredg@paravelsystems.com>
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
#include <openssl/evp.h>

#include <QDateTime>

#include "rdhash.h"

QString __RDSha1Hash_MakePasswordHash(const QString &secret,const QString &salt)
{
  QByteArray bytes;
  EVP_MD_CTX *ctx;
  unsigned char *digest=NULL;
  unsigned int digest_len=EVP_MD_size(EVP_sha1());

  ctx=EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx,EVP_sha1(),NULL);
  bytes=salt.toUtf8();
  EVP_DigestUpdate(ctx,bytes,bytes.size());
  bytes=secret.toUtf8();
  EVP_DigestUpdate(ctx,bytes,bytes.size());
  digest=(unsigned char *)OPENSSL_malloc(digest_len);
  EVP_DigestFinal_ex(ctx,digest,&digest_len);
  EVP_MD_CTX_free(ctx);
  QString ret=salt;
  for(unsigned i=0;i<digest_len;i++) {
    ret+=QString::asprintf("%02x",0xFF&(digest[i]));
  }
  
  return ret;
}


QString RDSha1HashData(const QByteArray &data)
{
  EVP_MD_CTX *ctx;
  unsigned char *digest=NULL;
  unsigned int digest_len=EVP_MD_size(EVP_sha1());

  ctx=EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx,EVP_sha1(),NULL);
  EVP_DigestUpdate(ctx,data,data.size());
  digest=(unsigned char *)OPENSSL_malloc(digest_len);
  EVP_DigestFinal_ex(ctx,digest,&digest_len);
  EVP_MD_CTX_free(ctx);
  QString ret;
  for(unsigned i=0;i<digest_len;i++) {
    ret+=QString::asprintf("%02x",0xFF&(digest[i]));
  }
  
  return ret;
}


QString RDSha1HashFile(const QString &filename,bool throttle)
{
  unsigned char buffer[1024];
  int fd=-1;
  int n;
  EVP_MD_CTX *ctx;
  unsigned char *digest=NULL;
  unsigned int digest_len=EVP_MD_size(EVP_sha1());

  if((fd=open(filename.toUtf8(),O_RDONLY))>=0) {
    ctx=EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx,EVP_sha1(),NULL);
    while((n=read(fd,buffer,1024))>0) {
      EVP_DigestUpdate(ctx,buffer,n);
    }
    if(n<0) {  // read() returned an error!
      fprintf(stderr,"RDSha1HashFile() read error: %s\n",strerror(errno));
    }
    digest=(unsigned char *)OPENSSL_malloc(digest_len);
    EVP_DigestFinal_ex(ctx,digest,&digest_len);
    EVP_MD_CTX_free(ctx);
    QString ret;
    for(unsigned i=0;i<digest_len;i++) {
      ret+=QString::asprintf("%02x",0xFF&(digest[i]));
    }
    return ret;
  }
  return QString();
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
