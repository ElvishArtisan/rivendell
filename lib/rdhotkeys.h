// rdlogedit_conf.h
//
// Abstract RDHotkeys Configuration
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHOTKEYS_H
#define RDHOTKEYS_H

#include <qsqldatabase.h>

class RDHotkeys
{
 public:
  RDHotkeys(const QString &station,const QString &module);
  QString station() const;
  int inputCard() const;
  QString GetRowLabel(const QString &station,const QString &value,const QString &module) const;

 private:
  void InsertHotkeys() const;
  QString station_hotkeys;
  QString module_name;
};


#endif  // RDHOTKEYS_H
