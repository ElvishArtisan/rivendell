// rdcut_dialog.h
//
// A widget to select a Rivendell Cut.
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

#ifndef RDCUT_DIALOG_H
#define RDCUT_DIALOG_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QStringList>

#include <rdbusydialog.h>
#include <rdcart.h>
#include <rdcartfilter.h>
#include <rddialog.h>
#include <rdlibrarymodel.h>
#include <rdtreeview.h>

#define RDCUT_DIALOG_STEP_SIZE 1000

class RDCutDialog : public RDDialog
{
 Q_OBJECT
 public:
  RDCutDialog(QString *filter,QString *group,QString *schedcode,bool show_clear,
	      bool allow_add,bool exclude_tracks,const QString &caption,
	      bool user_is_admin,QWidget *parent=0);
  ~RDCutDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *cutname);

 private slots:
  void modelResetData();
  void cartDoubleClickedData(const QModelIndex &index);
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void addButtonData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  QString StateFile();
  void LoadState();
  void SaveState();
  QString *cart_cutname;
  RDCartFilter *cart_cart_filter;
  RDTreeView *cart_cart_view;
  RDLibraryModel *cart_cart_model;
  QPushButton *cart_ok_button;
  QPushButton *cart_cancel_button;
  QPushButton *cart_add_button;
  QPushButton *cart_clear_button;
  QString *cart_filter;
  bool local_filter;
  RDStation::FilterMode cart_filter_mode;
  QProgressDialog *cart_progress_dialog;
  QString cart_import_path;
  QString cart_import_file_filter;
  bool *cart_temp_allowed;
  RDBusyDialog *cart_busy_dialog;
  QString cart_caption;
  bool cart_allow_add;
};


#endif  // RDCUT_DIALOG_H
