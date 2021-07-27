// edit_playout.h
//
// Edit a Rivendell RDCatch Playout
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>/
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

#ifndef EDIT_PLAYOUT_H
#define EDIT_PLAYOUT_H

#include <QCheckBox>
#include <QDateTimeEdit>

#include <rdcombobox.h>
#include <rdcut_dialog.h>
#include <rddeck.h>
#include <rddialog.h>
#include <rdrecording.h>
#include <rdstationlistmodel.h>

#include "eventwidget.h"
#include "dowselector.h"

class EditPlayout : public RDDialog
{
 Q_OBJECT
 public:
  EditPlayout(int id,std::vector<int> *adds,QString *filter,QWidget *parent=0);
  ~EditPlayout();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void locationChangedData(const QString &station,int decknum);
  void selectCutData();
  void saveasData();
  void okData();
  void cancelData();
  
 protected:
  void keyPressEvent(QKeyEvent *);
  void closeEvent(QCloseEvent *e);
  
 private:
  void Save();
  RDDeck *edit_deck;
  RDCutDialog *edit_cut_dialog;
  RDRecording *edit_recording;
  EventWidget *edit_event_widget;
  DowSelector *edit_dow_selector;
  QLineEdit *edit_description_edit;
  QString edit_cutname;
  QLineEdit *edit_destination_edit;
  QCheckBox *edit_oneshot_box;
  std::vector<int> *edit_added_events;
  QString *edit_filter;
};


#endif  // EDIT_PLAYOUT_H
