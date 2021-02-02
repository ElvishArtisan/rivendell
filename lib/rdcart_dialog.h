// rdcart_dialog.h
//
// A widget to select a Rivendell Cart.
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

#ifndef RDCART_DIALOG_H
#define RDCART_DIALOG_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QStringList>

#include <rdbusydialog.h>
#include <rdcart.h>
#include <rddialog.h>
#include <rdcartfilter.h>
#include <rdlibrarymodel.h>
#include <rdtableview.h>
#include <rdsimpleplayer.h>

#define RDCART_DIALOG_STEP_SIZE 1000

class RDCartDialog : public RDDialog
{
 Q_OBJECT
 public:
  RDCartDialog(QString *filter,QString *group,QString *schedcode,
	       const QString &caption,bool user_is_admin,QWidget *parent=0);
  ~RDCartDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(int *cartnum,RDCart::Type type,const QString &svc,
	   bool *temp_allowed);

 private slots:
  void modelResetData();
  void clickedData(Q3ListViewItem *item);
  void cartDoubleClickedData(const QModelIndex &index);
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void editorData();
  void loadFileData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  QString StateFile();
  void LoadState();
  void SaveState();
  int *cart_cartnum;
  RDCartFilter *cart_cart_filter;
  RDTableView *cart_cart_view;
  RDLibraryModel *cart_cart_model;
  QPushButton *cart_ok_button;
  QPushButton *cart_cancel_button;
  QPushButton *cart_editor_button;
  QPushButton *cart_file_button;
  QString *cart_filter;
  bool local_filter;
  RDCart::Type cart_type;
  QStringList cart_services;
  RDStation::FilterMode cart_filter_mode;
  QProgressDialog *cart_progress_dialog;
  QString cart_import_path;
  QString cart_import_file_filter;
  bool *cart_temp_allowed;
  RDBusyDialog *cart_busy_dialog;
  RDSimplePlayer *cart_player;
  QString cart_caption;
};


#endif  // RDCART_DIALOG_H
