// edit_rdlibrary.h
//
// Edit an RDLibrry Configuration
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_RDLIBRARY_H
#define EDIT_RDLIBRARY_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>

#include <rdcardselector.h>
#include <rdlibrary_conf.h>
#include <rdstation.h>

class EditRDLibrary : public QDialog
{
  Q_OBJECT
 public:
  EditRDLibrary(RDStation *station,RDStation *cae_station,QWidget *parent=0);
  ~EditRDLibrary();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void formatData(int);
  void okData();
  void cancelData();

 private:
  void ShowBitRates(int layer,int rate);
  RDLibraryConf *lib_lib;
  RDCardSelector *lib_input_card;
  RDCardSelector *lib_output_card;
  QComboBox *lib_format_box;
  QComboBox *lib_channels_box;
  QComboBox *lib_bitrate_box;
  QComboBox *lib_recmode_box;
  QComboBox *lib_trimstate_box;
  QSpinBox *lib_vox_spin;
  QSpinBox *lib_trim_spin;
  QSpinBox *lib_preroll_spin;
  QDateTimeEdit *lib_maxlength_time;
  QLineEdit *lib_ripdev_edit;
  QComboBox *lib_paranoia_box;
  QComboBox *lib_isrc_box;
  QSpinBox *lib_riplevel_spin;
  QLineEdit *lib_cddb_edit;
  QComboBox *lib_editor_box;
  QComboBox *lib_converter_box;
  QComboBox *lib_limit_search_box;
};


#endif

