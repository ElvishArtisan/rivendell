// rdlibrary_conf.h
//
// Abstract RDLibrary Configuration
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlibrary_conf.h,v 1.22.8.1 2014/01/09 01:03:54 cvs Exp $
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

#ifndef RDLIBRARY_CONF_H
#define RDLIBRARY_CONF_H

#include <qsqldatabase.h>

#include <rdsettings.h>

class RDLibraryConf
{
 public:
  enum RecordMode {Manual=0,Vox=1};
  enum SearchLimit {LimitNo=0,LimitYes=1,LimitPrevious=2};
  RDLibraryConf(const QString &station,unsigned instance);
  QString station() const;
  unsigned instance() const;
  int inputCard() const;
  void setInputCard(int input) const;
  int inputPort() const;
  void setInputPort(int input) const;
  int outputCard() const;
  void setOutputCard(int output) const;
  int outputPort() const;
  void setOutputPort(int input) const;
  int voxThreshold() const;
  void setVoxThreshold(int level) const;
  int trimThreshold() const;
  void setTrimThreshold(int level) const;
  unsigned defaultFormat() const;
  void setDefaultFormat(unsigned format) const;
  unsigned defaultChannels() const;
  void setDefaultChannels(unsigned chans) const;
  unsigned defaultLayer() const;
  void setDefaultLayer(unsigned layer) const;
  unsigned defaultBitrate() const;
  void setDefaultBitrate(unsigned rate) const;
  RDLibraryConf::RecordMode defaultRecordMode() const;
  void setDefaultRecordMode(RecordMode mode) const;
  bool defaultTrimState() const;
  void setDefaultTrimState(bool state) const;
  unsigned maxLength() const;
  void setMaxLength(unsigned length) const;
  unsigned tailPreroll() const;
  void setTailPreroll(unsigned length) const;
  QString ripperDevice() const;
  void setRipperDevice(QString dev) const;
  int paranoiaLevel() const;
  void setParanoiaLevel(int level) const;
  int ripperLevel() const;
  void setRipperLevel(int level) const;
  QString cddbServer() const;
  void setCddbServer(QString server) const;
  bool enableEditor() const;
  void setEnableEditor(bool state) const;
  void getSettings(RDSettings *s) const;
  int srcConverter() const;
  void setSrcConverter(int conv) const;
  RDLibraryConf::SearchLimit limitSearch() const;
  void setLimitSearch(RDLibraryConf::SearchLimit lmt) const;
  bool searchLimited() const;
  void setSearchLimited(bool state) const;

 private:
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,bool value) const;
  QString lib_station;
  unsigned lib_instance;
  unsigned lib_id;
};


#endif 
