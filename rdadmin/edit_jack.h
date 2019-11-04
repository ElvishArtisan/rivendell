// edit_jack.h
//
// Edit a Rivendell Jack Configuration
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

#ifndef EDIT_JACK_H
#define EDIT_JACK_H

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <q3textedit.h>

#include <rddialog.h>
#include <rdlistview.h>
#include <rdstation.h>

#define EDITJACK_DEFAULT_SERVERNAME QObject::tr("(default)")

class EditJack : public RDDialog
{
 Q_OBJECT
 public:
  EditJack(RDStation *station,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void startJackData(bool state);
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  QLabel *edit_start_jack_label;
  QCheckBox *edit_start_jack_box;
  QLabel *edit_jack_server_name_label;
  QLineEdit *edit_jack_server_name_edit;
  QLabel *edit_jack_command_line_label;
  QLineEdit *edit_jack_command_line_edit;
  QLabel *edit_jack_audio_ports_label;
  QSpinBox *edit_jack_audio_ports_spin;
  QLabel *edit_jack_client_label;
  RDListView *edit_jack_client_view;
  RDStation *edit_station;
  QPushButton *edit_add_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_JACK_H
