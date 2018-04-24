// rdhostvariable.h
//
// Abstract a Rivendell Host Variable
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHOSTVARIABLE_H
#define RDHOSTVARIABLE_H

#include <QString>

class RDHostVariable
{
 public:
  RDHostVariable(const QString &stationname,const QString &name);
  RDHostVariable(int id);
  int id();
  QString name() const;
  bool exists() const;
  QString value() const;
  void setValue(const QString &str);
  QString remarks() const;
  void setRemarks(const QString &str);
  static int create(const QString &stationname,const QString &name);
  static void remove(int id);
  static bool exists(const QString &stationname,const QString &name);
  static bool exists(int id);

 private:
  void SetRow(const QString &param,const QString &value) const;
  int var_id;
  QString var_name;
};


#endif  // RDHOSTVARIABLE_H
