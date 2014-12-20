// virtdetails.h
//
// Show profiling data for an AudioScience adapter
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef VIRTDETAILS_H
#define VIRTDETAILS_H

#include <stdint.h>

#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <asihpi/hpi.h>

class VirtDetails : public QDialog
{
 Q_OBJECT
 public:
  VirtDetails(uint16_t card,hpi_handle_t profile,uint16_t profile_quan,
	      QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

private slots:
  void updateProfileData();
  void closeData();

 private:
  uint16_t virt_card;
  hpi_handle_t virt_profile;
  QLabel *virt_utilization_label;
  QLineEdit *virt_utilization_edit;
  QLabel *virt_profile_label;
  QComboBox *virt_profile_box;
  QLabel *virt_interval_label;
  QLineEdit *virt_interval_edit;
  QLabel *virt_total_ticks_label;
  QLineEdit *virt_total_ticks_edit;
  QLabel *virt_call_count_label;
  QLineEdit *virt_call_count_edit;
  QLabel *virt_max_ticks_label;
  QLineEdit *virt_max_ticks_edit;
  QLabel *virt_ticks_per_ms_label;
  QLineEdit *virt_ticks_per_ms_edit;
};


#endif  // VIRTDETAILS_H
