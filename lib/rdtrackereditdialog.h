// rdtrackereditdialog.h
//
// Edit a Rivendell Voice Track Log Entry
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTRACKEREDITDIALOG_H
#define RDTRACKEREDITDIALOG_H

#include <qlineedit.h>

#include "rdlogeventdialog.h"

class RDTrackerEditDialog : public RDLogEventDialog
{
  Q_OBJECT
 public:
  RDTrackerEditDialog(RDLogLine *,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  bool saveData();

 private:
  QCheckBox *edit_overlap_box;
  QLabel *edit_overlap_label;
  QLineEdit *edit_comment_edit;
};


#endif  // RDTRACKEREDITDIALOG_H
