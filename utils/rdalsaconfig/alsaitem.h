// alsaitem.h
//
// QListBoxItem for ALSA PCM devices. 
//
//   (C) Copyright 2009-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ALSAITEM_H
#define ALSAITEM_H

#include <QListWidget>

class AlsaItem : public QListWidgetItem
{
 public:
  AlsaItem(const QString &text);
  AlsaItem(const AlsaItem &item);
  int cardNumber() const;
  void setCardNumber(int cardnum);
  int pcmNumber() const;
  void setPcmNumber(int pcmnum);

 private:
  int alsa_card_number;
  int alsa_pcm_number;
};


#endif  // ALSAITEM_H
