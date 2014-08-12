// audio_export_test.h
//
// Test the Rivendell file format converter.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audio_export_test.h,v 1.2 2010/07/29 19:32:38 cvs Exp $
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

#ifndef AUDIO_EXPORT_TEST_H
#define AUDIO_EXPORT_TEST_H

#include <list>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rdsettings.h>
#include <rdcmd_switch.cpp>

#define AUDIO_EXPORT_TEST_USAGE "[options]\n\nTest the Rivendell audio exporter routines\n\nOptions are:\n--username=<username>\n\n--password=<password>\n\n--cart-number=<cartnum>\n\n--cut-number=<cutnum>\n\n--destination-file=<filename>\n\n--start-point=<msecs>\n\n--end-point=<msecs>\n\n--destination-format=<fmt>\n     Supported formats are:\n        0 - PCM16 WAV\n        2 - MPEG Layer 2\n        3 - MPEG Layer 3\n        4 - FLAC\n        5 - OggVorbis\n        6 - MPEG Layer 2 WAV\n\n--destination-channels=<chans>\n\n--destination-sample-rate=<rate>\n\n--destination-bit-rate=<rate>\n\n--destination-quality=<qual>\n\n--normalization-level=<dbfs>\n\n"

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
  QString destination_filename;
  int start_point;
  int end_point;
  RDSettings *destination_settings;
};


#endif  // AUDIO_EXPORT_TEST_H
