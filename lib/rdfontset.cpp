// rdfontset.cpp
//
// Base set of fonts for Rivendell UIs
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

#include <rdapplication.h>

#include "rdfontset.h"

RDFontSet::RDFontSet(const QFont &default_font,RDConfig *c)
{
  if(c==NULL) {
    font_config=rda->config();
  }
  else {
    font_config=c;
  }
  MakeFonts(default_font);
}


RDFontSet::RDFontSet(RDConfig *c)
{
  if(c==NULL) {
    font_config=rda->config();
  }
  else {
    font_config=c;
  }
  MakeFonts(QFont(font_config->fontFamily(),font_config->fontDefaultSize(),
		  QFont::Normal));
}


QFont RDFontSet::buttonFont() const
{
  return font_button_font;
}


QFont RDFontSet::hugeButtonFont() const
{
  return font_huge_button_font;
}


QFont RDFontSet::bigButtonFont() const
{
  return font_big_button_font;
}


QFont RDFontSet::subButtonFont() const
{
  return font_sub_button_font;
}


QFont RDFontSet::sectionLabelFont() const
{
  return font_section_label_font;
}


QFont RDFontSet::labelFont() const
{
  return font_label_font;
}


QFont RDFontSet::subLabelFont() const
{
  return font_sub_label_font;
}


QFont RDFontSet::progressFont() const
{
  return font_progress_font;
}


QFont RDFontSet::bannerFont() const
{
  return font_banner_font;
}


QFont RDFontSet::timerFont() const
{
  return font_timer_font;
}


QFont RDFontSet::smallTimerFont() const
{
  return font_small_timer_font;
}


QFont RDFontSet::defaultFont() const
{
  return font_default_font;
}


void RDFontSet::MakeFonts(const QFont &default_font)
{
  QString family=default_font.family();
  int button_size=default_font.pixelSize();
  int label_size=default_font.pixelSize();
  int default_size=default_font.pixelSize();

  if(!font_config->fontFamily().isEmpty()) {
    family=font_config->fontFamily();
  }
  if(font_config->fontButtonSize()>0) {
    button_size=font_config->fontButtonSize();
  }
  if(font_config->fontLabelSize()>0) {
    label_size=font_config->fontLabelSize();
  }
  if(font_config->fontDefaultSize()>0) {
    default_size=font_config->fontDefaultSize();
  }

  font_button_font=QFont(family,button_size,QFont::Bold);
  font_button_font.setPixelSize(button_size);

  font_huge_button_font=QFont(family,button_size+24,QFont::DemiBold);
  font_huge_button_font.setPixelSize(button_size+24);

  font_big_button_font=QFont(family,button_size+4,QFont::DemiBold);
  font_big_button_font.setPixelSize(button_size+4);

  font_sub_button_font=QFont(family,button_size-2,QFont::Normal);
  font_sub_button_font.setPixelSize(button_size-2);

  font_section_label_font=QFont(family,label_size+2,QFont::Bold);
  font_section_label_font.setPixelSize(label_size+2);

  font_label_font=QFont(family,label_size,QFont::Bold);
  font_label_font.setPixelSize(label_size);

  font_sub_label_font=QFont(family,label_size,QFont::Normal);
  font_sub_label_font.setPixelSize(label_size);

  font_progress_font=QFont(family,label_size+4,QFont::Bold);
  font_progress_font.setPixelSize(label_size+4);

  font_banner_font=QFont(family,26,QFont::Normal);
  font_banner_font.setPixelSize(26);

  font_timer_font=QFont(family,20,QFont::Normal);
  font_timer_font.setPixelSize(20);

  font_small_timer_font=QFont(family,default_size+2,QFont::Normal);
  font_small_timer_font.setPixelSize(default_size+2);

  font_default_font=QFont(family,default_size,QFont::Normal);
  font_default_font.setPixelSize(default_size);
}
