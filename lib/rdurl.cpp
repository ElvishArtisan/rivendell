//   rdurl.cpp
//
//   A URL Handling Class with Support for SMB URLs.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdurl.h"

RDUrl::RDUrl()
  : QUrl()
{
}


RDUrl::RDUrl(const QString &url)
  : QUrl(url)
{
}


bool RDUrl::validSmbShare() const
{
  return (scheme()=="smb")&&(!host().isEmpty())&&(path()!="/"); 
}


QString RDUrl::smbShare() const
{
  int offset=0;
  if((offset=path().right(path().length()-1).indexOf("/"))<0) {
    offset=path().length();
  }
  return QString("//")+host()+path().left(offset+1);
}


QString RDUrl::smbPath() const
{
  int offset=0;
  if((offset=path().right(path().length()-1).indexOf("/"))<0) {
    return QString("/");
  }
  return path().right(path().length()-offset-1);
}
