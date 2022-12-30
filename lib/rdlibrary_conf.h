// rdlibrary_conf.h
//
// Abstract RDLibrary Configuration
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdconfig.h>
#include <rdsettings.h>

class RDLibraryConf
{
 public:
  enum RecordMode {Manual=0,Vox=1};
  enum SearchLimit {LimitNo=0,LimitYes=1,LimitPrevious=2};
  enum CdServerType {DummyType=0,CddbType=1,MusicBrainzType=2,LastType=3};
  RDLibraryConf(const QString &station);
  QString station() const;
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
  CdServerType cdServerType() const;
  void setCdServerType(CdServerType type) const;
  QString cddbServer() const;
  void setCddbServer(QString server) const;
  QString mbServer() const;
  void setMbServer(QString server) const;
  bool readIsrc() const;
  void setReadIsrc(bool state) const;
  bool enableEditor() const;
  void setEnableEditor(bool state) const;
  void getSettings(RDSettings *s) const;
  int srcConverter() const;
  void setSrcConverter(int conv) const;
  RDLibraryConf::SearchLimit limitSearch() const;
  void setLimitSearch(RDLibraryConf::SearchLimit lmt) const;
  bool searchLimited() const;
  void setSearchLimited(bool state) const;
  bool isSingleton() const;
  void setIsSingleton(bool state) const;
  static QString cdServerTypeText(CdServerType type);
  static QPixmap cdServerLogo(CdServerType type);

 private:
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,bool value) const;
  QString lib_station;
  unsigned lib_id;
};


#endif  // RDLIBRARY_CONF_H
