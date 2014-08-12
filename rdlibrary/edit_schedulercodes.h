// edit_schedulercodes.h
//
// Edit the scheduler codes of a cart
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

#ifndef EDIT_SCHEDULERCODES_H
#define EDIT_SCHEDULERCODES_H

#include <qdialog.h>
#include <qlabel.h>
#include <qsqldatabase.h>

#include <rdlistselector.h>

class EditSchedulerCodes : public QDialog
{
  Q_OBJECT
 public:
  EditSchedulerCodes(QString *sched_codes,QString *remove_codes,QWidget *parent=0,const char *name=0);
  ~EditSchedulerCodes();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void paintEvent(QPaintEvent *e);
  
 private slots:
  void okData();
  void cancelData();

 private:
  RDListSelector *codes_sel;
  RDListSelector *remove_codes_sel;
  QString *edit_sched_codes;
  QString *edit_remove_codes;
};



#endif

