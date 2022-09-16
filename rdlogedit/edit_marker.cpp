// edit_marker.cpp
//
// Edit a Rivendell marker event
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

#include "edit_marker.h"

EditMarker::EditMarker(QWidget *parent)
  : EditEvent(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDLogEdit - "+tr("Edit Log Marker"));

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
  // Label
  //
  edit_label_edit=new QLineEdit(this);
  edit_label_edit->setGeometry(10,156,sizeHint().width()-200,18);
  edit_label_edit->setMaxLength(64);
  label=new QLabel(tr("Label"),this);
  label->setFont(labelFont());
  label->setGeometry(12,140,60,14);
}


QSize EditMarker::sizeHint() const
{
  return QSize(625,230);
} 


QSizePolicy EditMarker::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditMarker::exec(RDLogLine *ll)
{
  setLogLine(ll);
  edit_comment_edit->setText(logLine()->markerComment());
  edit_label_edit->setText(logLine()->markerLabel());

  return EditEvent::exec();
}


bool EditMarker::saveData()
{
  logLine()->setMarkerComment(edit_comment_edit->text());
  logLine()->setMarkerLabel(edit_label_edit->text());

  return true;
}
