// edit_rdlogedit.h
//
// Edit an RDLogEdit Configuration
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_rdlogedit.h,v 1.9.6.1 2014/01/08 18:14:35 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>

#include <rdstation.h>
#include <rdlogedit_conf.h>
#include <rdcardselector.h>


class EditRDLogedit : public QDialog
{
  Q_OBJECT
 public:
  EditRDLogedit(RDStation *station,RDStation *cae_station,
		QWidget *parent=0,const char *name=0);
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
  void ShowBitRates(int layer,int rate);
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
  QLineEdit *lib_startcart_edit;
  QLineEdit *lib_endcart_edit;
  QLineEdit *lib_recstartcart_edit;
  QLineEdit *lib_recendcart_edit;
  QComboBox *lib_default_transtype_box;
  QString lib_filter;
  QString lib_group;
};


#endif
