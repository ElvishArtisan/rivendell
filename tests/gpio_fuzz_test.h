// gpio_fuzz_test.h
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

#ifndef GPIO_FUZZ_TEST_H
#define GPIO_FUZZ_TEST_H

#include <QObject>

#define GPIO_FUZZ_TEST_USAGE "[options]\n\nGenerate a series of Rivendell GPIO events\n\nOptions are:\n--first-line-number=<line-num>\n     First number of the range of line numbers to test\n\n--gpis\n     Include GPI events in the test\n\n--gpos\n     Include GPO events in the test\n\n--host-address=<addr>\n     IP address of system to execute events\n\n--interval=<msec>\n     Wait <msec> milliseconds between events\n\n--last-line-number=<line-num>\n     Last number of the range of line numbers to test\n\n--matrix-number=<num>\n     GPIO matrix number (0-7)\n\n--verbose\n     Print events to stdout\n\n"


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
};


#endif  // GPIO_FUZZ_TEST_H
