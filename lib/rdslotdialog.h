// rdslotdialog.h
//
// Slot Editor for RDCartSlots.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdslotdialog.h,v 1.3.2.3 2012/11/28 01:57:38 cvs Exp $
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

#ifndef RDSLOTDIALOG_H
#define RDSLOTDIALOG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <rdslotoptions.h>

class RDSlotDialog : public QDialog
{
  Q_OBJECT
 public:
  RDSlotDialog(const QString &caption,QWidget *parent=0);
  ~RDSlotDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDSlotOptions *opts);

 private slots:
  void modeActivatedData(int index);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_mode_label;
  QComboBox *edit_mode_box;
  QLabel *edit_hook_label;
  QComboBox *edit_hook_box;
  QLabel *edit_stop_action_label;
  QComboBox *edit_stop_action_box;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDSlotOptions *edit_options;
  QString edit_caption;
};


#endif  // RDSLOTDIALOG_H
