// dateparse_test.h
//
// Test the Rivendell date/time parser routines.
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef DATEPARSE_TEST_H
#define DATEPARSE_TEST_H

#include <qobject.h>

#define DATEPARSE_TEST_USAGE "[options]\n\nTest the Rivendell date parsing routines\n\nOptions are:\n--fomat=rfc822|xml|auto\n     Use RFC822 or XML string formats\n\n--print=date|time|datetime\n     If specified, print the system's current date/time, using the\n     specified format.\n\n--datetime=<datetime-str>\n     Parse the <datetime-str> string using RDGetWebDateTime() and print\n     the result on stdout.\n\n--time=<time-str>\n     Parse the <time-str> string using RDGetWebTime() and\n     print the results on stdout.\n\n"

class MainObject : public QObject
{
 public:
  enum Format {Unknown=0,Rfc822=1,Xml=2,Auto=3};
  enum PrintType {None=0,Date=1,Time=2,DateTime=3};
  MainObject(QObject *parent=0);
};


#endif  // DATEPARSE_TEST_H
