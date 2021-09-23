// presence.cpp
//
// Presence data for a Rivendell station
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>

#include "presence.h"

Presence::Presence(const QString &name,QObject *parent)
  : QObject(parent)
{
  d_name=name.toLower();
  rda->syslog(LOG_DEBUG,"adding presence record for \"%s\"",
	      name.toUtf8().constData());
}


Presence::~Presence()
{
  rda->syslog(LOG_DEBUG,"removing presence record for \"%s\"",
	      d_name.toUtf8().constData());
  if(d_timer!=NULL) {
    delete d_timer;
  }
}


QString Presence::name() const
{
  return d_name;
}


QHostAddress Presence::hostAddress() const
{
  return d_host_address;
}


void Presence::setHostAddress(const QHostAddress &addr)
{
  d_host_address=addr;
}


void Presence::updateLocalId()
{
  if(d_timer!=NULL) {
    delete d_timer;
  }
  d_timer=new QTimer(this);
  d_timer->setSingleShot(true);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
  d_timer->start(10000+(long long)random*10000/RAND_MAX);
}


void Presence::timeoutData()
{
  emit sendLocalId(d_name);
  d_timer->deleteLater();
  d_timer=NULL;
}
