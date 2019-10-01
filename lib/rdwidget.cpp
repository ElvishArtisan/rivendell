// rdwidget.cpp
//
// Base class for Rivendell modal widgets.
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

#include "rdwidget.h"

RDWidget::RDWidget(QWidget *parent,Qt::WindowFlags f)
  : QWidget(parent,f)
{
  widget_config=rda->config();
  MakeFonts();
  setFont(dataFont());
}


RDWidget::RDWidget(RDConfig *config,QWidget *parent,Qt::WindowFlags f)
  : QWidget(parent,f)
{
  widget_config=config;
  MakeFonts();
  setFont(dataFont());
}


QFont RDWidget::buttonFont() const
{
  return widget_button_font;
}


QFont RDWidget::subButtonFont() const
{
  return widget_sub_button_font;
}


QFont RDWidget::sectionLabelFont() const
{
  return widget_section_label_font;
}


QFont RDWidget::labelFont() const
{
  return widget_label_font;
}


QFont RDWidget::subLabelFont() const
{
  return widget_sub_label_font;
}


QFont RDWidget::progressFont() const
{
  return widget_progress_font;
}


QFont RDWidget::dataFont() const
{
  return widget_data_font;
}


void RDWidget::MakeFonts()
{
  QString family=font().family();
  int button_size=font().pixelSize();
  int label_size=font().pixelSize();
  int data_size=font().pixelSize();

  if(!widget_config->fontFamily().isEmpty()) {
    family=widget_config->fontFamily();
  }
  if(widget_config->fontButtonSize()>0) {
    button_size=widget_config->fontButtonSize();
  }
  if(widget_config->fontLabelSize()>0) {
    label_size=widget_config->fontLabelSize();
  }
  if(widget_config->fontDataSize()>0) {
    data_size=widget_config->fontDataSize();
  }

  widget_button_font=QFont(family,button_size,QFont::Bold);
  widget_button_font.setPixelSize(button_size);

  widget_sub_button_font=QFont(family,button_size-2,QFont::Normal);
  widget_sub_button_font.setPixelSize(button_size-2);

  widget_section_label_font=QFont(family,label_size+2,QFont::Bold);
  widget_section_label_font.setPixelSize(label_size+2);

  widget_label_font=QFont(family,label_size,QFont::Bold);
  widget_label_font.setPixelSize(label_size);

  widget_sub_label_font=QFont(family,label_size,QFont::Normal);
  widget_sub_label_font.setPixelSize(label_size);

  widget_progress_font=QFont(family,label_size+4,QFont::Bold);
  widget_progress_font.setPixelSize(label_size+4);

  widget_data_font=QFont(family,data_size,QFont::Normal);
  widget_data_font.setPixelSize(data_size);
}
