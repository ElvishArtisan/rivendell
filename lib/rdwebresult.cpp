// rdwebresult.cpp
//
// Container class for Rivendel Web Service result messages.
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdwebresult.cpp,v 1.1 2011/12/23 23:07:00 cvs Exp $
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

#include <qstringlist.h>

#include <rdwebresult.h>

RDWebResult::RDWebResult(const QString &str,int resp_code,
		       RDAudioConvert::ErrorCode conv_code)
{
  web_text=str;
  web_response_code=resp_code;
  web_converter_code=conv_code;
}


RDWebResult::RDWebResult()
{
  web_text="Unknown";
  web_response_code=0;
  web_converter_code=RDAudioConvert::ErrorOk;
}


QString RDWebResult::text() const
{
  return web_text;
}


void RDWebResult::setText(const QString &str)
{
  web_text=str;
}


int RDWebResult::responseCode() const
{
  return web_response_code;
}


void RDWebResult::setResponseCode(int code)
{
  web_response_code=code;
}


RDAudioConvert::ErrorCode RDWebResult::converterErrorCode() const
{
  return web_converter_code;
}


void RDWebResult::setConverterErrorCode(RDAudioConvert::ErrorCode code)
{
  web_converter_code=code;
}


QString RDWebResult::xml() const
{
  QString ret="";

  ret+="<RDWebResult>\r\n";
  ret+=QString().sprintf("  <ResponseCode>%d</ResponseCode>\r\n",
			 web_response_code);
  ret+="  <ErrorString>"+web_text+"</ErrorString>\r\n";
  if(web_converter_code!=RDAudioConvert::ErrorOk) {
    ret+=QString().sprintf("  <AudioConvertError>%d</AudioConvertError>\r\n",
			   web_converter_code);
  }
  ret+="</RDWebResult>\r\n";
  return ret;
}


bool RDWebResult::readXml(const QString &xml)
{
  //
  // FIXME: This is totally ad-hoc, but should work until we settle on
  //        a proper XML parser.
  //
  QStringList list=list.split("\r\n",xml);
  for(unsigned i=0;i<list.size();i++) {
    //printf("%d: %s\n",i,(const char *)list[i]);
    if(list[i].contains("ErrorString")) {
      QStringList list2=list.split("<",list[i]);
      if(list2.size()>=2) {
	list2=list2.split(">",list2[1]);
	if(list2.size()>=2) {
	  web_text=list2[1];
	}
      }
    }
    if(list[i].contains("ResponseCode")) {
      QStringList list2=list.split("<",list[i]);
      if(list2.size()>=2) {
	list2=list2.split(">",list2[1]);
	if(list2.size()>=2) {
	  web_response_code=list2[1].toInt();
	}
      }
    }
    if(list[i].contains("AudioConvertError")) {
      QStringList list2=list.split("<",list[i]);
      if(list2.size()>=2) {
	list2=list2.split(">",list2[1]);
	if(list2.size()>=2) {
	  web_converter_code=(RDAudioConvert::ErrorCode)list2[1].toInt();
	}
      }
    }
  }

  return true;
}


bool RDWebResult::readXmlFromFile(const QString &filename)
{
  FILE *f=NULL;
  char line[1024];
  QString xml="";

  if((f=fopen(filename,"r"))==NULL) {
    return false;
  }
  while(fgets(line,1024,f)!=NULL) {
    xml+=line;
  }
  fclose(f);
  return readXml(xml);
}
