// edit_notes.h
//
// Edit Cart Notes.
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_NOTES_H
#define EDIT_NOTES_H

#include <qpushbutton.h>
#include <qtextedit.h>

#include <rdcart.h>
#include <rddialog.h>

class EditNotes : public RDDialog
{
 Q_OBJECT
 public:
  EditNotes(RDCart *cart,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void resizeEvent(QResizeEvent *e);

 private slots:
  void okData();
  void cancelData();

 private:
  QTextEdit *notes_view;
  QPushButton *notes_ok_button;
  QPushButton *notes_cancel_button;
  RDCart *notes_cart;
};


#endif  // EDIT_NOTES_H
