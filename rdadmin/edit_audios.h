// edit_audios.h
//
// Edit a Rivendell Audio Port Configuration
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_AUDIOS_H
#define EDIT_AUDIOS_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <rd.h>
#include <rdaudio_port.h>
#include <rddialog.h>
#include <rdstation.h>

#include "help_audios.h"

class EditAudioPorts : public RDDialog
{
  Q_OBJECT
 public:
  EditAudioPorts(RDStation *station,QWidget *parent=0);
  ~EditAudioPorts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void cardSelectedData(int);
  void inputMapData(int);
  void helpData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void ReadRecord(int card);
  void WriteRecord();
  void SetEnable(bool state);
  int edit_card_num;
  RDAudioPort *edit_card;
  RDStation *rdstation;
  QLabel *edit_card_label;
  QComboBox *edit_card_box;
  QLabel *card_driver_label;
  QLineEdit *card_driver_edit;
  QLabel *card_label_label;
  QLineEdit *card_label_edit;
  QLabel *edit_clock_label;
  QComboBox *edit_clock_box;
  QLabel *edit_inportnum_label[RD_MAX_PORTS];
  QLabel *edit_input_label_label[RD_MAX_PORTS];
  QLineEdit *edit_input_label_edit[RD_MAX_PORTS];
  QComboBox *edit_type_box[RD_MAX_PORTS];
  QLabel *edit_type_label[RD_MAX_PORTS];
  QComboBox *edit_mode_box[RD_MAX_PORTS];
  QLabel *edit_mode_label[RD_MAX_PORTS];
  QSpinBox *edit_input_box[RD_MAX_PORTS];
  QLabel *edit_input_label[RD_MAX_PORTS];
  QLabel *edit_outportnum_label[RD_MAX_PORTS];
  QLabel *edit_output_label_label[RD_MAX_PORTS];
  QLineEdit *edit_output_label_edit[RD_MAX_PORTS];
  QSpinBox *edit_output_box[RD_MAX_PORTS];
  QLabel *edit_output_label[RD_MAX_PORTS];
  QPushButton *edit_help_button;
  QPushButton *edit_close_button;
};


#endif  // EDIT_AUDIO_H

