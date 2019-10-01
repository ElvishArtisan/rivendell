// view_pypad_errors.cpp
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

#include <rddb.h>

#include "view_pypad_errors.h"

ViewPypadErrors::ViewPypadErrors(int id,QWidget *parent)
  : RDDialog(parent)
{
  setMinimumSize(sizeHint());
  setWindowTitle(tr("Script Error Log")+
		 " ["+tr("ID")+QString().sprintf(": %d]",id)); 

  //
  // Viewer
  //
  view_text=new QTextEdit(this);
  view_text->setReadOnly(true);
  QString sql=QString("select ERROR_TEXT from PYPAD_INSTANCES where ")+
    QString().sprintf("ID=%u",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    view_text->setPlainText(q->value(0).toString());
  }
  delete q;

  //
  // Close Button
  //
  view_close_button=new QPushButton(tr("Close"),this);
  view_close_button->setFont(buttonFont());
  connect(view_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ViewPypadErrors::sizeHint() const
{
  return QSize(600,400);
}


void ViewPypadErrors::closeData()
{
  done(true);
}


void ViewPypadErrors::resizeEvent(QResizeEvent *e)
{
  view_text->setGeometry(10,10,size().width()-20,size().height()-80);

  view_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
