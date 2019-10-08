// rdfontengine.cpp
//
// Engine for calculating fonts in Rivnedell UIs
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

#include "rdfontengine.h"

RDFontEngine::RDFontEngine(const QFont &default_font,RDConfig *c)
{
  if(c==NULL) {
    font_config=rda->config();
  }
  else {
    font_config=c;
  }
  MakeFonts(default_font);
}


RDFontEngine::RDFontEngine(RDConfig *c)
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


QFont RDFontEngine::buttonFont() const
{
  return font_button_font;
}


QFont RDFontEngine::hugeButtonFont() const
{
  return font_huge_button_font;
}


QFont RDFontEngine::bigButtonFont() const
{
  return font_big_button_font;
}


QFont RDFontEngine::subButtonFont() const
{
  return font_sub_button_font;
}


QFont RDFontEngine::sectionLabelFont() const
{
  return font_section_label_font;
}


QFont RDFontEngine::labelFont() const
{
  return font_label_font;
}


QFont RDFontEngine::subLabelFont() const
{
  return font_sub_label_font;
}


QFont RDFontEngine::progressFont() const
{
  return font_progress_font;
}


QFont RDFontEngine::bannerFont() const
{
  return font_banner_font;
}


QFont RDFontEngine::timerFont() const
{
  return font_timer_font;
}


QFont RDFontEngine::smallTimerFont() const
{
  return font_small_timer_font;
}


QFont RDFontEngine::defaultFont() const
{
  return font_default_font;
}


void RDFontEngine::MakeFonts(const QFont &default_font)
{
  /*
  printf("family: %s  pixelSize: %d  pointSize: %d\n",
	 (const char *)default_font.family().toUtf8(),
	 default_font.pixelSize(),
	 default_font.pointSize());
  */

  //
  // Default Font Values
  //
  QString family="System";
  int button_size=12;
  int label_size=11;
  int default_size=11;

  //
  // Overrides from rd.conf(5)
  //
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

  //
  // Generate Fonts
  //
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
