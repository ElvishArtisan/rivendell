// edit_jack_client.h
//
// Edit a Rivendell Jack Client Configuration
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_JACK_CLIENT_H
#define EDIT_JACK_CLIENT_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

#include <rdstation.h>

class EditJackClient : public QDialog
{
 Q_OBJECT
 public:
  EditJackClient(RDStation *station,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(int id);

 private slots:
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  QLabel *edit_jack_description_label;
  QLineEdit *edit_jack_description_edit;
  QLabel *edit_jack_command_line_label;
  QLineEdit *edit_jack_command_line_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDStation *edit_station;
  int edit_id;
};


#endif  // EDIT_JACK_CLIENT_H
