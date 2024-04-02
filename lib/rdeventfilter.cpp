// rdeventfilter.cpp
//
// Filter one or more window system events
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QKeyEvent>

#include <stdio.h>

#include "rdeventfilter.h"

RDEventFilter::RDEventFilter(QObject *parent)
{
}


QList<QEvent::Type> RDEventFilter::filterList() const
{
  return d_filter_types;
}


void RDEventFilter::addFilter(QEvent::Type type)
{
  if(!d_filter_types.contains(type)) {
    d_filter_types.push_back(type);
  }
}


void RDEventFilter::removeFilter(QEvent::Type type)
{
  d_filter_types.removeAll(type);
}


bool RDEventFilter::eventFilter(QObject *obj,QEvent *e)
{
  if(d_filter_types.contains(e->type())) {
    // Block it
    return true;
  }
  return QObject::eventFilter(obj,e);
}
