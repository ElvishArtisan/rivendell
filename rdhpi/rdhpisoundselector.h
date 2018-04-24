//   rdhpisoundselector.h
//
//   A selection widget for audio devices.
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHPISOUNDSELECTOR_H
#define RDHPISOUNDSELECTOR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <q3listbox.h>

#include <rdhpisoundcard.h>

#ifdef ALSA
#include <alsa/asoundlib.h>
#endif  // HPI

#ifdef JACK
#include <jack/jack.h>
#endif  // JACK

class RDHPISoundSelector : public Q3ListBox
{
  Q_OBJECT

 public:
  RDHPISoundSelector(RDHPISoundCard::DeviceClass dev_class,QWidget *parent=0);

 signals:
  void changed(int card,int port);
  void cardChanged(int card);
  void portChanged(int port);
   
 private slots:
  void selection(int selection);

 private:
  RDHPISoundCard *sound_card;
};


#endif  // RDHPISOUNDSELECTOR_H
