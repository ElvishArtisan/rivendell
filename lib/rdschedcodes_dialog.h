// rdschedcode_dialog.h
//
// Scheduler code editor dialog
//
//  (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   Based on original code by Stefan Gabriel <stg@st-gabriel.de>
//
//     $Id: rdschedcodes_dialog.h,v 1.1.2.1 2014/05/28 21:21:40 cvs Exp $
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

#ifndef RDSCHEDCODE_DIALOG_H
#define RDSCHEDCODE_DIALOG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>

#include <rdlistselector.h>

class RDSchedCodesDialog : public QDialog
{
  Q_OBJECT
 public:
  RDSchedCodesDialog(QWidget *parent=0);
  ~RDSchedCodesDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QStringList *sched_codes,QStringList *remove_codes);

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private slots:
  void okData();
  void cancelData();

 private:
  RDListSelector *codes_sel;
  RDListSelector *remove_codes_sel;
  QStringList *edit_sched_codes;
  QStringList *edit_remove_codes;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // RDSCHEDCODE_DIALOG_H
