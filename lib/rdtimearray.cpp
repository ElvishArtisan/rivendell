// rdtimearray.cpp
//
// Record a sequence of precise points in time with microsecond precision.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/time.h>

#include <QObject>

#include "rdtimearray.h"

RDTimePoint::RDTimePoint(const QString &label)
{
  struct timeval tv;

  gettimeofday(&tv,NULL);
  d_label=label;
  d_usecs=1000000*tv.tv_sec+tv.tv_usec;
}

  
QString RDTimePoint::label() const
{
  return d_label;
}


int64_t RDTimePoint::usecs() const
{
  return d_usecs;
}


QString RDTimePoint::toString() const
{
  return QString::asprintf("%s: %ld",d_label.toUtf8().constData(),d_usecs);
}


int64_t RDTimePoint::operator-(const RDTimePoint &rhs) const
{
  return d_usecs-rhs.d_usecs;
}




RDTimeArray::RDTimeArray()
{
}


int RDTimeArray::size() const
{
  return d_points.size();
}


RDTimePoint RDTimeArray::timePoint(int n) const
{
  return d_points.at(n);
}


void RDTimeArray::addPoint(QString label)
{
  if(label.isEmpty()) {
    label=QObject::tr("point")+QString::asprintf(" %lld",d_points.size());
  }
  d_points.push_back(RDTimePoint(label));
}


QString RDTimeArray::toString(int from,int to) const
{
  QString ret;
  
  if(to<0) {
    to=d_points.size()-1;
  }
  for(int i=from;i<(to+1);i++) {
    ret+=QString::asprintf("%s: %ld\n",
			   d_points.at(i).label().toUtf8().constData(),
			   d_points.at(i).usecs());
  }
  return ret;
}


QString RDTimeArray::offsetsToString(int from,int to) const
{
  QString ret;
  
  if(to<0) {
    to=d_points.size()-1;
  }
  for(int i=from;i<(to+1);i++) {
    ret+=QString::asprintf("%s - %s: %ld\n",
			   d_points.at(i).label().toUtf8().constData(),
			   d_points.at(i-1).label().toUtf8().constData(),
			   d_points.at(i)-d_points.at(i-1));
  }
  return ret;

}


int64_t RDTimeArray::usecsElapsed(int from,int to) const
{
  if(to<0) {
    to=d_points.size()-1;
  }
  return d_points.at(to)-d_points.at(from);
}
