//   rdurl.h
//
//   A URL Handling Class with Support for SMB URLs.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdurl.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDURL_H
#define RDURL_H

#include <qurl.h>


class RDUrl : public QUrl
{
 public:
  RDUrl();
  RDUrl(const QString &url);
  bool validSmbShare() const;
  QString smbShare() const;
  QString smbPath() const;
};


#endif  // RDURL_H
