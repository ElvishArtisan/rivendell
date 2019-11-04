// view_pypad_errors.h
//
// View the Edit Log for a PyPAD Instance
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

#ifndef VIEW_PYPAD_ERRORS_H
#define VIEW_PYPAD_ERRORS_H

#include <qpushbutton.h>
#include <qtextedit.h>

#include <rddialog.h>

class ViewPypadErrors : public RDDialog
{
 Q_OBJECT
 public:
  ViewPypadErrors(int id,QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
   void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QTextEdit *view_text;
  QPushButton *view_close_button;
};


#endif  // VIEW_PYPAD_ERRORS_H

