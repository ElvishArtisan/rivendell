// edit_track.h
//
// Edit a Rivendell LogManager Track
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_TRACK_H
#define EDIT_TRACK_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <q3textedit.h>
//Added by qt3to4:
#include <QCloseEvent>

class EditTrack : public QDialog
{
 Q_OBJECT
 public:
  EditTrack(QString *text,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QString *edit_text;
  Q3TextEdit *edit_text_edit;
};


#endif  // EDIT_TRACK_H

