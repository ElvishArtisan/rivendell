// rdrange.cpp
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

#include <QStringList>

#include "rdrange.h"

RDRange::RDRange(int max_end)
{
  d_max_end=max_end;
  d_start=0;
  d_end=0;
}


int RDRange::start() const
{
  return d_start;
}


int RDRange::end() const
{
  return d_end;
}


bool RDRange::parse(const QString &str)
{
  QStringList f0=str.split(":",QString::KeepEmptyParts);
  bool ok1=false;
  bool ok2=false;

  switch(f0.size()) {
  case 1:
    d_start=f0.at(0).toInt(&ok1);
    d_end=f0.at(0).toInt(&ok2);
    if((!ok1)||(!ok2)||(d_start<=0)||(d_start>d_max_end)) {
      d_start=0;
      d_end=0;
      return false;
    }
    return true;

  case 2:
    if(f0.at(0).isEmpty()) {
      d_start=1;
      ok1=true;
    }
    else {
      d_start=f0.at(0).toInt(&ok1);
    }
    if(f0.at(1).isEmpty()) {
      d_end=d_max_end;
      ok2=true;
    }
    else {
      d_end=f0.at(1).toInt(&ok2);
    }
    if((!ok1)||(!ok2)||(d_start<=0)||(d_end>d_max_end)||(d_start>d_end)) {
      d_start=0;
      d_end=0;
      return false;
    }
    return true;
  }

  d_start=0;
  d_end=0;
  return false;
}
