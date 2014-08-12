// rdcartslot.h
//
// The cart slot widget.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcartslot.h,v 1.8.2.11 2014/01/07 23:23:17 cvs Exp $
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

#ifndef RDCARTSLOT_H
#define RDCARTSLOT_H

#include <vector>

#include <qwidget.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>

#include <rdpushbutton.h>

#include <rdcart_dialog.h>
#include <rdcart.h>
#include <rduser.h>
#include <rdlog_line.h>
#include <rdplay_deck.h>
#include <rdpanel_button.h>
#include <rdbutton_dialog.h>
#include <rdbutton_panel.h>
#include <rdripc.h>
#include <rdcombobox.h>
#include <rdsvc.h>
#include <rdslotbox.h>
#include <rdslotdialog.h>
#include <rdslotoptions.h>
#include <rdcueeditdialog.h>
#include <rdlistsvcs.h>
#include <rdairplay_conf.h>

class RDCartSlot : public QWidget
{
 Q_OBJECT
 public:
  RDCartSlot(int slotnum,RDRipc *ripc,RDCae *cae,RDStation *station,
	     RDConfig *config,RDListSvcs *svcs_dialog,RDSlotDialog *slot_dialog,
	     RDCartDialog *cart_dialog,RDCueEditDialog *cue_dialog,
	     const QString &caption,RDAirPlayConf *conf,QWidget *parent=0);
  ~RDCartSlot();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setUser(RDUser *user);
  RDSlotOptions *slotOptions() const;
  void updateOptions();
  void setSvcNames(std::vector<QString> *svcnames);
  void setCart(RDCart *cart,int break_len=-1);
  bool load(int cartnum,int break_len=-1);
  void unload();
  bool play();
  bool pause();
  bool stop();
  bool breakAway(unsigned msecs);
  bool pauseEnabled() const;
  void setPauseEnabled(bool state);

 public slots:
  void updateMeters();

 signals:
  void tick();
  void buttonFlash(bool state);
  void selectClicked(unsigned cartnum,int row,int col);

 private slots:
  void startData();
  void doubleClickedData();
  void loadData();
  void optionsData();
  void stateChangedData(int id,RDPlayDeck::State state);
  void positionData(int id,int msecs);
  void hookEndData(int id);
  void timescalingSupportedData(int card,bool state);
  void cartDroppedData(unsigned cartnum);

 private:
  void InitializeOptions();
  unsigned SelectCart(const QString &svcname,unsigned msecs);
  void SetInput(bool state);
  void LogPlayout(RDPlayDeck::State state);
  void ClearTempCart();
  QPushButton *slot_start_button;
  QPushButton *slot_load_button;
  QPushButton *slot_options_button;
  std::vector<QString> *slot_svc_names;
  QString slot_svcname;
  RDLogLine *slot_logline;
  RDSlotOptions *slot_options;
  RDPlayDeck *slot_deck;
  bool slot_pause_enabled;
  bool slot_stop_requested;
  RDUser *slot_user;
  RDSlotBox *slot_box;
  RDRipc *slot_ripc;
  RDCae *slot_cae;
  RDStation *slot_station;
  RDConfig *slot_config;
  RDListSvcs *slot_svcs_dialog;
  RDSlotDialog *slot_slot_dialog;
  RDCartDialog *slot_cart_dialog;
  RDCueEditDialog *slot_cue_dialog;
  QString slot_caption;
  QPalette slot_ready_color;
  QPalette slot_playing_color;
  int slot_number;
  unsigned slot_breakaway_cart;
  int slot_breakaway_length;
  bool slot_timescaling_active;
  bool slot_temp_cart;
  RDAirPlayConf *slot_airplay_conf;
};

#endif  // RDCARTSLOT_H
