// metadata_wildcard_test.h
//
// Test the Rivendell multicast receiver routines
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef METADATA_WILDCARD_TEST_H
#define METADATA_WILDCARD_TEST_H

#include <qobject.h>

#include <rdmulticaster.h>

#define METADATA_WILDCARD_TEST_USAGE "[options]\n\nTest the Rivendell metadata wildcard routines\n\nOptions are:\n--cart=<cart-num>\n     Cart number.\n\n--cut=<cut-num>\n     Cut number [optional].\n\n--code=<str>\n     Encoded string to resolve.\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);
};


#endif  // METADATA_WILDCARD_TEST_H
