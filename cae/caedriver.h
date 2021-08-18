// caedriver.h
//
// Abstract base class for caed(8) audio driver implementations.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CAEDRIVER_H
#define CAEDRIVER_H

#include <QList>
#include <QObject>

#include <rdapplication.h>

class CaeDriver : public QObject
{
  Q_OBJECT
 public:
  CaeDriver(RDStation::AudioDriver type,QObject *parent=0);
  RDStation::AudioDriver driverType() const;
  bool hasCard(int cardnum) const;
  virtual QString version() const=0;
  virtual bool initialize(unsigned *next_cardnum)=0;;
  virtual void updateMeters()=0;

 public:
  virtual bool loadPlayback(int card,QString wavename,int *stream)=0;
  virtual bool unloadPlayback(int card,int stream)=0;
  virtual bool playbackPosition(int card,int stream,unsigned pos)=0;
  virtual bool play(int card,int stream,int length,int speed,bool pitch,
	       bool rates)=0;
  virtual bool stopPlayback(int card,int stream)=0;
  virtual bool timescaleSupported(int card)=0;
  virtual bool loadRecord(int card,int port,int coding,int chans,int samprate,
		     int bitrate,QString wavename)=0;
  virtual bool unloadRecord(int card,int stream,unsigned *len)=0;
  virtual bool record(int card,int stream,int length,int thres)=0;
  virtual bool stopRecord(int card,int stream)=0;
  virtual bool setClockSource(int card,int src)=0;
  virtual bool setInputVolume(int card,int stream,int level)=0;
  virtual bool setOutputVolume(int card,int stream,int port,int level)=0;
  virtual bool fadeOutputVolume(int card,int stream,int port,int level,
				int length)=0;
  virtual bool setInputLevel(int card,int port,int level)=0;
  virtual bool setOutputLevel(int card,int port,int level)=0;
  virtual bool setInputMode(int card,int stream,int mode)=0;
  virtual bool setOutputMode(int card,int stream,int mode)=0;
  virtual bool setInputVoxLevel(int card,int stream,int level)=0;
  virtual bool setInputType(int card,int port,int type)=0;
  virtual bool getInputStatus(int card,int port)=0;
  virtual bool getInputMeters(int card,int port,short levels[2])=0;
  virtual bool getOutputMeters(int card,int port,short levels[2])=0;
  virtual bool getStreamOutputMeters(int card,int stream,short levels[2])=0;
  virtual bool setPassthroughLevel(int card,int in_port,int out_port,
				   int level)=0;
  virtual void getOutputPosition(int card,unsigned *pos)=0;

 signals:
  void playStateChanged(int card,int stream,int state);
  void recordStateChanged(int card,int stream,int state);

 protected slots:
  void statePlayUpdate(int card,int stream,int state);
  void stateRecordUpdate(int card,int stream,int state);

 protected:
  void addCard(unsigned cardnum);
  RDConfig *config() const;

 private:
  RDStation::AudioDriver d_driver_type;
  QList<unsigned> d_cards;
};


#endif  // CAEDRIVER_H
