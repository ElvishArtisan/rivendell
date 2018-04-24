// add_event.h
//
// Add a Rivendell Log Manager Event
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

#ifndef ADD_EVENT_H
#define ADD_EVENT_H

#include <qdialog.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QCloseEvent>

#include <rdevent.h>

class AddEvent : public QDialog
{
  Q_OBJECT
 public:
  AddEvent(QString *logname,QWidget *parent=0);
  ~AddEvent();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QLineEdit *event_name_edit;
  QString *event_name;
};


#endif

