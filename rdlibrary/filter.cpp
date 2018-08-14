// filter.cpp
//
// Set Filter widget for RDLibrary.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <q3buttongroup.h>

#include <filter.h>

Filter::Filter(QString *filter,QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);

  filter_text=filter;
  setWindowTitle("RDLibrary - "+tr("Library Filter"));

  //
  // OK Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(15,40,100,55);
  ok_button->setFont(button_font);
  ok_button->setText(tr("&OK"));
  ok_button->setDefault(true);
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Clear Button
  //
  QPushButton *clear_button=new QPushButton(this);
  clear_button->setGeometry(125,40,100,55);
  clear_button->setFont(button_font);
  clear_button->setText(tr("C&lear"));
  connect(clear_button,SIGNAL(clicked()),this,SLOT(clearData()));

  //
  // Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(235,40,100,55);
  cancel_button->setFont(button_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Filter Text
  //
  filter_text_edit=new QLineEdit(this);
  filter_text_edit->setGeometry(65,10,275,19);
  filter_text_edit->setMaxLength(16);
  filter_text_edit->setFocus();
  filter_text_edit->setFont(QFont("Helvetica",14,QFont::Normal));
  filter_text_edit->setText(*filter);
  QLabel *filter_text_label=new QLabel(filter_text_edit,tr("&Filter:"),this);
  filter_text_label->setGeometry(10,12,50,19);
  filter_text_label->setFont(label_font);
  filter_text_label->setAlignment(Qt::AlignRight|Qt::TextShowMnemonic);
}


QSize Filter::sizeHint() const
{
  return QSize(350,100);
} 


QSizePolicy Filter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void Filter::okData()
{
  *filter_text=filter_text_edit->text();
  done(0);
}


void Filter::clearData()
{
  filter_text_edit->clear();
}


void Filter::cancelData()
{
  done(1);
}
