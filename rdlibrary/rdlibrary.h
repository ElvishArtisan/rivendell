// rdlibrary.h
//
// Library Utility for Rivendell.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLIBRARY_H
#define RDLIBRARY_H

#include <QCheckBox>
#include <QComboBox>
#include <QProgressDialog>

#include <rdcartfilter.h>
#include <rdlibrarymodel.h>
#include <rdsimpleplayer.h>
#include <rdwidget.h>

#include "disk_gauge.h"
#include "libraryview.h"
#include "mainwindow.h"

#define RDLIBRARY_GEOMETRY_FILE ".rdlibrary"
#define RDLIBRARY_STEP_SIZE 5000

//
// Cut Length Deviation Values
//
#define RDLIBRARY_MID_LENGTH_LIMIT 500
#define RDLIBRARY_MID_LENGTH_COLOR Qt::darkYellow
#define RDLIBRARY_MAX_LENGTH_LIMIT 1500
#define RDLIBRARY_MAX_LENGTH_COLOR Qt::red
#define RDLIBRARY_ENFORCE_LENGTH_COLOR Qt::blue

#define RDLIBRARY_USAGE "[--profile-ripping]\n\n--profile-ripping\n     Print profiling information to stdout when performing rips from\n     optical media.\n\n"

class MainWidget : public MainWindow
{
 Q_OBJECT
 public:
  enum Column {Icon=0,Cart=1,Group=2,Length=3,Talk=4,Title=5,Artist=6,
		Start=7,End=8,Album=9,Label=10,Composer=11,Conductor=12,
		Publisher=13,Client=14,Agency=15,UserDefined=16,
		Cuts=17,LastCutPlayed=18,EnforceLength=19,PreservePitch=20,
		LengthDeviation=21,OwnedBy=22};
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectedGroupChangedData(const QString &grpname);
  void caeConnectedData(bool state);
  void userData();
  void addData();
  void editData();
  void deleteData();
  void macroData();
  void ripData();
  void reportsData();
  void playerShortcutData();
  void cartDoubleClickedData(const QModelIndex &);
  void selectionChangedData(const QItemSelection &,const QItemSelection &);
  void modelResetData();
  void audioChangedData(int state);
  void macroChangedData(int state);
  void dragsChangedData(bool state);
  void notificationReceivedData(RDNotification *notify);
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  
 private:
  void SetCaption(QString user);
  int CurrentSelection(QModelIndexList *carts,QModelIndexList *cuts=NULL) const;
  void SelectRow(const QModelIndex &index);
  void SelectRow(unsigned cartnum);
  QString GeometryFile();
  void LoadGeometry();
  void SaveGeometry();
  void LockUser();
  bool UnlockUser();
  void SendNotification(RDNotification::Action action,unsigned cartnum);
  void SetPlayer(RDCart::Type type);
  RDCartFilter *lib_cart_filter;
  LibraryView *lib_cart_view;
  RDLibraryModel *lib_cart_model;
  QString lib_filter_text;
  QPushButton *lib_add_button;
  QPushButton *lib_edit_button;
  QPushButton *lib_delete_button;
  QPushButton *lib_rip_button;
  QPushButton *lib_reports_button;
  QPushButton *lib_close_button;
  QPushButton *lib_macro_button;
  RDMacroEvent *lib_macro_events;
  RDSimplePlayer *lib_player;
  int lib_output_card;
  int lib_output_port;
  QString lib_default_group;
  QString lib_import_path;
  QProgressDialog *lib_progress_dialog;
  bool profile_ripping;
  bool lib_edit_pending;
  bool lib_user_changed;
  QTimer *lib_user_timer;
  bool lib_resize;
  QList<unsigned> lib_deleted_carts;
};


#endif  // RDLIBRARY_H
