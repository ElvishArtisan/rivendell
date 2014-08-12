// rdmeteraverage.h
//
// Average sucessive levels for a meter.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmeteraverage.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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
#ifndef RDMETERAVERAGE_H
#define RDMETERAVERAGE_H

#include <queue>


class RDMeterAverage
{
 public:
  RDMeterAverage(int maxsize);
  double average() const;
  void addValue(double value);

 private:
  int avg_maxsize;
  double avg_total;
  std::queue<double> avg_values;
};


#endif  // RDMETERAVERAGE_H
