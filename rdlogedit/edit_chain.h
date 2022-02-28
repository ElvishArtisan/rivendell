// edit_chain.h
//
// Edit a Rivendell Chain-To event
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_CHAIN_H
#define EDIT_CHAIN_H

#include <rddialog.h>
#include <rdlog_line.h>
#include <rdtimeedit.h>

#include "edit_event.h"

class EditChain : public EditEvent
{
  Q_OBJECT
 public:
  EditChain(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDLogLine *ll);

 private slots:
  void selectLogData();
  void labelChangedData(const QString &);

 protected:
  bool saveData();
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_comment_label;
  QLineEdit *edit_comment_edit;
  QLabel *edit_label_label;
  QLineEdit *edit_label_edit;
  QPushButton *edit_select_button;
};


#endif  // EDIT_CHAIN_H
