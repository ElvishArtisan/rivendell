// alsaitem.h
//
// QListBoxItem for ALSA PCM devices. 
//
//   (C) Copyright 2009-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3listbox.h>

class AlsaItem : public Q3ListBoxText
{
 public:
  AlsaItem(Q3ListBox *listbox,const QString &text=QString::null);
  AlsaItem(const QString &text=QString::null);
  AlsaItem(const AlsaItem &item);
  int card() const;
  void setCard(int card);
  int device() const;
  void setDevice(int device);

 private:
  int alsa_card;
  int alsa_device;
};


#endif  // ALSAITEM_H
