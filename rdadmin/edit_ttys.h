// edit_ttys.h
//
// Edit a Rivendell TTY Configuration
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_ttys.h,v 1.7 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_TTYS_H
#define EDIT_TTYS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <rdtty.h>
#include <rd.h>


class EditTtys : public QDialog
{
  Q_OBJECT
  public:
   EditTtys(QString station,QWidget *parent=0,const char *name=0);
   ~EditTtys();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void idSelectedData();
   void enableButtonData(int);
   void closeData();

  private:
   void ReadRecord(int id);
   void WriteRecord(int id);
   void SetEnable(bool state);
   RDTty *edit_tty;
   int edit_id;
   bool edit_port_modified[MAX_TTYS];
   QString edit_station;
   QComboBox *edit_port_box;
   QCheckBox *edit_enable_button;
   QLabel *edit_port_label;
   QLineEdit *edit_port_edit;
   QLabel *edit_baudrate_label;
   QComboBox *edit_baudrate_box;
   QLabel *edit_databits_label;
   QComboBox *edit_databits_box;
   QLabel *edit_stopbits_label;
   QComboBox *edit_stopbits_box;
   QLabel *edit_parity_label;
   QComboBox *edit_parity_box;
   QLabel *edit_termination_label;
   QComboBox *edit_termination_box;
};


#endif

