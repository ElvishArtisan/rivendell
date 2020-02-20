// rdtransfer.cpp
//
// Abstract base class for Rivendell remote data tranfer methods
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdtransfer.h"

RDTransfer::RDTransfer(RDConfig *c,QObject *parent)
  : QObject(parent)
{
  xfer_config=c;
}


bool RDTransfer::urlIsSupported(const QString &url)
{
  return urlIsSupported(QUrl(url));
}


bool RDTransfer::urlIsSupported(const QUrl &url)
{
  if(url.isRelative()||(!url.isValid())) {
    return false;
  }
  return supportedSchemes().contains(url.scheme());
}


RDConfig *RDTransfer::config() const
{
  return xfer_config;
}
