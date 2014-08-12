// edit_matrix.h
//
// Edit a Rivendell Matrix
//
//   (C) Copyright 2002-2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_matrix.h,v 1.17.6.3 2013/02/21 02:46:25 cvs Exp $
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

#ifndef EDIT_MATRIX_H
#define EDIT_MATRIX_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <rduser.h>
#include <rdmatrix.h>

class EditMatrix : public QDialog
{
 Q_OBJECT
 public:
  EditMatrix(RDMatrix *matrix,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 protected:
  void paintEvent(QPaintEvent *e);

 private slots:
  void portTypeActivatedData(int index);
  void portType2ActivatedData(int index);
  void inputsButtonData();
  void outputsButtonData();
  void xpointsButtonData();
  void gpisButtonData();
  void gposButtonData();
  void livewireButtonData();
  void livewireGpioButtonData();
  void vguestRelaysButtonData();
  void vguestDisplaysButtonData();
  void sasResourcesButtonData();
  void inputsChangedData(int value);
  void gpisChangedData(int value);
  void gposChangedData(int value);
  void startCartData();
  void stopCartData();
  void startCart2Data();
  void stopCart2Data();
  void okData();
  void cancelData();

 private:
  bool WriteMatrix();
  void WriteGpioTable(RDMatrix::GpioType type);
  RDMatrix *edit_matrix;
  QString edit_stationname;
  int edit_matrix_number;
  QLineEdit *edit_name_edit;
  QLabel *edit_porttype_label;
  QComboBox *edit_porttype_box;
  QLabel *edit_porttype2_label;
  QComboBox *edit_porttype2_box;
  QComboBox *edit_layer_box;
  QLabel *edit_layer_label;
  QLabel *edit_ipaddress_label;
  QLineEdit *edit_ipaddress_edit;
  QLabel *edit_ipport_label;
  QSpinBox *edit_ipport_spin;
  QLabel *edit_username_label;
  QLineEdit *edit_username_edit;
  QLabel *edit_password_label;
  QLineEdit *edit_password_edit;
  QLabel *edit_ipaddress2_label;
  QLineEdit *edit_ipaddress2_edit;
  QLabel *edit_ipport2_label;
  QSpinBox *edit_ipport2_spin;
  QLabel *edit_username2_label;
  QLineEdit *edit_username2_edit;
  QLabel *edit_password2_label;
  QLineEdit *edit_password2_edit;
  QLabel *edit_start_cart_label;
  QLineEdit *edit_start_cart_edit;
  QPushButton *edit_start_cart_button;
  QLabel *edit_start_cart2_label;
  QLineEdit *edit_start_cart2_edit;
  QPushButton *edit_start_cart2_button;
  QLabel *edit_stop_cart_label;
  QLineEdit *edit_stop_cart_edit;
  QPushButton *edit_stop_cart_button;
  QLabel *edit_stop_cart2_label;
  QLineEdit *edit_stop_cart2_edit;
  QPushButton *edit_stop_cart2_button;
  QLabel *edit_port_label;
  QComboBox *edit_port_box;
  QLabel *edit_port2_label;
  QComboBox *edit_port2_box;
  QLabel *edit_inputs_label;
  QSpinBox *edit_inputs_box;
  QLabel *edit_outputs_label; 
  QSpinBox *edit_outputs_box;
  QPushButton *edit_inputs_button;
  QPushButton *edit_outputs_button;
  QPushButton *edit_gpis_button;
  QPushButton *edit_gpos_button;
  QPushButton *edit_livewire_button;
  QPushButton *edit_livewire_gpio_button;
  QPushButton *edit_vguestrelays_button;
  QPushButton *edit_vguestdisplays_button;
  QPushButton *edit_sasresources_button;
  QLabel *edit_device_label;
  QLineEdit *edit_device_edit;
  QLabel *edit_gpis_label;
  QSpinBox *edit_gpis_box;
  QLabel *edit_gpos_label;
  QSpinBox *edit_gpos_box;
  QLabel *edit_card_label;
  QSpinBox *edit_card_box;
  QLabel *edit_displays_label;
  QSpinBox *edit_displays_box;
  
};


#endif

