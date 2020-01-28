// readcd_test.h
//
// Test the Rivendell CD reader routines
//
//   (C) Copyright 2013-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef READCD_TEST_H
#define READCD_TEST_H

#include <qobject.h>

#define READCD_TEST_USAGE "[options]\n\nTest the Rivendell CD reading routines\n\nOptions are:\n--device\n     CD reader device\n\n--extended\n     Attempt to read extended per-track data\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);
};


#endif  // READCD_TEST_H
