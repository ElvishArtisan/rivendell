//   rdtimeprobe.cpp
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

#include "rdtimeprobe.h"

RDTimeProbeStamp::RDTimeProbeStamp(struct timespec *tp)
{
  d_timespec=*tp;
}


RDTimeProbeStamp::~RDTimeProbeStamp()
{
}


QString RDTimeProbeStamp::toString() const
{
  QDateTime dt=QDateTime::fromMSecsSinceEpoch(1000*(qint64)d_timespec.tv_sec);
  QString microsec=QString::asprintf(".%06ld",d_timespec.tv_nsec);

  return dt.time().toString("hh:mm:ss")+microsec.left(4);
}


double RDTimeProbeStamp::operator-(const RDTimeProbeStamp &other)
{
  double lhs=(double)d_timespec.tv_sec+(double)d_timespec.tv_nsec/1000000000.0;
  double rhs=(double)other.d_timespec.tv_sec+
    (double)other.d_timespec.tv_nsec/1000000000.0;

  return lhs-rhs;
}


RDTimeProbeStamp *RDTimeProbeStamp::currentStamp()
{
  struct timespec now;

  memset(&now,0,sizeof(now));
  clock_gettime(CLOCK_REALTIME,&now);

  return new RDTimeProbeStamp(&now);
}




RDTimeProbe::RDTimeProbe(FILE *out)
{
  d_current_timestamp=NULL;
  d_output_stream=out;

  fprintf(d_output_stream,
	  "================================================================\n");
  fprintf(d_output_stream,"%p: RDTimeProbe created\n",this);
}


RDTimeProbe::~RDTimeProbe()
{
  fprintf(d_output_stream,"%p: RDTimeProbe destroyed\n",this);
  fprintf(d_output_stream,
	  "================================================================\n");
  if(d_current_timestamp!=NULL) {
    delete d_current_timestamp;
  }
}


void RDTimeProbe::printWaypoint(const QString &label)
{
  RDTimeProbeStamp *now=RDTimeProbeStamp::currentStamp();
  double diff=0.0;

  if(d_current_timestamp!=NULL) {
    diff=*now-*d_current_timestamp;
  }
  QString diff_str=QString::asprintf("%18.6lf",diff).trimmed();
  fprintf(d_output_stream,"%p : %s [%s] : %s\n",
	  this,
	  now->toString().toUtf8().constData(),
	  diff_str.toUtf8().constData(),
	  label.toUtf8().constData());
  delete d_current_timestamp;
  d_current_timestamp=now;
}
