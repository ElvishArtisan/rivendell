// rdalsacard.h
//
// Abstract ALSA 'card' information
//
//   (C) Copyright 2019-2025 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDALSACARD_H
#define RDALSACARD_H

#ifdef ALSA
#include <alsa/asoundlib.h>

#include <QString>
#include <QStringList>

class RDAlsaCard
{
 public:
  RDAlsaCard(snd_ctl_t *ctl,int index);
  RDAlsaCard(const QString &id,int index);
  int index() const;
  QString id() const;
  QString driver() const;
  QString name() const;
  QString prettyName() const;
  QString longName() const;
  QString prettyLongName() const;
  QString mixerName() const;
  QString prettyMixerName() const;
  bool isEnabled() const;
  void setEnabled(bool state);
  int maxChannelsPerPcm() const;
  int periodFrames() const;
  void setPeriodFrames(int frames);
  int periodQuantity() const;
  void setPeriodQuantity(int n);
  QString dump() const;

 private:
  int card_index;
  QString card_id;
  QString card_driver;
  QString card_name;
  QString card_pretty_name;
  QString card_long_name;
  QString card_pretty_long_name;
  QString card_mixer_name;
  QString card_pretty_mixer_name;
  int card_max_channels_per_pcm;
  int card_period_frames;
  int card_period_quantity;
  bool card_enabled;
};
#endif  // ALSA

#endif  // RDALSACARD_H
