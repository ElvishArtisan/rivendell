// rdaudiosettings.h
//
// A container class for audio settings.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdaudiosettings.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDAUDIOSETTINGS_H
#define RDAUDIOSETTINGS_H


class RDAudioSettings
{
  public:
  enum Format {Pcm16=0,Layer1=1,Layer2=2,Layer3=3,OggVorbis=4};
  RDAudioSettings();
  RDAudioSettings::Format format() const;
  void setFormat(RDAudioSettings::Format format);
  unsigned channels() const;
  void setChannels(unsigned channels);
  unsigned sampleRate() const;
  void setSampleRate(unsigned rate);
  unsigned bitRate() const;
  void setBitRate(unsigned rate);

  private:
   RDAudioSettings::Format set_format;
   unsigned set_channels;
   unsigned set_sample_rate;
   unsigned set_bit_rate;
};


#endif  // RDAUDIOSETTINGS_H
