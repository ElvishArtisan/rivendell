// rdaudiosettings.cpp
//
// Common Audio Settings
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdaudiosettings.cpp,v 1.1 2007/09/14 14:06:24 fredg Exp $
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

#include <rdaudiosettings.h>


RDAudioSettings::RDAudioSettings()
{
}


RDAudioSettings::Format RDAudioSettings::format() const
{
  return set_format;
}


void RDAudioSettings::setFormat(RDAudioSettings::Format format)
{
  set_format=format;
}


unsigned RDAudioSettings::channels() const
{
  return set_channels;
}


void RDAudioSettings::setChannels(unsigned channels)
{
  set_channels=channels;
}


unsigned RDAudioSettings::sampleRate() const
{
  return set_sample_rate;
}


void RDAudioSettings::setSampleRate(unsigned rate)
{
  set_sample_rate=rate;
}


unsigned RDAudioSettings::bitRate() const
{
  return set_bit_rate;
}


void RDAudioSettings::setBitRate(unsigned rate)
{
  set_bit_rate=rate;
}
