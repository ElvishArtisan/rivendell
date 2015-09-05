// driver.cpp
//
// Abstract base class for CAE audio drivers.
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <qdatetime.h>

#include "driver.h"

Driver::Driver(Driver::Type type,RDStation *station,RDConfig *config,
	       QObject *parent)
  : QObject(parent)
{
  dvr_type=type;
  dvr_station=station;
  dvr_config=config;
}


Driver::~Driver()
{
}


Driver::Type Driver::type() const
{
  return dvr_type;
}


bool Driver::connectPorts(const QString &out,const QString &in)
{
  return false;
}


bool Driver::disconnectPorts(const QString &out,const QString &in)
{
  return false;
}


RDStation *Driver::station()
{
  return dvr_station;
}


RDConfig *Driver::config()
{
  return dvr_config;
}


void Driver::logLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *file;

  dvr_config->log("caed",prio,line);

  if(dvr_config->caeLogfile().isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();

  file=fopen(dvr_config->caeLogfile(),"a");
  if(file==NULL) {
    return;
  }
  chmod(dvr_config->caeLogfile(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(file,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(file);
}


QString Driver::typeText(Driver::Type type)
{
  QString ret=tr("Unknown");

  switch(type) {
  case Driver::Alsa:
    ret=tr("Advanced Linux Sound Architecture");
    break;

  case Driver::Hpi:
    ret=tr("AudioScience HPI");
    break;

  case Driver::Jack:
    ret=tr("JACK Audio Connection Kit");
    break;
  }

  return ret;
}
