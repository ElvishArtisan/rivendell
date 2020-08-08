// rddatapacer.cpp
//
// Pace a stream of data messages.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddatapacer.h"

RDDataPacer::RDDataPacer(QObject *parent)
  : QObject(parent)
{
  d_pace_interval=RDDATAPACER_DEFAULT_PACE_INTERVAL;

  d_timer=new QTimer(this);
  d_timer->setSingleShot(true);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
}


RDDataPacer::~RDDataPacer()
{
  delete d_timer;
}


int RDDataPacer::paceInterval() const
{
  return d_pace_interval;
}


void RDDataPacer::setPaceInterval(int msecs)
{
  d_pace_interval=msecs;
}


void RDDataPacer::send(const QByteArray &data)
{
  if(d_timer->isActive()) {
    //
    // Queue it up
    //
    d_data_queue.enqueue(data);
  }
  else {
    //
    // Wake up
    //
    emit dataSent(data);
    d_timer->start(d_pace_interval);
  }
}


void RDDataPacer::timeoutData()
{
  if(d_data_queue.isEmpty()) {
    //
    // Nothing to do, go to sleep
    //
    return;
  }
  emit dataSent(d_data_queue.dequeue());
  d_timer->start(d_pace_interval);
}
