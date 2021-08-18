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

#include "caedriverfactory.h"
#include "hpidriver.h"

CaeDriver *CaeDriverFactory(RDStation::AudioDriver dvr,QObject *parent)
{
  CaeDriver *ret=NULL;
  printf("HERE1\n");
  switch(dvr) {
  case RDStation::Hpi:
  printf("HERE2\n");
    ret=new HpiDriver(parent);
    break;

  case RDStation::Jack:
  case RDStation::Alsa:
  case RDStation::None:
  printf("HERE3\n");
    break;
  }
  printf("HERE4: %p\n",ret);

  return ret;
}
