// rddiscimport.h
//
// A Qt-based application for importing TM Century GoldDisc CDs
//
//   (C) Copyright 2013-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDISCIMPORT_H
#define RDDISCIMPORT_H


#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qspinbox.h>

#include <rdaudioimport.h>
#include <rdcdplayer.h>
#include <rdcdripper.h>
#include <rdgroup.h>
#include <rdlistview.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#include "metalibrary.h"

#define RDDISCIMPORT_USAGE "\n"

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void indexFileSelectedData();
  void groupActivatedData(int);
  void autotrimCheckData(bool state);
  void trackDoubleClickedData(Q3ListViewItem *item,const QPoint &pt,int row);
  void ripData();
  void normalizeCheckData(bool state);
  void mediaChangedData();
  void discIdChangedData(const QString &str);
  void ejectData();
  void userChangedData();
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void LockGui(bool state);
  void SetCaption();
  void LoadConfig();
  void SaveConfig();
  QLabel *dg_indexfile_label;
  QLineEdit *dg_indexfile_edit;
  QPushButton *dg_indexfile_button;
  QLabel *dg_group_label;
  QComboBox *dg_group_box;
  QLabel *dg_userdef_label;
  QLineEdit *dg_userdef_edit;
  RDListView *dg_track_list;
  QLabel *dg_disc_label;
  QProgressBar *dg_disc_bar;
  QLabel *dg_track_label;
  QProgressBar *dg_track_bar;
  QLabel *dg_discid_label;
  QLineEdit *dg_discid_edit;
  QPushButton *dg_rip_button;
  QLabel *dg_channels_label;
  QComboBox *dg_channels_box;
  QLabel *dg_autotrim_label;
  QCheckBox *dg_autotrim_box;
  QSpinBox *dg_autotrim_spin;
  QLabel *dg_autotrim_unit;
  QLabel *dg_normalize_label;
  QCheckBox *dg_normalize_box;
  QSpinBox *dg_normalize_spin;
  QLabel *dg_normalize_unit;
  RDTransportButton *dg_eject_button;
  QPushButton *dg_close_button;
  MetaLibrary *dg_metalibrary;
  RDCdPlayer *dg_player;
  RDCdRipper *dg_ripper;
  RDAudioImport *dg_importer;
  RDGroup *dg_group;
  QString dg_group_name;
  QString dg_tempfile;
  std::vector<bool> dg_rip_enableds;
  bool dg_resize;
};


#endif  // RDDISCIMPORT_H
