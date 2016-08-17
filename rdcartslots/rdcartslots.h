// rdcartslots.h
//
// A Dedicated Cart Slot Utility for Rivendell.
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCARTSLOTS_H
#define RDCARTSLOTS_H

#include <vector>

#include <qwidget.h>
#include <qtimer.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QCloseEvent>

#include <rdstereometer.h>
#include <rdmacro.h>
#include <rdevent_player.h>
#include <rdcartslot.h>
#include <rdcart_dialog.h>
#include <rdslotdialog.h>
#include <rdcueeditdialog.h>
#include <rdlistsvcs.h>

//
// Settings
//
#define MASTER_TIMER_INTERVAL 100
#define METER_INTERVAL 50
#define RDCARTSLOTS_USAGE "\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void userData();
  void rmlReceivedData(RDMacro *rml);

 protected:
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void RunLocalMacros(RDMacro *rml);
  void SetCaption();
  RDEventPlayer *panel_player;
  QPixmap *lib_rivendell_map;
  QString panel_filter;
  QString panel_group;
  QString panel_schedcode;
  std::vector<RDCartSlot *> panel_slots;
  RDCartDialog *panel_cart_dialog;
  RDSlotDialog *panel_slot_dialog;
  RDCueEditDialog *panel_cue_dialog;
  RDListSvcs *panel_svcs_dialog;
};


#endif  // RDCARTSLOTS_H
