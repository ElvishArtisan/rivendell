// datedecode_test.h
//
// Test the Rivendell date decoder routines.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: datedecode_test.h,v 1.1.2.2 2012/05/10 23:12:43 cvs Exp $
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

#ifndef DATEDECODE_TEST_H
#define DATEDECODE_TEST_H

#include <qobject.h>

#define DATEDECODE_TEST_USAGE "[options]\n\nTest the Rivendell date decoding routines\n\nOptions are:\n--date=<date-code>\n     Decode the <date-code> string using RDDateDecode() and print the result\n     on stdout.\n\n--datetime=<datetime-code>\n     Decode the <datetime-code> string using RDDateTimeDecode() and print\n     the result on stdout.\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);
};


#endif  // DATEDECODE_TEST_H
