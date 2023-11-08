// rml_torture_test.h
//
// Generate a series of Rivendell GPIO events.
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RML_TORTURE_TEST_H
#define RML_TORTURE_TEST_H

#include <QObject>

#define RML_TORTURE_TEST_USAGE "[options]\n\nGenerate a series of Rivendell RML command\n\nOptions are:\n--rml=<rml-cmd>\n     RML command to send (may be given multiple times)\n\n--host-address=<addr>\n     IP address of system to execute events\n\n--interval=<msec>\n     Wait <msec> milliseconds between commands\n\n--verbose\n     Print events to stdout\n\n"


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);
};


#endif  // RML_TORTURE_TEST_H
