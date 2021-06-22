// rdtrackereditdialog.cpp
//
// Edit a Rivendell Voice Track Log Entry
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

#include "rdtrackereditdialog.h"

RDTrackerEditDialog::RDTrackerEditDialog(RDLogLine *line,QWidget *parent)
  : RDLogEventDialog(line,parent)
{
  setWindowTitle("RDLogEdit - "+tr("Edit Voice Track Marker"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Comment
  //
  edit_comment_edit=new QLineEdit(this);
  edit_comment_edit->setGeometry(10,116,sizeHint().width()-20,18);
  edit_comment_edit->setMaxLength(255);
  QLabel *label=new QLabel(tr("Comment"),this);
  label->setFont(labelFont());
  label->setGeometry(12,100,70,14);

  //
  // Populate Data
  //
  edit_comment_edit->setText(logLine()->markerComment());
}


QSize RDTrackerEditDialog::sizeHint() const
{
  return QSize(625,230);
} 


QSizePolicy RDTrackerEditDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool RDTrackerEditDialog::saveData()
{
  logLine()->setMarkerComment(edit_comment_edit->text());

  return true;
}
