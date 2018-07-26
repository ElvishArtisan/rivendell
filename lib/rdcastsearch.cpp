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

QString RDCastSearchString(const QString &filter,bool unexp_only,
			   bool active_only)
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
  if(unexp_only) {
    ret+=QString().sprintf("&&(STATUS!=%d)",RDPodcast::StatusExpired);
  }
  if(active_only) {
    ret+=QString().sprintf("&&(STATUS=%d)",RDPodcast::StatusActive);
  }
  return ret;
}

QString RDCastSearch(int feed_id,const QString &filter,bool unexp_only,
		     bool active_only)
{
  QString ret=QString().sprintf("where (FEED_ID=%d)",feed_id);
  ret+=RDCastSearchString(filter,unexp_only,active_only);

  return ret;
}


QString RDCastSearch(const QString &keyname,const QString &filter,
		     bool unexp_only,bool active_only)
{
  QString ret=QString("where (KEY_NAME=\"")+
    RDEscapeString(keyname)+"\")";
  ret+=RDCastSearchString(filter,unexp_only,active_only);

  return ret;
}
