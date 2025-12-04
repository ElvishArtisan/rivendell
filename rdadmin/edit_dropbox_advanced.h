// edit_dropbox_advanced.h
//
// Edit a Rivendell Dropbox Configuration
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

#ifndef EDIT_DROPBOX_ADVANCED_H
#define EDIT_DROPBOX_ADVANCED_H

#include <QLabel>
#include <QSpinBox>

#include <rddialog.h>
#include <rddropbox.h>

class EditDropboxAdvanced : public RDDialog
{
 Q_OBJECT
 public:
  EditDropboxAdvanced(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDDropbox *dropbox);
  
 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDDropbox *box_dropbox;
  QLabel *box_scan_count_label;
  QSpinBox *box_scan_count_spin;
  QLabel *box_scan_interval_label;
  QSpinBox *box_scan_interval_spin;
  QLabel *box_scan_interval_unit;
  QPushButton *box_ok_button;
  QPushButton *box_cancel_button;
};


#endif  // EDIT_DROPBOX_ADVANCED_H
