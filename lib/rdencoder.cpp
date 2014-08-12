// rdencoder.cpp
//
// Abstract a Rivendell Custom Encoder
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdencoder.cpp,v 1.1 2008/09/18 19:02:07 fredg Exp $
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

#include <rdencoder.h>

RDEncoder::RDEncoder()
{
  clear();
}


QString RDEncoder::name() const
{
  return encoder_name;
}


void RDEncoder::setName(const QString &str)
{
  encoder_name=str;
}


int RDEncoder::id() const
{
  return encoder_id;
}


void RDEncoder::setId(int id)
{
  encoder_id=id;
}


QString RDEncoder::commandLine() const
{
  return encoder_command_line;
}


void RDEncoder::setCommandLine(const QString &str)
{
  encoder_command_line=str;
}


QString RDEncoder::defaultExtension() const
{
  return encoder_default_extension;
}


void RDEncoder::setDefaultExtension(const QString &str)
{
  encoder_default_extension=str;
}


int RDEncoder::allowedChannelsQuantity() const
{
  return encoder_channels.size();
}


int RDEncoder::allowedChannel(unsigned n)
{
  return encoder_channels[n];
}


void RDEncoder::addAllowedChannel(int val)
{
  encoder_channels.push_back(val);
}


int RDEncoder::allowedSampleratesQuantity() const
{
  return encoder_samplerates.size();
}


int RDEncoder::allowedSamplerate(unsigned n)
{
  return encoder_samplerates[n];
}


void RDEncoder::addAllowedSamplerate(int val)
{
  encoder_samplerates.push_back(val);
}


int RDEncoder::allowedBitratesQuantity() const
{
  return encoder_bitrates.size();
}


int RDEncoder::allowedBitrate(unsigned n)
{
  return encoder_bitrates[n];
}


void RDEncoder::addAllowedBitrate(int val)
{
  encoder_bitrates.push_back(val);
}


void RDEncoder::clear()
{
  encoder_name="";
  encoder_id=-1;
  encoder_command_line="";
  encoder_default_extension="";
  encoder_channels.clear();
  encoder_samplerates.clear();
  encoder_bitrates.clear();
}
