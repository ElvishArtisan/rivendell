// rdedit_panel_name.cpp
//
// Edit a SoundPanel Panel Name
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdedit_panel_name.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#include <qlabel.h>
#include <qpushbutton.h>

#include <rdedit_panel_name.h>


RDEditPanelName::RDEditPanelName(QString *panelname,
				 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  panel_name=panelname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Edit Panel Name"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Panel Name
  //
  panel_name_edit=new QLineEdit(this,"panel_name_edit");
  panel_name_edit->setGeometry(95,11,sizeHint().width()-105,19);
  panel_name_edit->setMaxLength(64);
  panel_name_edit->setText(*panel_name);
  panel_name_edit->selectAll();
  QLabel *panel_name_label=new QLabel(panel_name_edit,tr("Panel &Name:"),this,
				       "panel_name_label");
  panel_name_label->setGeometry(10,13,80,19);
  panel_name_label->setFont(font);
  panel_name_label->setAlignment(AlignRight|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,45,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,45,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize RDEditPanelName::sizeHint() const
{
  return QSize(400,110);
} 


QSizePolicy RDEditPanelName::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDEditPanelName::okData()
{
  *panel_name=panel_name_edit->text();
  done(0);
}


void RDEditPanelName::cancelData()
{
  done(-1);
}
