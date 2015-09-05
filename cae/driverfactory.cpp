// driverfactory.cpp
//
// Instantiate CAE audio drivers
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include "driver_hpi.h"

#include "driverfactory.h"

Driver *DriverFactory(Driver::Type type,RDStation *station,RDConfig *config,
		      QObject *parent)
{
  Driver *dvr=NULL;

  switch(type) {
  case Driver::Alsa:
    break;

  case Driver::Hpi:
#ifdef HPI
    dvr=new HpiDriver(station,config,parent);
#endif  // HPI
    break;

  case Driver::Jack:
    break;
  }

  return dvr;
}
