// alsaitem.h
//
// QListBoxItem for ALSA PCM devices. 
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: alsaitem.h,v 1.2 2010/07/29 19:32:39 cvs Exp $
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

#include <qlistbox.h>

class AlsaItem : public QListBoxText
{
 public:
  AlsaItem(QListBox *listbox,const QString &text=QString::null);
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
