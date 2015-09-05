// driver.h
//
// Abstract base class for CAE audio drivers.
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef DRIVER_H
#define DRIVER_H

#include <qstring.h>
#include <qobject.h>

#include <rdconfig.h>
#include <rdstation.h>

class Driver : public QObject
{
  Q_OBJECT
 public:
  enum Type {Hpi=0,Alsa=1,Jack=2};
  Driver(Driver::Type type,RDStation *station,RDConfig *config,
	 QObject *parent=0);
  ~Driver();
  Driver::Type type() const;
  virtual QString version()=0;
  virtual QString cardName(int card)=0;
  virtual int inputs(int card)=0;
  virtual int outputs(int card)=0;
  virtual int start(int next_card)=0;
  virtual bool loadPlayback(int card,QString wavename,int *stream)=0;
  virtual bool unloadPlayback(int card,int stream)=0;
  virtual bool playbackPosition(int card,int stream,unsigned pos)=0;
  virtual bool play(int card,int stream,int length,int speed,bool pitch,
		    bool rates)=0;
  virtual bool stopPlayback(int card,int stream)=0;
  virtual bool timescaleSupported(int card)=0;
  virtual bool loadRecord(int card,int stream,int coding,int chans,int samprate,
			  int bitrate,QString wavename)=0;
  virtual bool record(int card,int stream,int length,int thres)=0;
  virtual bool stopRecord(int card,int stream)=0;
  virtual bool unloadRecord(int card,int stream,unsigned *len)=0;
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
  virtual bool connectPorts(const QString &out,const QString &in);
  virtual bool disconnectPorts(const QString &out,const QString &in);
  static QString typeText(Driver::Type type);

 signals:
  void playStateChanged(int card,int stream,int state);
  void recordStateChanged(int card,int stream,int state);

 protected:
  RDStation *station();
  RDConfig *config();
  void logLine(RDConfig::LogPriority prio,const QString &line);

 private:
  Driver::Type dvr_type;
  RDStation *dvr_station;
  RDConfig *dvr_config;
};


#endif  // DRIVER_H
