// hpiplayout_test.h
//
// Test the Rivendell HPI playout routines
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef HPIPLAYOUT_TEST_H
#define HPIPLAYOUT_TEST_H

#include <QObject>

#include "rdconfig.h"
#include "rdwavefile.h"

#include "rdhpiplaystream.h"
#include "rdhpisoundcard.h"

#define HPIPLAYOUT_TEST_USAGE "[options]\n\nTest the Rivendell HPI play-out routines\n\nOptions are:\n--filename=<filename>\n     WAV file to play\n\n--card=<card-num>\n     Card number [0-15]\n\n--port=<port-num>\n     Port number [0-16]\n\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void isStoppedData(bool state);
  void playedData();
  void pausedData();
  void stoppedData();
  void positionData(int samples);
  void stateChangedData(int card,int stream,int state);

 private:
  QString d_filename;
  int d_card;
  int d_stream;
  int d_port;
  RDHPISoundCard *d_soundcard;
  RDHPIPlayStream *d_playstream;
  RDWaveData *d_wavedata;
  RDConfig *d_rdconfig;
};


#endif  // HPIPLAYOUT_TEST_H
