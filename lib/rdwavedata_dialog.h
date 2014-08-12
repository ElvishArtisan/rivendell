// rdwavedata_dialog.h
//
// A dialog to edit the contents of an RDWaveData.
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdwavedata_dialog.h,v 1.1.2.1 2014/05/28 21:21:41 cvs Exp $
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

#ifndef RDWAVEDATA_DIALOG_H
#define RDWAVEDATA_DIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qspinbox.h>

#include <rdcombobox.h>
#include <rdcart.h>
#include <rdschedcodes_dialog.h>

class RDWaveDataDialog : public QDialog
{
 Q_OBJECT
 public:
  RDWaveDataDialog(const QString &caption,QWidget *parent=0);
  ~RDWaveDataDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDWaveData *data);

 private slots:
  void schedClickedData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  QLabel *wave_title_label;
  QLineEdit *wave_title_edit;
  QLabel *wave_artist_label;
  QLineEdit *wave_artist_edit;
  QLabel *wave_year_label;
  QLineEdit *wave_year_edit;
  QLabel *wave_usage_label;
  QComboBox *wave_usage_box;
  QPushButton *wave_sched_button;
  QLabel *wave_songid_label;
  QLineEdit *wave_songid_edit;
  QLabel *wave_bpm_label;
  QSpinBox *wave_bpm_spin;
  QLabel *wave_album_label;
  QLineEdit *wave_album_edit;
  QLabel *wave_label_label;
  QLineEdit *wave_label_edit;
  QLabel *wave_client_label;
  QLineEdit *wave_client_edit;
  QLabel *wave_agency_label;
  QLineEdit *wave_agency_edit;
  QLabel *wave_publisher_label;
  QLineEdit *wave_publisher_edit;
  QLabel *wave_composer_label;
  QLineEdit *wave_composer_edit;
  QLabel *wave_conductor_label;
  QLineEdit *wave_conductor_edit;
  QLabel *wave_userdef_label;
  QLineEdit *wave_userdef_edit;
  QPushButton *wave_ok_button;
  QPushButton *wave_cancel_button;
  QString wave_caption;
  RDSchedCodesDialog *wave_schedcodes_dialog;
  RDWaveData *wave_data;
};


#endif  // RDWAVEDATA_DIALOG_H
