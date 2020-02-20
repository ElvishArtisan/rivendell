// rdtransfer.h
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

#ifndef RDTRANSFER_H
#define RDTRANSFER_H

#include <qobject.h>
#include <qstringlist.h>
#include <qurl.h>

#include <rdconfig.h>

class RDTransfer : public QObject
{
  Q_OBJECT;
 public:
  RDTransfer(RDConfig *c,QObject *parent=0);
  virtual QStringList supportedSchemes() const=0;
  bool urlIsSupported(const QString &url);
  bool urlIsSupported(const QUrl &url);

 protected:
  RDConfig *config() const;

 private:
  RDConfig *xfer_config;
};


#endif  // RDTRANSFER_H
