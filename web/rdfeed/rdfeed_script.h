// rdfeed_script.h
//
// An RSS Feed Generator for Rivendell.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>

#define RDFEED_XML_USAGE "\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  void ServeRss(const char *keyname,bool count);
  void ServeLink(const char *keyname,int cast_id,bool count);
  bool ShouldCount(const QString &hdr);
  void Redirect(const QString &url);
};


#endif  // RDFEED_SCRIPT_H
