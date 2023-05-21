// rdweb.cpp
//
// Functions for interfacing with web components using the
// Common Gateway Interface (CGI) Standard 
//
//   (C) Copyright 1996-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rdconf.h"
#include "rddatetime.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdtempdirectory.h"
#include "rduser.h"
#include "rdwebresult.h"

#include "rdweb.h"

extern void RDXMLResult(const char *str,int resp_code,
			RDAudioConvert::ErrorCode err)
{
  RDWebResult *we=new RDWebResult(str,resp_code,err);

  printf("Content-type: application/xml\n");
  printf("Status: %d\n",resp_code);
  printf("\n");
  printf("%s",we->xml().toUtf8().constData());
  delete we;

  exit(0);
}


QString RDXmlField(const QString &tag,const QString &value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+RDXmlEscape(value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const char *value,const QString &attrs)
{
  return RDXmlField(tag,QString(value),attrs);
}


QString RDXmlField(const QString &tag,const int value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+QString::asprintf("%d",value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const int64_t value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+QString::asprintf("%ld",value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const unsigned value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  return QString("<")+tag+str+">"+QString::asprintf("%u",value)+"</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const bool value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value) {
    return QString("<")+tag+str+">true</"+tag+">\n";
  }
  return QString("<")+tag+str+">false</"+tag+">\n";
}


QString RDXmlField(const QString &tag,const QDateTime &value,
		   const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value.isValid()) {
    return QString("<")+tag+str+">"+RDWriteXmlDateTime(value)+"</"+tag+">\n";
  }
  return RDXmlField(tag);
}


QString RDXmlField(const QString &tag,const QDate &value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value.isValid()&&(!value.isNull())) {
    return QString("<")+tag+str+">"+RDWriteXmlDate(value)+"</"+tag+">\n";
  }
  return RDXmlField(tag);
}


QString RDXmlField(const QString &tag,const QTime &value,const QString &attrs)
{
  QString str="";

  if(!attrs.isEmpty()) {
    str=" "+attrs;
  }
  if(value.isValid()&&(!value.isNull())) {
    return QString("<")+tag+str+">"+RDWriteXmlTime(value)+"</"+tag+">\n";
  }
  return RDXmlField(tag);
}


QString RDXmlField(const QString &tag)
{
  return QString("<")+tag+"/>\n";
}


QString RDXmlEscape(const QString &str)
{
  /*
   * Escape a string in accordance with XML-1.0
   */
  QString ret=str;
  ret.replace("&","&amp;");
  ret.replace("<","&lt;");
  ret.replace(">","&gt;");
  ret.replace("'","&apos;");
  ret.replace("\"","&quot;");
  return ret;
}


QString RDXmlUnescape(const QString &str)
{
  /*
   * Unescape a string in accordance with XML-1.0
   */
  QString ret=str;
  ret.replace("&amp;","&");
  ret.replace("&lt;","<");
  ret.replace("&gt;",">");
  ret.replace("&apos;","'");
  ret.replace("&quot;","\"");
  return ret;
}


QString RDJsonPadding(int padding)
{
  QString ret="";

  for(int i=0;i<padding;i++) {
    ret+=" ";
  }
  return ret;
}


QString RDJsonNullField(const QString &name,int padding,bool final)
{
  QString comma=",";

  if(final) {
    comma="";
  }

  return RDJsonPadding(padding)+"\""+name+"\": null"+comma+"\r\n";
}


QString RDJsonField(const QString &name,bool value,int padding,bool final)
{
  QString comma=",";

  if(final) {
    comma="";
  }

  if(value) {
    return RDJsonPadding(padding)+"\""+name+"\": true"+comma+"\r\n";
  }
  return RDJsonPadding(padding)+"\""+name+"\": false"+comma+"\r\n";
}


QString RDJsonField(const QString &name,int value,int padding,bool final)
{
  QString comma=",";

  if(final) {
    comma="";
  }

  return RDJsonPadding(padding)+"\""+name+"\": "+QString::asprintf("%d",value)+
    comma+"\r\n";
}


QString RDJsonField(const QString &name,unsigned value,int padding,bool final)
{
  QString comma=",";

  if(final) {
    comma="";
  }

  return RDJsonPadding(padding)+"\""+name+"\": "+QString::asprintf("%u",value)+
    comma+"\r\n";
}


QString RDJsonField(const QString &name,const QString &value,int padding,
		    bool final)
{
  QString ret;
  QString comma=",";

  if(final) {
    comma="";
  }

  for(int i=0;i<value.length();i++) {
    QChar c=value.at(i);
    switch(c.category()) {
    case QChar::Other_Control:
      ret+=QString::asprintf("\\u%04X",c.unicode());
      break;

    default:
      switch(c.unicode()) {
      case 0x22:   // Quote
	ret+="\\\"";
	break;

      case 0x5C:   // Backslash
	ret+="\\\\";
	break;

      default:
	ret+=c;
	break;
      }
      break;
    }
  }

  return RDJsonPadding(padding)+"\""+name+"\": \""+ret+"\""+comma+"\r\n";
}


QString RDJsonField(const QString &name,const QDateTime &value,int padding,
		    bool final)
{
  QString comma=",";

  if(final) {
    comma="";
  }

  if(!value.isValid()) {
    return RDJsonNullField(name,padding,final);
  }
  return RDJsonPadding(padding)+"\""+name+"\": \""+
    RDWriteXmlDateTime(value)+"\""+
    comma+"\r\n";
}


QString RDUrlEscape(const QString &str)
{
  /*
   * Escape a string in accordance with RFC 2396 Section 2.4
   */
  QString ret=str;

  ret.replace("%","%25");
  ret.replace(" ","%20");
  ret.replace("<","%3C");
  ret.replace(">","%3E");
  ret.replace("#","%23");
  ret.replace("\"","%22");
  ret.replace("{","%7B");
  ret.replace("}","%7D");
  ret.replace("|","%7C");
  ret.replace("\\","%5C");
  ret.replace("^","%5E");
  ret.replace("[","%5B");
  ret.replace("]","%5D");
  ret.replace("~","%7E");

  return ret;
}


QString RDUrlUnescape(const QString &str)
{
  /*
   * Unescape a string in accordance with RFC 2396 Section 2.4
   */
  QString ret="";

  for(int i=0;i<str.length();i++) {
    if((str.at(i)==QChar('%'))&&(i<str.length()-2)) {
      ret+=QString::asprintf("%c",str.mid(i+1,2).toInt(NULL,16));
      i+=2;
    }
    else {
      ret+=str.at(i);
    }
  }

  return ret;
}
