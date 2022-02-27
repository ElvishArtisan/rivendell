//   rdtimeprobe.h
//
//   Report elapsed times for profiling and optimization
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDTIMEPROBE_H
#define RDTIMEPROBE_H

#include <stdio.h>
#include <time.h>

#include <QDateTime>
#include <QString>

class RDTimeProbeStamp
{
 public:
  RDTimeProbeStamp(struct timespec *tp=NULL);
  ~RDTimeProbeStamp();
  QString toString() const;
  double operator-(const RDTimeProbeStamp &other);
  static RDTimeProbeStamp *currentStamp();

 private:
  struct timespec d_timespec;
};




class RDTimeProbe
{
 public:
  RDTimeProbe(FILE *out=stderr);
  ~RDTimeProbe();
  void printWaypoint(const QString &label);

 private:
  RDTimeProbeStamp *d_current_timestamp;
  FILE *d_output_stream;
};


#endif  // RDTIMEPROBE_H
