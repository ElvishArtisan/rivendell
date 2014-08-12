// rddeck.h
//
// Abstract a Rivendell Record/Play Deck
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddeck.h,v 1.20 2010/07/29 19:32:33 cvs Exp $
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

#include <qsqldatabase.h>
#include <qtimer.h>

#include <rdsettings.h>

#ifndef RDDECK_H
#define RDDECK_H

class RDDeck
{
  public:
   enum Status {Offline=0,Idle=1,Ready=2,Recording=3,Waiting=4};
   RDDeck(QString station,unsigned channel,bool create=false);
   bool isActive() const;
   QString station() const;
   int channel() const;
   int cardNumber() const;;
   void setCardNumber(int card) const;
   int streamNumber() const;
   void setStreamNumber(int stream) const;
   int portNumber() const;
   void setPortNumber(int port) const;
   int monitorPortNumber() const;
   void setMonitorPortNumber(int port) const;
   bool defaultMonitorOn() const;
   void setDefaultMonitorOn(bool state) const;
   RDSettings::Format defaultFormat() const;
   void setDefaultFormat(RDSettings::Format format) const;
   int defaultChannels() const;
   void setDefaultChannels(int chan) const;
   int defaultBitrate() const;
   void setDefaultBitrate(int rate) const;
   int defaultThreshold() const;
   void setDefaultThreshold(int level) const;
   QString switchStation() const;
   void setSwitchStation(QString str) const;
   int switchMatrix() const;
   QString switchMatrixName() const;
   void setSwitchMatrix(int matrix) const;
   int switchOutput() const;
   QString switchOutputName() const;
   void setSwitchOutput(int output) const;
   int switchDelay() const;
   void setSwitchDelay(int delay) const;

  private:
   int GetIntValue(const QString &field) const;
   QString GetStringValue(const QString &field) const;
   void SetRow(const QString &param,int value) const;
   void SetRow(const QString &param,const QString &value) const;
   void SetRow(const QString &param,bool value) const;
   int deck_channel;
   QString deck_station;
};


#endif 
