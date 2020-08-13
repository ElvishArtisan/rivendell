// rdcastsearch.cpp
//
// SQL search clause for RDCastManager
//
//   (C) Copyright 2009,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qstring.h>

#include <rdescape_string.h>
#include <rdpodcast.h>
#include <rdcastsearch.h>

QString RDCastSearchString(const QString &filter,bool active_only)
{
  QString ret;
  if(!filter.stripWhiteSpace().isEmpty()) {
    QString fil=RDEscapeString(filter);
    ret+=QString("&&((PODCASTS.ITEM_TITLE like \"%")+fil+"%\")||"+
      "(PODCASTS.ITEM_DESCRIPTION like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_CATEGORY like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_LINK like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_COMMENTS like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_AUTHOR like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_SOURCE_TEXT like \"%"+fil+"%\")||"+
      "(PODCASTS.ITEM_SOURCE_URL like \"%"+fil+"%\"))";
  }
  if(active_only) {
    ret+=QString().sprintf("&&(STATUS=%d)",RDPodcast::StatusActive);
  }
  return ret;
}


QString RDCastSearch(const QString &keyname,bool is_super,const QString &filter,
		     bool active_only)
{
  QString sql;
  RDSqlQuery *q;
  QString ret=QString("where (.KEY_NAME=\"")+
    RDEscapeString(keyname)+"\")";

  if(is_super) {
    ret="where ";
    sql=QString("select ")+
      "MEMBER_FEED_ID "+  // 00
      "from SUPERFEED_MAPS where "+
      "KEY_NAME=\""+RDEscapeString(keyname)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      ret+=QString().sprintf("PODCASTS.FEED_ID=%u || ",q->value(0).toUInt());
    }
    delete q;
    ret=ret.left(ret.length()-3);
  }
  else {
    sql=QString("select ")+
      "ID "+  // 00
      "from FEEDS where "+
      "KEY_NAME=\""+RDEscapeString(keyname)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      ret=QString().sprintf("where PODCASTS.FEED_ID=%u ",q->value(0).toUInt());
    }
    delete q;
  }
  ret+=RDCastSearchString(filter,active_only);

  return ret;
}
