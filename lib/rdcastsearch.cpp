// rdcastsearch.cpp
//
// SQL search clause for RDCastManager
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastsearch.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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
  if(!filter.isEmpty()) {
    ret+=QString().sprintf("&&((PODCASTS.ITEM_TITLE like \"%%%s%%\")||\
                              (PODCASTS.ITEM_DESCRIPTION like \"%%%s%%\")||\
                              (PODCASTS.ITEM_CATEGORY like \"%%%s%%\")||\
                              (PODCASTS.ITEM_LINK like \"%%%s%%\")||\
                              (PODCASTS.ITEM_COMMENTS like \"%%%s%%\")||\
                              (PODCASTS.ITEM_AUTHOR like \"%%%s%%\")||\
                              (PODCASTS.ITEM_SOURCE_TEXT like \"%%%s%%\")||\
                              (PODCASTS.ITEM_SOURCE_URL like \"%%%s%%\"))",
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter,
			   (const char *)filter);
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
  QString ret=QString().sprintf("where (KEY_NAME=\"%s\")",
				(const char *)RDEscapeString(keyname));
  ret+=RDCastSearchString(filter,unexp_only,active_only);

  return ret;
}
