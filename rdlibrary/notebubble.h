// notebubble.h
//
// Bubble for displaying a cart note
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef NOTEBUBBLE_H
#define NOTEBUBBLE_H

#include <qlabel.h>
#include <qtimer.h>

class NoteBubble : public QLabel
{
  Q_OBJECT
 public:
  NoteBubble(QWidget *parent=0);
  unsigned cartNumber() const;
  bool setCartNumber(unsigned cartnum);

 private:
  unsigned note_cart_number;
  QTimer *note_show_timer;
};


#endif  // NOTEBUBBLE_H
