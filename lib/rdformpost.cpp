// rdformpost.cpp
//
// Handle data from an HTML form.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "rdconf.h"
#include "rddatetime.h"
#include "rdweb.h"

#include <rdformpost.h>

RDFormPost::RDFormPost(RDFormPost::Encoding encoding,unsigned maxsize,
		       bool auto_delete)
{
  bool ok=false;

  post_encoding=encoding;
  post_error=RDFormPost::ErrorNotInitialized;
  post_auto_delete=auto_delete;
  post_data=NULL;
  post_tempdir=NULL;

  //
  // Client Info
  //
  if(getenv("REMOTE_ADDR")!=NULL) {
    post_client_address.setAddress(getenv("REMOTE_ADDR"));
  }

  //
  // Verify Transfer Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    post_error=RDFormPost::ErrorNotPost;
    return;
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    post_error=RDFormPost::ErrorNotPost;
    return;
  }

  //
  // Verify Size
  //
  if(getenv("CONTENT_LENGTH")==NULL) {
    post_error=RDFormPost::ErrorPostTooLarge;
    return;
  }
  post_content_length=QString(getenv("CONTENT_LENGTH")).toUInt(&ok);
  if((!ok)||((maxsize>0)&&(post_content_length>maxsize))) {
    post_error=RDFormPost::ErrorPostTooLarge;
    return;
  }

  //
  // Get Content Type
  //
  if(getenv("CONTENT_TYPE")!=NULL) {
    post_content_type=getenv("CONTENT_TYPE");
  }

  //
  // Initialize Temp Directory
  //
  post_tempdir=new RDTempDirectory("rdformpost");
  QString err_msg;
  if(!post_tempdir->create(&err_msg)) {
    post_error=RDFormPost::ErrorNoTempDir;
    return;
  }

  //
  // (Perhaps) autodetect the encoding type
  //
  char first[2];
  read(0,first,1);
  if(post_encoding==RDFormPost::AutoEncoded) {
    if(first[0]=='-') {
      post_encoding=RDFormPost::MultipartEncoded;
    }
    else {
      post_encoding=RDFormPost::UrlEncoded;
    }
  }

  switch(post_encoding) {
  case RDFormPost::UrlEncoded:
    LoadUrlEncoding(first[0]);
    break;

  case RDFormPost::MultipartEncoded:
    LoadMultipartEncoding(first[0]);
    break;

  case RDFormPost::AutoEncoded:
    break;
  }
}


RDFormPost::~RDFormPost()
{
  if(post_auto_delete) {
    for(std::map<QString,bool>::const_iterator ci=post_filenames.begin();
	ci!=post_filenames.end();ci++) {
      if(ci->second) {
	unlink(post_values.at(ci->first).toString());
      }
    }
    if(post_tempdir!=NULL) {
      delete post_tempdir;
    }
    if(post_data!=NULL) {
      delete post_data;
    }
  }
}


RDFormPost::Error RDFormPost::error() const
{
  return post_error;
}


QHostAddress RDFormPost::clientAddress() const
{
  return post_client_address;
}


QStringList RDFormPost::names() const
{
  QStringList list;
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    list.push_back(ci->first);
  }
  return list;
}


QVariant RDFormPost::value(const QString &name,bool *ok)
{
  QVariant v;
  if(post_values.count(name)>0) {
    v=post_values.at(name);
  }
  if(ok!=NULL) {
    *ok=(post_values.count(name)>0);
  }
  return v;
}


bool RDFormPost::getValue(const QString &name,QHostAddress *addr,bool *ok)
{
  QString str;
  bool lok=getValue(name,&str);
  if(!lok) {
    return false;
  }
  addr->setAddress(str);
  if(ok!=NULL) {
    *ok=addr->isNull();
  }
  return true;
}


bool RDFormPost::getValue(const QString &name,QString *str,bool *ok)
{
  if(post_values.count(name)>0) {
    *str=post_values.at(name).toString();
    return true;
  }
  return false;
}


bool RDFormPost::getValue(const QString &name,int *n,bool *ok)
{
  if(post_values.count(name)>0) {
    *n=post_values.at(name).toInt(ok);
    return true;
  }
  return false;
}


bool RDFormPost::getValue(const QString &name,long *n,bool *ok)
{
  if(post_values.count(name)>0) {
    *n=post_values.at(name).toLongLong(ok);
    return true;
  }
  *n=0;
  return false;
}


bool RDFormPost::getValue(const QString &name,QDateTime *datetime,bool *ok)
{
  QString str;

  if(ok!=NULL) {
    *ok=false;
  }
  if(!getValue(name,&str)) {
    return false;
  }
  if(str.length()==0) {
    *datetime=QDateTime();
    if(ok!=NULL) {
      *ok=true;
    }
  }
  else {
    *datetime=RDParseDateTime(str,ok);
  }
  return true;
}


