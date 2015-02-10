// conveyor_test.h
//
// Test the Rivendell replicator conveyor routines.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CONVEYOR_TEST_H
#define CONVEYOR_TEST_H

#include <qobject.h>

#define CONVEYOR_TEST_USAGE "[options]\n\nTest the Rivendell replicator conveyor routines\n\nOptions are:\n--name=<repl-name>\n     Use <name> replicator.\n\n--direction=<dir>\n     The direction, 0 = Inbound, 1 = Outbound\n\n--add-file=<filename>\n     Add <filename> to the specified conveyor.\n\n--recv-file=<filename>\n     Receive file from the specified conveyor.\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  bool Process() const;
};


#endif  // CONVEYOR_TEST_H
