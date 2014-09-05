// rdcart_dialog.h
//
// A widget to select a Rivendell Cart.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcart_dialog.h,v 1.21.4.3 2014/02/11 23:46:25 cvs Exp $
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

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprogressdialog.h>
#include <qcheckbox.h>

#include <rdcae.h>
#include <rdconfig.h>
#include <rdripc.h>
#include <rdsystem.h>
#include <rdsimpleplayer.h>
#include <rdlistviewitem.h>
#include <rdcart.h>
#include <rdstation.h>
#include <rdcombobox.h>
#include <rduser.h>
#include <rdbusydialog.h>

#define RDCART_DIALOG_STEP_SIZE 1000

class RDCartDialog : public QDialog
{
 Q_OBJECT
 public:
  RDCartDialog(QString *filter,QString *group,QString *schedcode,
	       RDCae *cae,RDRipc *ripc,RDStation *station,RDSystem *system,
	       RDConfig *config,QWidget *parent=0);

  ~RDCartDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(int *cartnum,RDCart::Type type,QString *svcname,int svc_quan,
	   const QString &username,const QString &passwd,
	   bool *temp_allowed=NULL);

 private slots:
  void filterChangedData(const QString &);
  void filterSearchedData();
  void filterClearedData();
  void groupActivatedData(const QString &group);
  void schedcodeActivatedData(const QString &schedcode);
  void limitChangedData(int state);
  void clickedData(QListViewItem *item);
  void doubleClickedData(QListViewItem *,const QPoint &,int);
  void editorData();
  void loadFileData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void RefreshCarts();
  void BuildGroupList();
  QString GetSearchFilter(const QString &filter,const QString &group,
			  const QString &schedcode);
  QString StateFile();
  void LoadState();
  void SaveState();
  int *cart_cartnum;
  QLabel *cart_cart_label;
  RDListView *cart_cart_list;
  QLabel *cart_filter_label;
  QLineEdit *cart_filter_edit;
  QLabel *cart_limit_label;
  QCheckBox *cart_limit_box;
  QPushButton *cart_ok_button;
  QPushButton *cart_cancel_button;
  QPushButton *cart_search_button;
  QPushButton *cart_clear_button;
  QPushButton *cart_editor_button;
  QPushButton *cart_file_button;
  QLabel *cart_group_label;
  RDComboBox *cart_group_box;
  QLabel *cart_schedcode_label;
  RDComboBox *cart_schedcode_box;
  QString *cart_filter;
  QString *cart_group;
  QString *cart_schedcode;
  bool local_filter;
  RDCart::Type cart_type;
  QPixmap *cart_playout_map;
  QPixmap *cart_macro_map;
  QString *cart_service;
  int cart_service_quan;
  RDStation::FilterMode cart_filter_mode;
  QProgressDialog *cart_progress_dialog;
  QString cart_import_path;
  QString cart_import_file_filter;
  bool *cart_temp_allowed;
  RDStation *cart_station;
  RDSystem *cart_system;
  RDConfig *cart_config;
  QString cart_user_name;
  QString cart_user_password;
  RDBusyDialog *cart_busy_dialog;
#ifndef WIN32
  RDSimplePlayer *cart_player;
#endif  // WIN32
};


#endif
