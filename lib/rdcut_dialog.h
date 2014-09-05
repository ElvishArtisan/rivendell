// rdcut_dialog.h
//
// A widget to select a Rivendell Cut.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcut_dialog.h,v 1.14 2011/08/30 23:35:43 cvs Exp $
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

#ifndef RDCUT_DIALOG_H
#define RDCUT_DIALOG_H

#include <qdialog.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qprogressdialog.h>
#include <qcheckbox.h>

#include <rdlistviewitem.h>
#include <rdstation.h>
#include <rdsystem.h>

#define RDCUT_DIALOG_STEP_SIZE 1000

class RDCutDialog : public QDialog
{
 Q_OBJECT
 public:
  RDCutDialog(QString *cutname,RDStation *station,RDSystem *system,
	      QString *filter=0,QString *group=0,QString *schedcode=NULL,
	      QString username="",bool show_clear=false,bool allow_add=false,
	      bool exclude_tracks=false,
	      QWidget *parent=0,const char *name=0);
  ~RDCutDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec();

 private slots:
  void filterChangedData(const QString &);
  void clearData();
  void groupActivatedData(const QString &);
  void limitChangedData(int state);
  void cartClickedData(QListViewItem *);
  void selectionChangedData();
  void searchButtonData();
  void clearButtonData();
  void addButtonData();
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  
 private:
  void RefreshCarts();
  void RefreshCuts();
  void SelectCut(QString cutname);
  void BuildGroupList();
  QString StateFile();
  void LoadState();
  void SaveState();
  RDListView *cut_cart_list;
  QListView *cut_cut_list;
  QLineEdit *cut_filter_edit;
  QCheckBox *cart_limit_box;
  QPushButton *cut_search_button;
  QPushButton *cut_clear_button;
  QPushButton *cut_ok_button;
  QPushButton *cut_cancel_button;
  QComboBox *cut_group_box;
  QLabel *cut_schedcode_label;
  QComboBox *cut_schedcode_box;
  QString *cut_cutname;
  QString *cut_filter;
  QString *cut_group;
  QString *cut_schedcode;
  bool local_filter;
  QPixmap *cut_playout_map;
  QPixmap *cut_macro_map;
  QString cut_username;
  bool cut_allow_clear;
  bool cut_exclude_tracks;
  RDStation::FilterMode cut_filter_mode;
  RDSystem *cut_system;
  QProgressDialog *cut_progress_dialog;
};


#endif  // RDCUT_DIALOG_H