bool RDFormPost::getValue(const QString &name,QDate *date,bool *ok)
{
  QString str;

  if(ok!=NULL) {
    *ok=false;
  }
  if(!getValue(name,&str)) {
    return false;
  }
  if(str.length()==0) {
    if(ok!=NULL) {
      *ok=true;
    }    
    *date=QDate();
  }
  else {
    *date=RDParseXmlDate(str,ok);
  }
  return true;
}


bool RDFormPost::getValue(const QString &name,QTime *time,bool *ok)
{
  QString str;

  if(ok!=NULL) {
    *ok=false;
  }
  if(!getValue(name,&str)) {
    return false;
  }
  if(str.length()==0) {
    if(ok!=NULL) {
      *ok=true;
    }    
    *time=QTime();
  }
  else {
    *time=RDParseXmlTime(str,ok);
  }
  return true;
}


bool RDFormPost::getValue(const QString &name,bool *state,bool *ok)
{
  if(post_values.count(name)>0) {
    *state=post_values.at(name).toInt(ok);
    return true;
  }
  return false;
  
}


bool RDFormPost::isFile(const QString &name)
{
  return post_filenames[name];
}


QString RDFormPost::tempDir() const
{
  return post_tempdir->path();
}


unsigned RDFormPost::headerContentLength() const
{
  return post_content_length;
}


QString RDFormPost::headerContentType() const
{
  return post_content_type;
}


void RDFormPost::dump()
{
  printf("Content-type: text/html\n\n");
  printf("<table cellpadding=\"5\" cellspacing=\"0\" border=\"1\">\n");
  printf("<tr>\n");
  printf("<td colspan=\"3\" align=\"center\"><strong>RDFormPost Data Dump</strong></td>\n");
  printf("</tr>\n");

  printf("<tr>\n");
  printf("<th align=\"center\">NAME</th>\n");
  printf("<th align=\"center\">VALUE</th>\n");
  printf("<th align=\"center\">FILE</th>\n");
  printf("</tr>\n");
  
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    printf("<tr>\n");
    printf("<td align=\"left\">|%s|</td>\n",(const char *)ci->first.utf8());
    printf("<td align=\"left\">|%s|</td>\n",
	   (const char *)ci->second.toString().utf8());
    if(post_filenames[ci->first]) {
      printf("<td align=\"center\">Yes</td>\n");
    }
    else {
      printf("<td align=\"center\">No</td>\n");
    }
    printf("</tr>\n");
  }

  printf("</table>\n");
}


void RDFormPost::dumpRawPost()
{
  printf("Content-type: text/html\n\n");
  printf("%s",post_data);
  fflush(stdout);
}


QString RDFormPost::errorString(RDFormPost::Error err)
{
  QString str="Unknown error";

  switch(err) {
  case RDFormPost::ErrorOk:
    str="OK";
    break;

  case RDFormPost::ErrorNotPost:
    str="Request is not POST";
    break;

  case RDFormPost::ErrorNoTempDir:
    str="Unable to create temporary directory";
    break;

  case RDFormPost::ErrorMalformedData:
    str="The data is malformed";
    break;

  case RDFormPost::ErrorPostTooLarge:
    str="POST is too large";
    break;

  case RDFormPost::ErrorInternal:
    str="Internal error";
    break;

  case RDFormPost::ErrorNotInitialized:
    str="POST class not initialized";
    break;
  }
  return str;
}


QString RDFormPost::urlEncode(const QString &str)
{
  QString ret;

  for(int i=0;i<str.length();i++) {
    if(str.at(i).isLetterOrNumber()) {
      ret+=str.mid(i,1);
    }
    else {
      ret+=QString().sprintf("%%%02X",str.at(i).latin1());
    }
  }

  return ret;
}


QString RDFormPost::urlDecode(const QString &str)
{
  int istate=0;
  unsigned n;
  QString code;
  QString ret;
  bool ok=false;

  for(int i=0;i<str.length();i++) {
    switch(istate) {
    case 0:
      if(str.at(i)==QChar('+')) {
	ret+=" ";
      }
      else {
	if(str.at(i)==QChar('%')) {
	  istate=1;
	}
	else {
	  ret+=str.at(i);
	}
      }
      break;

    case 1:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code=str.mid(i,1);
      istate=2;
      break;

    case 2:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code+=str.mid(i,1);
      ret+=QChar(code.toInt(&ok,16));
      istate=0;
      break;
    }
  }

  return ret;
}


