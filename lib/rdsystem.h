// rdsystem.h
//
// System-wide Rivendell settings
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsystem.h,v 1.3.8.1 2012/11/26 20:19:37 cvs Exp $
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

#ifndef RDSYSTEM_H
#define RDSYSTEM_H

#include <qvariant.h>

class RDSystem
{
 public:
  RDSystem();
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate) const;
  bool allowDuplicateCartTitles() const;
  void setAllowDuplicateCartTitles(bool state) const;
  unsigned maxPostLength() const;
  void setMaxPostLength(unsigned bytes) const;
  QString isciXreferencePath() const;
  void setIsciXreferencePath(const QString &str) const;
  QString tempCartGroup() const;
  void setTempCartGroup(const QString &str) const;

 private:
  QVariant GetValue(const QString &field) const;
  void SetRow(const QString &param,QString value) const;
  void SetRow(const QString &param,int value) const;
};


#endif  // RDSYSTEM_H
