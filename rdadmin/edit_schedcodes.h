// edit_schedcodes.h
//
// Edit scheduler codes dialog
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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

#include <qdialog.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qlabel.h>

class EditSchedCode : public QDialog
{
  Q_OBJECT
  public:
   EditSchedCode(QString schedCode,QString description,QWidget *parent=0,const char *name=0);
   ~EditSchedCode();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QString *schedCode_code;
   QString *schedCode_description;
   QLineEdit *schedCode_name_edit;
   QLineEdit *schedCode_description_edit;
   };


#endif  // EDIT_SCHEDCODES_H
