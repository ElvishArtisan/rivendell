// reserve_carts_test.h
//
// Test the Rivendell cart reservation routines.
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: reserve_carts_test.h,v 1.1.2.1 2014/05/30 00:26:31 cvs Exp $
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

#ifndef RESERVE_CARTS_TEST_H
#define RESERVE_CARTS_TEST_H

#include <qobject.h>

#define RESERVE_CARTS_TEST_USAGE "[options]\n\nTest the Rivendell cart reservation routines\n\nOptions are:\n--group=<name>\n     Name of group to reserve carts in.\n\n--quantity=<num>\n     Number of carts to reserve.\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);
};


#endif  // RESERVE_CARTS_TEST_H
