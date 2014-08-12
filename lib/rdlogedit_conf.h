// rdlogedit_conf.h
//
// Abstract RDLogedit Configuration
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogedit_conf.h,v 1.10.8.1 2014/01/08 18:14:35 cvs Exp $
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

#ifndef RDLOGEDIT_CONF_H
#define RDLOGEDIT_CONF_H

#include <qsqldatabase.h>

#ifndef WIN32
#include <rdsettings.h>
#endif  // WIN32
#include <rdlog_line.h>


class RDLogeditConf
{
 public:
  RDLogeditConf(const QString &station);
  QString station() const;
  int inputCard() const;
  void setInputCard(int input) const;
  int inputPort() const;
  void setInputPort(int input) const;
  int outputCard() const;
  void setOutputCard(int output) const;
  int outputPort() const;
  void setOutputPort(int input) const;
  unsigned format() const;
  void setFormat(unsigned format) const;
  unsigned layer() const;
  void setLayer(unsigned layer) const;
  unsigned bitrate() const;
  void setBitrate(unsigned rate) const;
  bool enableSecondStart() const;
  void setEnableSecondStart(bool state) const;
  unsigned defaultChannels() const;
  void setDefaultChannels(unsigned chans) const;
  unsigned maxLength() const;
  void setMaxLength(unsigned length) const;
  unsigned tailPreroll() const;
  void setTailPreroll(unsigned length) const;
  unsigned startCart() const;
  void setStartCart(unsigned cartnum) const;
  unsigned endCart() const;
  void setEndCart(unsigned cartnum) const;
  unsigned recStartCart() const;
  void setRecStartCart(unsigned cartnum) const;
  unsigned recEndCart() const;
  void setRecEndCart(unsigned cartnum) const;
  int trimThreshold() const;
  void setTrimThreshold(int level);
  int ripperLevel() const;
  void setRipperLevel(int level);
  RDLogLine::TransType defaultTransType() const;
  void setDefaultTransType(RDLogLine::TransType type);
#ifndef WIN32
  void getSettings(RDSettings *s) const;
#endif  // WIN32
  
 private:
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,bool value) const;
  QString lib_station;
};


#endif 
