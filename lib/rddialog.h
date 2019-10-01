// rddialog.h
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

#ifndef RDDIALOG_H
#define RDDIALOG_H

#include <qdialog.h>
#include <qfont.h>

#include <rdapplication.h>

class RDDialog : public QDialog
{
  Q_OBJECT;
 public:
  RDDialog(QWidget *parent=0,Qt::WindowFlags f=0);
  RDDialog(RDConfig *config,QWidget *parent=0,Qt::WindowFlags f=0);
  QFont buttonFont() const;
  QFont subButtonFont() const;
  QFont sectionLabelFont() const;
  QFont labelFont() const;
  QFont subLabelFont() const;
  QFont dataFont() const;

 private:
  void MakeFonts();
  QFont dialog_button_font;
  QFont dialog_sub_button_font;
  QFont dialog_section_label_font;
  QFont dialog_label_font;
  QFont dialog_sub_label_font;
  QFont dialog_data_font;
  RDConfig *dialog_config;
};


#endif  // RDDIALOG_H
