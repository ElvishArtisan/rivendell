// rdfontset.h
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

#ifndef RDFONTSET_H
#define RDFONTSET_H

#include <qfont.h>

#include <rdapplication.h>

class RDFontSet
{
 public:
  RDFontSet(const QFont &default_font,RDConfig *c=NULL);
  RDFontSet(RDConfig *c=NULL);
  QFont buttonFont() const;
  QFont bigButtonFont() const;
  QFont subButtonFont() const;
  QFont sectionLabelFont() const;
  QFont labelFont() const;
  QFont subLabelFont() const;
  QFont progressFont() const;
  QFont bannerFont() const;
  QFont timerFont() const;
  QFont defaultFont() const;

 private:
  void MakeFonts(const QFont &default_font);
  QFont font_button_font;
  QFont font_big_button_font;
  QFont font_sub_button_font;
  QFont font_section_label_font;
  QFont font_label_font;
  QFont font_sub_label_font;
  QFont font_progress_font;
  QFont font_banner_font;
  QFont font_timer_font;
  QFont font_default_font;
  RDConfig *font_config;
};


#endif  // RDFONTSET_H
