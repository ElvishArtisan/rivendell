// rdnownext.cpp
//
// Rivendell Metadata Wildcards Implementation
//
//  MAINTAINERS'S NOTE: These mappings must be kept in sync with those
//                      in 'apis/PyPAD/api/PyPAD.py'!
//  
//   (C) Copyright 2008-2019 Fred Gleason <fredg@paravelsystems.com>
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
    for(int i=ptr+3;i<str->length();i++) {
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


QString RDResolveNowNext(const QString &pattern,RDLogLine *ll,int line)
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