void RDFormPost::LoadUrlEncoding(char first)
{
  post_data=new char[post_content_length+1];
  int n;
  unsigned total_read=0;
  QStringList lines;
  QStringList line;

  post_data[0]=first;
  while(total_read<(post_content_length-1)) {
    if((n=read(0,post_data+1+total_read,post_content_length-1-total_read))<0) {
      post_error=RDFormPost::ErrorMalformedData;
      return;
    }
    total_read+=n;
  }

  post_data[post_content_length]=0;
  lines=QString(post_data).split("&");
  for(int i=0;i<lines.size();i++) {
    line=lines[i].split("=",QString::KeepEmptyParts);
    for(int j=2;j<line.size();j++) {
      line[1]+="="+line.at(j);
    }
    while(line.size()>2) {
      line.removeLast();
    }
    switch(line.size()) {
    case 1:
      post_values[line[0]]="";
      post_filenames[line[0]]=false;
      break;

    case 2:
      post_values[line[0]]=RDFormPost::urlDecode(line[1]);
      post_filenames[line[0]]=false;
      break;
    }
  }

  post_error=RDFormPost::ErrorOk;
}


void RDFormPost::LoadMultipartEncoding(char first)
{
  //
  // Create Stream Reader
  //
  if((post_stream=fdopen(0,"r"))==NULL) {
    post_error=RDFormPost::ErrorInternal;
    return;
  }

  /*
   * Uncomment to save raw post to disc
   *
  FILE *f;
  QString dumpfile=QString("/var/snd/post-")+
    QTime::currentTime().toString("hhmmsszzz")+".dat";
  if((f=fopen(dumpfile.toUtf8(),"w"))!=NULL) {
    char data[1025];
    int n;

    while((n=fread(data,1,1024,post_stream))>0) {
      fwrite(data,1,n,f);
    }
    fclose(f);
    printf("Content-type: text/html\n\n");
    printf("Raw post written to \"%s\"\n",(const char *)dumpfile.toUtf8());
    exit(0);
  }
  */

  //
  // Get Separator Line
  //
  post_separator=first+QString::fromUtf8(GetLine()).trimmed();

  //
  // Read Mime Parts
  //
  QString name;
  QString value;
  bool is_file;
  bool again=false;

  do {
    again=GetMimePart(&name,&value,&is_file);
    post_values[name]=value;
    post_filenames[name]=is_file;
  } while(again);
  post_error=RDFormPost::ErrorOk;
}


bool RDFormPost::GetMimePart(QString *name,QString *value,bool *is_file)
{
  QString line;
  int fd=-1;

  *name="";
  *value="";
  *is_file=false;

  //
  // Headers
  //
  do {
    line=QString::fromUtf8(GetLine());
    QStringList f0=line.split(":");
    if(f0.size()==2) {
      if(f0[0].lower()=="content-disposition") {
	QStringList f1=f0[1].split(";");
	for(int i=0;i<f1.size();i++) {
	  QStringList f2=f1[i].trimmed().split("=",QString::KeepEmptyParts);
	  for(int j=2;j<f2.size();j++) {
	    f2[1]+="="+f2.at(j);
	  }
	  while(f2.size()>2) {
	    f2.removeLast();
	  }
	  if(f2.size()==2) {
	    if(f2[0]=="name") {
	      *name=f2[1].replace("\"","");
	    }
	    if(f2[0]=="filename") {
	      *value=post_tempdir->path()+"/"+f2[1].replace("\"","");
	      fd=open(value->utf8(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
	      *is_file=true;
	    }
	  }
	}
      }
    }
  } while(!line.trimmed().isEmpty());

  //
  // Value
  //
  if(*is_file) {
    QByteArray data;
    data=GetLine();
    line=QString::fromUtf8(data).trimmed();
    while(!line.contains(post_separator)) {
      write(fd,data,data.length());
      data=GetLine();
      line=QString::fromUtf8(data).trimmed();
    }
  }
  else {
    line=QString::fromUtf8(GetLine());
    while((!line.isEmpty())&&(!line.contains(post_separator))) {
      *value+=line;
      line=QString::fromUtf8(GetLine());
    }
    *value=value->trimmed();
  }

  if(fd>=0) {
    ftruncate(fd,lseek(fd,0,SEEK_CUR)-2);  // Remove extraneous final CR/LF
    close(fd);
  }

  return line.trimmed().right(2)!="--";
}


QByteArray RDFormPost::GetLine() const
{
  char *data=NULL;
  size_t n=0;

  n=getline(&data,&n,post_stream);
  QByteArray ret(data,n);
  free(data);
  return ret;
}
