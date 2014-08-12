// loaddrivers.cpp
//
// Load Switcher drivers for ripcd(8)
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: loaddrivers.cpp,v 1.1.8.9 2014/02/17 02:19:03 cvs Exp $
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

#include <stdlib.h>

#include <ripcd.h>
#include <switcher.h>
#include <globals.h>
#include <acu1p.h>
#include <am16.h>
#include <bt10x1.h>
#include <bt16x1.h>
#include <bt16x2.h>
#include <bt8x2.h>
#include <btacs82.h>
#include <btsentinel4web.h>
#include <btsrc16.h>
#include <btsrc8iii.h>
#include <btss124.h>
#include <btss164.h>
#include <btss42.h>
#include <btss44.h>
#include <btss82.h>
#include <harlond.h>
#include <livewire_lwrpaudio.h>
#include <livewire_lwrpgpio.h>
#include <livewire_mcastgpio.h>
#include <local_audio.h>
#include <local_gpio.h>
#include <quartz1.h>
#include <sas32000.h>
#include <sas64000.h>
#include <sas64000gpi.h>
#include <sasusi.h>
#include <starguide3.h>
#include <unity4000.h>
#include <vguest.h>

bool MainObject::LoadSwitchDriver(int matrix_num)
{
  RDMatrix *matrix=new RDMatrix(rdstation->name(),matrix_num);

  switch(matrix->type()) {
  case RDMatrix::Acu1p:
    ripcd_switcher[matrix_num]=new Acu1p(matrix,this);
    break;

  case RDMatrix::Am16:
    ripcd_switcher[matrix_num]=new Am16(matrix,this);
    break;

  case RDMatrix::Bt10x1:
    ripcd_switcher[matrix_num]=new Bt10x1(matrix,this);
    break;
    
  case RDMatrix::Bt16x1:
    ripcd_switcher[matrix_num]=new Bt16x1(matrix,this);
    break;

  case RDMatrix::Bt16x2:
    ripcd_switcher[matrix_num]=new Bt16x2(matrix,this);
    break;

  case RDMatrix::Bt8x2:
    ripcd_switcher[matrix_num]=new Bt8x2(matrix,this);
    break;

  case RDMatrix::BtAcs82:
    ripcd_switcher[matrix_num]=new BtAcs82(matrix,this);
    break;

  case RDMatrix::BtSentinel4Web:
    ripcd_switcher[matrix_num]=new BtSentinel4Web(matrix,this);
    break;

  case RDMatrix::BtSrc16:
    ripcd_switcher[matrix_num]=new BtSrc16(matrix,this);
    break;

  case RDMatrix::BtSrc8III:
    ripcd_switcher[matrix_num]=new BtSrc8Iii(matrix,this);
    break;

  case RDMatrix::BtSs124:
    ripcd_switcher[matrix_num]=new BtSs124(matrix,this);
    break;

  case RDMatrix::BtSs164:
    ripcd_switcher[matrix_num]=new BtSs164(matrix,this);
    break;

  case RDMatrix::BtSs42:
    ripcd_switcher[matrix_num]=new BtSs42(matrix,this);
    break;

  case RDMatrix::BtSs44:
    ripcd_switcher[matrix_num]=new BtSs44(matrix,this);
    break;

  case RDMatrix::BtSs82:
    ripcd_switcher[matrix_num]=new BtSs82(matrix,this);
    break;

  case RDMatrix::Harlond:
    ripcd_switcher[matrix_num]=new Harlond(matrix,this);
    break;

  case RDMatrix::LiveWireLwrpAudio:
    ripcd_switcher[matrix_num]=new LiveWireLwrpAudio(matrix,this);
    break;

  case RDMatrix::LiveWireMcastGpio:
    ripcd_switcher[matrix_num]=new LiveWireMcastGpio(matrix,this);
    break;

  case RDMatrix::LiveWireLwrpGpio:
    ripcd_switcher[matrix_num]=new LiveWireLwrpGpio(matrix,this);
    break;

  case RDMatrix::LocalAudioAdapter:
    ripcd_switcher[matrix_num]=new LocalAudio(matrix,this);
    break;

  case RDMatrix::LocalGpio:
    ripcd_switcher[matrix_num]=new LocalGpio(matrix,this);
    break;

  case RDMatrix::LogitekVguest:
    ripcd_switcher[matrix_num]=new VGuest(matrix,this);
    break;

  case RDMatrix::Quartz1:
    ripcd_switcher[matrix_num]=new Quartz1(matrix,this);
    break;

  case RDMatrix::Sas32000:
    ripcd_switcher[matrix_num]=new Sas32000(matrix,this);
    break;

  case RDMatrix::Sas64000:
    ripcd_switcher[matrix_num]=new Sas64000(matrix,this);
    break;

  case RDMatrix::Sas64000Gpi:
    ripcd_switcher[matrix_num]=new Sas64000Gpi(matrix,this);
    break;

  case RDMatrix::SasUsi:
    ripcd_switcher[matrix_num]=new SasUsi(matrix,this);
    break;

  case RDMatrix::StarGuideIII:
    ripcd_switcher[matrix_num]=new StarGuide3(matrix,this);
    break;

  case RDMatrix::Unity4000:
    ripcd_switcher[matrix_num]=new Unity4000(matrix,this);
    break;

  default:
    ripcd_switcher[matrix_num]=NULL;
    delete matrix;
    return false;
  }
  if(ripcd_switcher[matrix_num]->primaryTtyActive()) {
    ripcd_switcher_tty[matrix_num][0]=
      matrix->port(RDMatrix::Primary);
    ripcd_switcher_tty[matrix_num][0]=-1;
    ripcd_tty_inuse[matrix->port(RDMatrix::Primary)]=true;
  }
  if(ripcd_switcher[matrix_num]->secondaryTtyActive()) {
    ripcd_switcher_tty[matrix_num][1]=
      matrix->port(RDMatrix::Primary);
    ripcd_switcher_tty[matrix_num][1]=-1;
    ripcd_tty_inuse[matrix->port(RDMatrix::Backup)]=true;
  }
  connect(ripcd_switcher[matrix_num],SIGNAL(rmlEcho(RDMacro *)),
	  this,SLOT(sendRml(RDMacro *)));
  connect(ripcd_switcher[matrix_num],
	  SIGNAL(gpiChanged(int,int,bool)),
	  this,SLOT(gpiChangedData(int,int,bool)));
  connect(ripcd_switcher[matrix_num],
	  SIGNAL(gpoChanged(int,int,bool)),
	  this,SLOT(gpoChangedData(int,int,bool)));
  connect(ripcd_switcher[matrix_num],
	  SIGNAL(gpiState(int,unsigned,bool)),
	  this,SLOT(gpiStateData(int,unsigned,bool)));
  connect(ripcd_switcher[matrix_num],
	  SIGNAL(gpoState(int,unsigned,bool)),
	  this,SLOT(gpoStateData(int,unsigned,bool)));
  delete matrix;

  return true;
}
