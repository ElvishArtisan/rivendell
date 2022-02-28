// edit_cartevent.h
//
// Edit a Rivendell Netcatch Cart Event
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_CARTEVENT_H
#define EDIT_CARTEVENT_H

#include <QCheckBox>

#include <rdcombobox.h>
#include <rddeck.h>
#include <rddialog.h>
#include <rdcart.h>
#include <rdrecording.h>
#include <rdstationlistmodel.h>

#include "dowselector.h"
#include "eventwidget.h"

class EditCartEvent : public RDDialog
{
 Q_OBJECT
 public:
 EditCartEvent(QWidget *parent=0);
 ~EditCartEvent();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;

 public slots:
  int exec(int id,std::vector<int> *adds);

 private slots:
  void selectCartData();
  void saveasData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  bool CheckEvent(bool include_myself);
  RDDeck *edit_deck;
  RDCart *edit_cart;
  RDRecording *edit_recording;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QString edit_cutname;
  QLabel *edit_destination_label;
  QLineEdit *edit_destination_edit;
  QPushButton *edit_destination_button;
  QCheckBox *edit_oneshot_box;
  QLabel *edit_oneshot_label;
  QPushButton *edit_saveas_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  DowSelector *edit_dow_selector;
  EventWidget *edit_event_widget;
  std::vector<int> *edit_added_events;
};


#endif  // EDIT_CARTEVENT_H
