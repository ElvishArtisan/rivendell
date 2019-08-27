// alsaitem.cpp
//
// QListBoxItem for ALSA PCM devices. 
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <alsaitem.h>

AlsaItem::AlsaItem(Q3ListBox *listbox,const QString &text)
: Q3ListBoxText(listbox,text)
{
}


AlsaItem::AlsaItem(const QString &text)
  : Q3ListBoxText(text)
{
  alsa_card_number=-1;
  alsa_pcm_number=-1;
}


AlsaItem::AlsaItem(const AlsaItem &item)
{
  setCardNumber(item.cardNumber());
  setPcmNumber(item.pcmNumber());
}


int AlsaItem::cardNumber() const
{
  return alsa_card_number;
}


void AlsaItem::setCardNumber(int cardnum)
{
  alsa_card_number=cardnum;
}


int AlsaItem::pcmNumber() const
{
  return alsa_pcm_number;
}


void AlsaItem::setPcmNumber(int pcmnum)
{
  alsa_pcm_number=pcmnum;
}
