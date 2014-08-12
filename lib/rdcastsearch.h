// rdcastsearch.h
//
// SQL search clause for RDCastManager
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastsearch.h,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDCASTSEARCH_H
#define RDCASTSEARCH_H

QString RDCastSearch(int feed_id,const QString &filter,bool unexp_only,
		     bool active_only);
QString RDCastSearch(const QString &keyname,const QString &filter,
		     bool unexp_only,bool active_only);


#endif  // RDCASTSEARCH_H
