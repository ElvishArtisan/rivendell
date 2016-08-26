// edit_schedcodes.h
//
// Edit scheduler codes dialog
//
//   by Stefan Gabriel <stg@st-gabriel.de>
//   Changes for Qt4 (C) 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SCHEDCODES_H
#define EDIT_SCHEDCODES_H

#include <QDialog>
#include <QLineEdit>

class EditSchedCode : public QDialog
{
  Q_OBJECT
  public:
   EditSchedCode(QString schedcode,QString description,QWidget *parent=0);
   ~EditSchedCode();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QString *schedcode_code;
   QString *schedcode_description;
   QLineEdit *schedcode_name_edit;
   QLineEdit *schedcode_description_edit;
   };


#endif  // EDIT_SCHEDCODES_H
