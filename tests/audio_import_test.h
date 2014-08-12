// audio_import_test.h
//
// Test Rivendell file importing
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audio_import_test.h,v 1.2 2010/07/29 19:32:38 cvs Exp $
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

#ifndef AUDIO_IMPORT_TEST_H
#define AUDIO_IMPORT_TEST_H

#include <list>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rdsettings.h>
#include <rdcmd_switch.cpp>

#define AUDIO_IMPORT_TEST_USAGE "[options]\n\nTest the Rivendell audio importer routines\n\nOptions are:\n--username=<username>\n\n--password=<password>\n\n--cart-number=<cartnum>\n\n--cut-number=<cutnum>\n\n--source-file=<filename>\n\n--destination-channels=<chans>\n\n--normalization-level=<dbfs>\n\n--autotrim-level=<dbfs>\n\n--use-metadata\n\n"

//
// Global Variables
//
RDConfig *rdconfig;


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  QString username;
  QString password;
  unsigned cart_number;
  unsigned cut_number;
  QString source_filename;
  RDSettings *destination_settings;
  bool use_metadata;
};


#endif  // AUDIO_IMPORT_TEST_H
