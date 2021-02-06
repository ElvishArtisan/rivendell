// edit_rdlogedit.h
//
// Edit an RDLogEdit Configuration
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

#ifndef EDIT_RDLOGEDIT_H
#define EDIT_RDLOGEDIT_H

#include <QComboBox>
#include <QDateTimeEdit>

#include <rdcardselector.h>
#include <rddialog.h>
#include <rdlogedit_conf.h>
#include <rdstation.h>

class EditRDLogedit : public RDDialog
{
  Q_OBJECT
 public:
  EditRDLogedit(RDStation *station,RDStation *cae_station,QWidget *parent=0);
  ~EditRDLogedit();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void formatData(int);
  void selectStartData();
  void selectEndData();
  void selectRecordStartData();
  void selectRecordEndData();
  void okData();
  void cancelData();

 private:
  void ShowBitRates(int index,int rate);
  RDLogeditConf *lib_lib;
  RDCardSelector *lib_input_card;
  RDCardSelector *lib_output_card;
  QComboBox *lib_format_box;
  QComboBox *lib_channels_box;
  QComboBox *lib_bitrate_box;
  QComboBox *lib_enable_second_start_box;
  QSpinBox *lib_preroll_spin;
  QSpinBox *lib_threshold_spin;
  QSpinBox *lib_normalization_spin;
  QTimeEdit *lib_maxlength_time;
  QLineEdit *lib_waveform_caption_edit;
  QLineEdit *lib_startcart_edit;
  QLineEdit *lib_endcart_edit;
  QLineEdit *lib_recstartcart_edit;
  QLineEdit *lib_recendcart_edit;
  QComboBox *lib_default_transtype_box;
  QString lib_filter;
  QString lib_group;
};


#endif  // EDIT_RDLOGEDIT_H
