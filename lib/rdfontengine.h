// rdfontengine.h
//
// Engine for calculating fonts in Rivnedell UIs
//
//   (C) Copyright 2019-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFONTENGINE_H
#define RDFONTENGINE_H

#include <qfont.h>
#include <qfontmetrics.h>

#include <rdapplication.h>

class RDFontEngine
{
 public:
  RDFontEngine(const QFont &default_font,RDConfig *c=NULL);
  RDFontEngine(RDConfig *c=NULL);
  ~RDFontEngine();
  QFont buttonFont() const;
  QFontMetrics *buttonFontMetrics() const;
  QFont hugeButtonFont() const;
  QFontMetrics *hugeButtonFontMetrics() const;
  QFont bigButtonFont() const;
  QFontMetrics *bigButtonFontMetrics() const;
  QFont subButtonFont() const;
  QFontMetrics *subButtonFontMetrics() const;
  QFont sectionLabelFont() const;
  QFontMetrics *sectionLabelFontMetrics() const;
  QFont bigLabelFont() const;
  QFontMetrics *bigLabelFontMetrics() const;
  QFont labelFont() const;
  QFontMetrics *labelFontMetrics() const;
  QFont subLabelFont() const;
  QFontMetrics *subLabelFontMetrics() const;
  QFont progressFont() const;
  QFontMetrics *progressFontMetrics() const;
  QFont bannerFont() const;
  QFontMetrics *bannerFontMetrics() const;
  QFont timerFont() const;
  QFontMetrics *timerFontMetrics() const;
  QFont smallTimerFont() const;
  QFontMetrics *smallTimerFontMetrics() const;
  QFont defaultFont() const;
  QFontMetrics *defaultFontMetrics() const;

 private:
  void MakeFonts(const QFont &default_font);
  QFont font_button_font;
  QFontMetrics *font_button_font_metrics;
  QFont font_huge_button_font;
  QFontMetrics *font_huge_button_font_metrics;
  QFont font_big_button_font;
  QFontMetrics *font_big_button_font_metrics;
  QFont font_sub_button_font;
  QFontMetrics *font_sub_button_font_metrics;
  QFont font_section_label_font;
  QFontMetrics *font_section_label_font_metrics;
  QFont font_big_label_font;
  QFontMetrics *font_big_label_font_metrics;
  QFont font_label_font;
  QFontMetrics *font_label_font_metrics;
  QFont font_sub_label_font;
  QFontMetrics *font_sub_label_font_metrics;
  QFont font_progress_font;
  QFontMetrics *font_progress_font_metrics;
  QFont font_banner_font;
  QFontMetrics *font_banner_font_metrics;
  QFont font_timer_font;
  QFontMetrics *font_timer_font_metrics;
  QFont font_small_timer_font;
  QFontMetrics *font_small_timer_font_metrics;
  QFont font_default_font;
  QFontMetrics *font_default_font_metrics;
  RDConfig *font_config;
};


#endif  // RDFONTENGINE_H
