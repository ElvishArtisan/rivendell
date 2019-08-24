// rdalsacard.h
//
// Abstract ALSA 'card' information
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <alsa/asoundlib.h>

#include <qstring.h>
#include <qstringlist.h>

class RDAlsaCard
{
 public:
  RDAlsaCard(snd_ctl_t *ctl,int index);
  int index() const;
  QString id() const;
  QString driver() const;
  QString name() const;
  QString longName() const;
  QString mixerName() const;
  int pcmQuantity() const;
  QString pcmName(int n) const;
  QString dump() const;

 private:
  int card_index;
  QString card_id;
  QString card_driver;
  QString card_name;
  QString card_long_name;
  QString card_mixer_name;
  QStringList card_pcm_names;
};


#endif  // RDALSACARD_H
