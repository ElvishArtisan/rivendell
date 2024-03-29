// audio_cart.h
//
// The audio cart editor for RDLibrary.
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

#ifndef AUDIO_CART_H
#define AUDIO_CART_H

#include <rdbipushbutton.h>
#include <rdcart.h>
#include <rdcutlistmodel.h>
#include <rdmarkerdialog.h>
#include <rdtableview.h>
#include <rdwidget.h>

#include "audio_controls.h"
#include "globals.h"

extern bool import_active;

class AudioCart : public RDWidget
{
  Q_OBJECT
 public:
  AudioCart(AudioControls *controls,RDCart *cart,QString *path,bool new_cart,
	    bool profile_rip,QWidget *parent=0);
  ~AudioCart();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDCutListModel *cutListModel();

 public slots:
  void changeCutScheduling(int sched);

 private slots:
  void addCutData();
  void deleteCutData();
  void copyCutData();
  void pasteCutData();
  void editCutData();
  void recordCutData();
  void doubleClickedData(const QModelIndex &index);
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void ripCutData();
  void importCutData();
  
 signals:
  void cartDataChanged();
  void audioChanged();

 private:
  void UpdateButtons();
  QModelIndex SingleSelectedLine() const;
  RDCart *rdcart_cart;
  RDTableView *rdcart_cut_view;
  RDCutListModel *rdcart_cut_model;
  unsigned rdcart_average_length;
  QString *rdcart_import_path;
  bool rdcart_new_cart;
  AudioControls *rdcart_controls;
  QPushButton *add_cut_button;
  QPushButton *delete_cut_button;
  QPushButton *copy_cut_button;
  QPushButton *paste_cut_button;
  RDBiPushButton *record_cut_button;
  QPushButton *edit_cut_button;
  QPushButton *import_cut_button;
  QPushButton *rip_cut_button;
  bool rdcart_modification_allowed;
  bool rdcart_import_metadata;
  bool rdcart_profile_rip;
  bool rdcart_use_weighting;
  RDMarkerDialog *rdcart_marker_dialog;
};


#endif  // AUDIO_CART_H
