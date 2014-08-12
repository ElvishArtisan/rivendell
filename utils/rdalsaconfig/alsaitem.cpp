// alsaitem.cpp
//
// QListBoxItem for ALSA PCM devices. 
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: alsaitem.cpp,v 1.2 2010/07/29 19:32:39 cvs Exp $
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

AlsaItem::AlsaItem(QListBox *listbox,const QString &text)
: QListBoxText(listbox,text)
{
}


AlsaItem::AlsaItem(const QString &text)
  : QListBoxText(text)
{
  alsa_card=-1;
  alsa_device=-1;
}


AlsaItem::AlsaItem(const AlsaItem &item)
{
  setText(item.text());
  setCard(item.card());
  setDevice(item.device());
}


int AlsaItem::card() const
{
  return alsa_card;
}


void AlsaItem::setCard(int card)
{
  alsa_card=card;
}


int AlsaItem::device() const
{
  return alsa_device;
}


void AlsaItem::setDevice(int device)
{
  alsa_device=device;
}
