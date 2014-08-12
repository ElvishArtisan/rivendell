//   rdbusydialog.cpp
//
//   A 'progress dialog' widget that shows busy state.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdbusydialog.cpp,v 1.1.2.1 2012/11/26 20:19:36 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <rdbusydialog.h>

RDBusyDialog::RDBusyDialog(QWidget *parent,const char *name,WFlags f)
  : QDialog(parent,name,true,f)
{
  QFont font("helvetica",16,QFont::Bold);
  font.setPixelSize(16);

  bar_label=new QLabel(this);
  bar_label->setFont(font);
  bar_label->setAlignment(Qt::AlignCenter);

  bar_bar=new RDBusyBar(this);
}


RDBusyDialog::~RDBusyDialog()
{
  delete bar_bar;
  delete bar_label;
}


QSize RDBusyDialog::sizeHint() const
{
  return QSize(200,80);
}


void RDBusyDialog::show(const QString &caption,const QString &label)
{
  setCaption(caption);
  bar_label->setText(label);
  bar_bar->activate(true);
  QDialog::show();
}


void RDBusyDialog::hide()
{
  bar_bar->activate(false);
  QDialog::hide();
}


void RDBusyDialog::resizeEvent(QResizeEvent *e)
{
  bar_label->setGeometry(0,0,size().width(),size().height()/2);
  bar_bar->
    setGeometry(10,size().height()/2,size().width()-20,size().height()/2-10);
}
