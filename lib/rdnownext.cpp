// rdnownext.cpp
//
// Rivendell Now & Next Implementation
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdnownext.cpp,v 1.3.8.5 2014/01/07 18:18:29 cvs Exp $
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

#include <vector>

#include <qdatetime.h>

#include <rdnownext.h>
#include <rdweb.h>

void RDResolveNowNextDateTime(QString *str,const QString &code,
			      const QDateTime &dt)
{
  int ptr=0;
  std::vector<QString> dts;

  while((ptr=str->find(code,ptr))>=0) {
    for(unsigned i=ptr+3;i<str->length();i++) {
      if(str->at(i)==')') {
	dts.push_back(str->mid(ptr+3,i-ptr-3));
	ptr+=(i-ptr-3);
	break;
      }
    }
  }
  if(dt.isValid()&&(!dt.time().isNull())) {
    for(unsigned i=0;i<dts.size();i++) {
      str->replace(code+dts[i]+")",dt.toString(dts[i]));
    }
  }
  else {
    for(unsigned i=0;i<dts.size();i++) {
      str->replace(code+dts[i]+")","");
    }
  }
}


QString RDResolveNowNextEncode(const QString &str,int encoding)
{
  QString ret=str;
  
  switch(encoding) {
  case RLM_ENCODE_NONE:
    break;

  case RLM_ENCODE_XML:
    ret=RDXmlEscape(str);
    break;

  case RLM_ENCODE_URL:
    ret=RDUrlEscape(str);
    break;
  }

  return ret;
}

void RDResolveNowNext(QString *str,RDLogLine **loglines,int encoding)
{
  //
  // NOW PLAYING Event
  //
  if(loglines[0]!=NULL) {
    str->replace("%n",QString().sprintf("%06u",loglines[0]->cartNumber()));
    str->replace("%h",QString().sprintf("%d",loglines[0]->effectiveLength()));
    str->replace("%g",RDResolveNowNextEncode(loglines[0]->groupName(),
					     encoding));
    str->replace("%t",
		 RDResolveNowNextEncode(loglines[0]->title(),encoding));
    str->replace("%a",RDResolveNowNextEncode(loglines[0]->artist(),encoding));
    str->replace("%l",RDResolveNowNextEncode(loglines[0]->album(),encoding));
    str->replace("%r",RDResolveNowNextEncode(loglines[0]->conductor(),
					     encoding));
    str->replace("%s",RDResolveNowNextEncode(loglines[0]->songId(),encoding));
    str->replace("%y",loglines[0]->year().toString("yyyy"));
    str->replace("%b",RDResolveNowNextEncode(loglines[0]->label(),encoding));
    str->replace("%c",RDResolveNowNextEncode(loglines[0]->client(),encoding));
    str->replace("%e",RDResolveNowNextEncode(loglines[0]->agency(),encoding));
    str->replace("%m",RDResolveNowNextEncode(loglines[0]->composer(),encoding));
    str->replace("%p",RDResolveNowNextEncode(loglines[0]->publisher(),
					     encoding));
    str->replace("%u",RDResolveNowNextEncode(loglines[0]->userDefined(),
					     encoding));
    str->replace("%o",RDResolveNowNextEncode(loglines[0]->outcue(),encoding));
    str->replace("%i",RDResolveNowNextEncode(loglines[0]->description(),
    					     encoding));
    RDResolveNowNextDateTime(str,"%d(",loglines[0]->startDatetime());
  }
  else {   // No NOW PLAYING Event
    str->replace("%n","");
    str->replace("%h","");
    str->replace("%g","");
    str->replace("%t","");
    str->replace("%a","");
    str->replace("%l","");
    str->replace("%r","");
    str->replace("%s","");
    str->replace("%y","");
    str->replace("%b","");
    str->replace("%c","");
    str->replace("%e","");
    str->replace("%m","");
    str->replace("%p","");
    str->replace("%u","");
    str->replace("%o","");
    str->replace("%i","");
    RDResolveNowNextDateTime(str,"%d(",QDateTime());
  }

  //
  // NEXT Event
  //
  if(loglines[1]!=NULL) {
    str->replace("%N",QString().sprintf("%06u",loglines[1]->cartNumber()));
    str->replace("%H",QString().sprintf("%d",loglines[1]->effectiveLength()));
    str->replace("%G",RDResolveNowNextEncode(loglines[1]->groupName(),
					     encoding));
    str->replace("%T",RDResolveNowNextEncode(loglines[1]->title(),encoding));
    str->replace("%A",RDResolveNowNextEncode(loglines[1]->artist(),encoding));
    str->replace("%L",RDResolveNowNextEncode(loglines[1]->album(),encoding));
    str->replace("%R",RDResolveNowNextEncode(loglines[1]->conductor(),
					     encoding));
    str->replace("%S",RDResolveNowNextEncode(loglines[1]->songId(),encoding));
    str->replace("%Y",loglines[1]->year().toString("yyyy"));
    str->replace("%B",RDResolveNowNextEncode(loglines[1]->label(),encoding));
    str->replace("%C",RDResolveNowNextEncode(loglines[1]->client(),encoding));
    str->replace("%E",RDResolveNowNextEncode(loglines[1]->agency(),encoding));
    str->replace("%M",RDResolveNowNextEncode(loglines[1]->composer(),encoding));
    str->replace("%P",RDResolveNowNextEncode(loglines[1]->publisher(),
					     encoding));
    str->replace("%U",RDResolveNowNextEncode(loglines[1]->userDefined(),
					     encoding));
    str->replace("%O",RDResolveNowNextEncode(loglines[1]->outcue(),encoding));
    str->replace("%I",RDResolveNowNextEncode(loglines[1]->description(),
					     encoding));
    RDResolveNowNextDateTime(str,"%D(",loglines[1]->startDatetime());
  }
  else {   // No NEXT Event
    str->replace("%N","");
    str->replace("%H","");
    str->replace("%G","");
    str->replace("%T","");
    str->replace("%A","");
    str->replace("%L","");
    str->replace("%R","");
    str->replace("%S","");
    str->replace("%Y","");
    str->replace("%B","");
    str->replace("%C","");
    str->replace("%E","");
    str->replace("%M","");
    str->replace("%P","");
    str->replace("%U","");
    str->replace("%O","");
    str->replace("%I","");
    RDResolveNowNextDateTime(str,"%D(",QDateTime());
  }
  str->replace("%%","%");
  str->replace("\\r","\n");
  str->replace("\\n","\r\n");
}


QString RDResolveNowNext(const QString &pattern,RDLogLine *ll)
{
  QString ret=pattern;

  ret.replace("%n",QString().sprintf("%06u",ll->cartNumber()));
  ret.replace("%h",QString().sprintf("%d",ll->effectiveLength()));
  ret.replace("%g",ll->groupName());
  ret.replace("%t",ll->title());
  ret.replace("%a",ll->artist());
  ret.replace("%l",ll->album());
  ret.replace("%r",ll->conductor());
  ret.replace("%s",ll->songId());
  ret.replace("%y",ll->year().toString("yyyy"));
  ret.replace("%b",ll->label());
  ret.replace("%c",ll->client());
  ret.replace("%e",ll->agency());
  ret.replace("%m",ll->composer());
  ret.replace("%p",ll->publisher());
  ret.replace("%u",ll->userDefined());
  ret.replace("%o",ll->outcue());
  ret.replace("%i",ll->description());
  RDResolveNowNextDateTime(&ret,"%d(",ll->startDatetime());

  return ret;
}

