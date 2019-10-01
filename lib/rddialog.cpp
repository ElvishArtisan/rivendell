// rddialog.cpp
//
// Base class for Rivendell modal dialogs.
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddialog.h"

RDDialog::RDDialog(QWidget *parent,Qt::WindowFlags f)
  : QDialog(parent,f)
{
  dialog_config=rda->config();
  MakeFonts();
  setFont(dataFont());
}


RDDialog::RDDialog(RDConfig *config,QWidget *parent,Qt::WindowFlags f)
  : QDialog(parent,f)
{
  dialog_config=config;
  MakeFonts();
}


QFont RDDialog::buttonFont() const
{
  return dialog_button_font;
}


QFont RDDialog::subButtonFont() const
{
  return dialog_sub_button_font;
}


QFont RDDialog::sectionLabelFont() const
{
  return dialog_section_label_font;
}


QFont RDDialog::labelFont() const
{
  return dialog_label_font;
}


QFont RDDialog::subLabelFont() const
{
  return dialog_sub_label_font;
}


QFont RDDialog::dataFont() const
{
  return dialog_data_font;
}


void RDDialog::MakeFonts()
{
  QString family=font().family();
  int button_size=font().pixelSize();
  int label_size=font().pixelSize();
  int data_size=font().pixelSize();

  if(!dialog_config->fontFamily().isEmpty()) {
    family=dialog_config->fontFamily();
  }
  if(dialog_config->fontButtonSize()>0) {
    button_size=dialog_config->fontButtonSize();
  }
  if(dialog_config->fontLabelSize()>0) {
    label_size=dialog_config->fontLabelSize();
  }
  if(dialog_config->fontDataSize()>0) {
    data_size=dialog_config->fontDataSize();
  }

  dialog_button_font=QFont(family,button_size,QFont::Bold);
  dialog_button_font.setPixelSize(button_size);

  dialog_sub_button_font=QFont(family,button_size-2,QFont::Normal);
  dialog_sub_button_font.setPixelSize(button_size-2);

  dialog_section_label_font=QFont(family,label_size+2,QFont::Bold);
  dialog_section_label_font.setPixelSize(label_size+2);

  dialog_label_font=QFont(family,label_size,QFont::Bold);
  dialog_label_font.setPixelSize(label_size);

  dialog_sub_label_font=QFont(family,label_size,QFont::Normal);
  dialog_sub_label_font.setPixelSize(label_size);

  dialog_data_font=QFont(family,data_size,QFont::Normal);
  dialog_data_font.setPixelSize(data_size);
}
