// rdencoder.h
//
// Abstract a Rivendell Custom Encoder
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdencoder.h,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDENCODER_H
#define RDENCODER_H

#include <vector>

#include <qstring.h>

class RDEncoder
{
 public:
  RDEncoder();
  QString name() const;
  void setName(const QString &str);
  int id() const;
  void setId(int id);
  QString commandLine() const;
  void setCommandLine(const QString &str);
  QString defaultExtension() const;
  void setDefaultExtension(const QString &str);
  int allowedChannelsQuantity() const;
  int allowedChannel(unsigned n);
  void addAllowedChannel(int val);
  int allowedSampleratesQuantity() const;
  int allowedSamplerate(unsigned n);
  void addAllowedSamplerate(int val);
  int allowedBitratesQuantity() const;
  int allowedBitrate(unsigned n);
  void addAllowedBitrate(int val);
  void clear();

 private:
  QString encoder_name;
  int encoder_id;
  QString encoder_command_line;
  QString encoder_default_extension;
  std::vector<int> encoder_channels;
  std::vector<int> encoder_samplerates;
  std::vector<int> encoder_bitrates;
};


#endif  // RDENCODER_H
