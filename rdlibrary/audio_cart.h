// audio_cart.h
//
// The audio cart editor for RDLibrary.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <vector>

#include <qwidget.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qcombobox.h>
#include <q3textedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <q3progressdialog.h>

#include <rdcart.h>
#include <rdlibrary_conf.h>
#include <rdlistview.h>
#include <rdlistviewitem.h>

#include <globals.h>
#include <audio_controls.h>

extern bool import_active;

class AudioCart : public QWidget
{
  Q_OBJECT
 public:
  AudioCart(AudioControls *controls,RDCart *cart,QString *path,bool select_cut,
	    bool profile_rip,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void changeCutScheduling(int sched);

 private slots:
  void addCutData();
  void deleteCutData();
  void copyCutData();
  void pasteCutData();
  void editCutData();
  void recordCutData();
  void ripCutData();
  void importCutData();

  /**
   * fork() a child process and start an external audio application to open a
   * cut of audio.
   **/
  void extEditorCutData();
  void doubleClickedData(Q3ListViewItem *,const QPoint &,int);
  void copyProgressData(const QVariant &step);
  
 signals:
  void cartDataChanged();
  void audioChanged();

 private:
  RDListViewItem *SelectedCuts(std::vector<QString> *cutnames);
  void RefreshList();
  void RefreshLine(RDListViewItem *item);
  unsigned NextCut();
  RDCart *rdcart_cart;
  RDListView *rdcart_cut_list;
  unsigned rdcart_average_length;
  QString *rdcart_import_path;
  bool rdcart_select_cut;
  AudioControls *rdcart_controls;
  QPushButton *paste_cut_button;
  Q3ProgressDialog *rdcart_progress_dialog;
  bool rdcart_modification_allowed;
  bool rdcart_import_metadata;
  bool rdcart_profile_rip;
  bool rdcart_use_weighting;
};


#endif

