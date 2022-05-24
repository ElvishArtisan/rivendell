// rdrange.h
//
// Implement Rivendell range specifications
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRANGE_H
#define RDRANGE_H

#include <QString>

class RDRange
{
 public:
  RDRange(int max_end);
  int start() const;
  int end() const;
  bool parse(const QString &str);

 private:
  int d_start;
  int d_end;
  int d_max_end;
};


#endif  // RDRANGE_H
