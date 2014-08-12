// rddropbox.h
//
// Abstract a Rivendell Dropbox Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddropbox.h,v 1.6.8.1 2013/12/11 20:17:13 cvs Exp $
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


#ifndef RDDROPBOX_H
#define RDDROPBOX_H

class RDDropbox
{
 public:
  RDDropbox(int id,const QString &stationname="");
  int id() const;
  QString stationName() const;
  void setStationName(const QString &name) const;
  QString groupName() const;
  void setGroupName(const QString &name) const;
  QString path() const;
  void setPath(const QString &path) const;
  int normalizationLevel() const;
  void setNormalizationLevel(int lvl) const;
  int autotrimLevel() const;
  void setAutotrimLevel(int lvl) const;
  bool singleCart() const;
  void setSingleCart(bool state) const;
  unsigned toCart() const;
  void setToCart(unsigned cart) const;
  bool useCartchunkId() const;
  void setUseCartchunkId(bool state) const;
  bool titleFromCartchunkId() const;
  void setTitleFromCartchunkId(bool state) const;
  bool deleteCuts() const;
  void setDeleteCuts(bool state) const;
  bool deleteSource() const;
  void setDeleteSource(bool state) const;
  QString metadataPattern() const;
  void setMetadataPattern(const QString &str) const;
  QString userDefined() const;
  void setUserDefined(const QString &str) const;
  int startdateOffset() const;
  void setStartdateOffset(int offset) const;
  int enddateOffset() const;
  void setEnddateOffset(int offset) const;
  bool fixBrokenFormats() const;
  void setFixBrokenFormats(bool state) const;
  QString logPath() const;
  void setLogPath(const QString &path) const;
  bool createDates() const;
  void setCreateDates(bool state) const;
  int createStartdateOffset() const;
  void setCreateStartdateOffset(int offset) const;
  int createEnddateOffset() const;
  void setCreateEnddateOffset(int offset) const;

 private:
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,unsigned value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,bool value) const;
  int box_id;
};


#endif  // RDDROPBOX_H
