// edit_encoder.h
//
// Edit a Rivendell Encoder
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_encoder.h,v 1.3 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_ENCODER_H
#define EDIT_ENCODER_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rdintegeredit.h>

class EditEncoder : public QDialog
{
 Q_OBJECT
 public:
  EditEncoder(int encoder_id,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  void LoadList(const QString &paramname,RDIntegerEdit *edit);
  void SaveList(const QString &paramname,RDIntegerEdit *edit);
  QLineEdit *edit_extension_edit;
  QLineEdit *edit_commandline_edit;
  RDIntegerEdit *edit_channel_edit;
  RDIntegerEdit *edit_samprate_edit;
  RDIntegerEdit *edit_bitrate_edit;
  int edit_encoder_id;
};


#endif  // EDIT_ENCODER

