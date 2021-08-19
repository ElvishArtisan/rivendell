// caedriverfactory.xpp
//
// Create CaeDriver instances.
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

#include "alsadriver.h"
#include "caedriverfactory.h"
#include "hpidriver.h"
#include "jackdriver.h"

CaeDriver *CaeDriverFactory(RDStation::AudioDriver dvr,QObject *parent)
{
  CaeDriver *ret=NULL;

  switch(dvr) {
  case RDStation::Hpi:
    ret=new HpiDriver(parent);
    break;

  case RDStation::Jack:
    ret=new JackDriver(parent);
    break;

  case RDStation::Alsa:
    ret=new AlsaDriver(parent);
    break;

  case RDStation::None:
    break;
  }

  return ret;
}
