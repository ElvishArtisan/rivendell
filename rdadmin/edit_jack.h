// edit_jack.h
//
// Edit a Rivendell Jack Configuration
//
//   (C) Copyright 2002-2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_jack.h,v 1.2.4.4 2012/11/15 19:27:13 cvs Exp $
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

#include <qdialog.h>
#include <qlistview.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdstation.h>
#include <rdlistview.h>

#define EDITJACK_DEFAULT_SERVERNAME QObject::tr("(default)")

class EditJack : public QDialog
{
 Q_OBJECT
 public:
  EditJack(RDStation *station,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void startJackData(bool state);
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
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
