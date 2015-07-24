// audio_trim_test.h
//
// Test Rivendell low-level audio trim level measurement code.
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

#ifndef AUDIO_TRIM_TEST_H
#define AUDIO_TRIM_TEST_H

#include <list>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rdsettings.h>
#include <rdcmd_switch.cpp>

#define AUDIO_TRIM_TEST_USAGE "[options]\n\nTest the low-level Rivendell audio trim level routines\n\nOptions are:\n--filename=<filename>\n\n--trim-level=<100*dbfs>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  QString filename;
  int trim_level;
};


#endif  // AUDIO_TRIM_TEST_H
