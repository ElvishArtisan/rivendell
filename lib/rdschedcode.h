// rdschedcode.h
//
// Abstract a Rivendell Scheduler Code
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSCHEDCODE_H
#define RDSCHEDCODE_H

class RDSchedCode
{
 public:
  RDSchedCode(const QString &code);
  QString code() const;
  bool exists() const;
  QString description() const;
  void setDescription(const QString &desc) const;
  QString xml() const;

 private:
  void SetRow(const QString &param,const QString &value) const;
  QString sched_code;
};


#endif  // RDSCHEDCODE_H
