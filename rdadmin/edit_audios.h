// edit_audios.h
//
// Edit a Rivendell Audio Port Configuration
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_audios.h,v 1.9 2010/07/29 19:32:34 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <rd.h>
#include <rdaudio_port.h>

#include <help_audios.h>
#include <rdstation.h>


class EditAudioPorts : public QDialog
{
  Q_OBJECT
  public:
   EditAudioPorts(QString station,QWidget *parent=0,const char *name=0);
   ~EditAudioPorts();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void cardSelectedData(int);
   void inputMapData(int);
   void helpData();
   void closeData();

  private:
   void ReadRecord(int card);
   void WriteRecord();
   void SetEnable(bool state);
   int edit_card_num;
   RDAudioPort *edit_card;
   RDStation *rdstation;
   QString edit_station;
   QComboBox *edit_card_box;
   QLineEdit *card_driver_edit;
   QComboBox *edit_clock_box;
   QLabel *edit_clock_label;
   QComboBox *edit_type_box[RD_MAX_PORTS];
   QLabel *edit_type_label[RD_MAX_PORTS];
   QComboBox *edit_mode_box[RD_MAX_PORTS];
   QLabel *edit_mode_label[RD_MAX_PORTS];
   QSpinBox *edit_input_box[RD_MAX_PORTS];
   QLabel *edit_input_label[RD_MAX_PORTS];
   QSpinBox *edit_output_box[RD_MAX_PORTS];
   QLabel *edit_output_label[RD_MAX_PORTS];
};


#endif

