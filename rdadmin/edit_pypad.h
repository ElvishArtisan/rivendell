// edit_pypad.h
//
// Edit a PyPAD Instance Configuration
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_PYPAD_H
#define EDIT_PYPAD_H

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>

#include <rddialog.h>

class EditPypad : public RDDialog
{
 Q_OBJECT
 public:
  EditPypad(int id,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_script_path_label;
  QLineEdit *edit_script_path_edit;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QLabel *edit_config_label;
  QTextEdit *edit_config_text;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int edit_id;
};


#endif  // EDIT_PYPAD_H
