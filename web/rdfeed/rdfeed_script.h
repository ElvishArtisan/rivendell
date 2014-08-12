// rdfeed_script.h
//
// An RSS Feed Generator for Rivendell.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdfeed_script.h,v 1.2 2010/07/29 19:32:40 cvs Exp $
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


#ifndef RDFEED_SCRIPT_H
#define RDFEED_SCRIPT_H

#include <qobject.h>
#include <qsqldatabase.h>

#include <rddb.h>


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void ServeRss(const char *keyname,bool count);
  void ServeLink(const char *keyname,int cast_id,bool count);
  QString ResolveChannelWildcards(RDSqlQuery *chan_q);
  QString ResolveItemWildcards(const QString &keyname,
			       RDSqlQuery *item_q,RDSqlQuery *chan_q);
  QString ResolveAuxWildcards(QString xml,QString keyname,unsigned feed_id,
			      unsigned cast_id);
  bool ShouldCount(const QString &hdr);
  void Redirect(const QString &url);
};


#endif  // RDFEED_SCRIPT_H
