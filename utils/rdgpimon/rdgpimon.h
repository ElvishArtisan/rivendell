// rdgpimon.h
//
// A Qt-based application for testing general purpose input (GPI) devices.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgpimon.h,v 1.9 2010/07/29 19:32:40 cvs Exp $
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


#ifndef RDGPIMON_H
#define RDGPIMON_H

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qstring.h>
#include <qsignalmapper.h>
#include <qlabel.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qpixmap.h>

#include <rdmatrix.h>
#include <rdconfig.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rdtransportbutton.h>

#include <gpi_label.h>


#define GPIMON_START_UP_DELAY 100
#define GPIMON_ROWS 4
#define GPIMON_COLS 8

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
   MainWidget(QWidget *parent=0,const char *name=0);
   ~MainWidget();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

 private slots:
  void userData();
  void typeActivatedData(int index);
  void matrixActivatedData(int index);
  void gpiStateChangedData(int matrix,int line,bool state);
  void gpoStateChangedData(int matrix,int line,bool state);
  void gpiMaskChangedData(int matrix,int line,bool state);
  void gpoMaskChangedData(int matrix,int line,bool state);
  void gpiCartChangedData(int matrix,int line,int off_cartnum,int on_cartnum);
  void gpoCartChangedData(int matrix,int line,int off_cartnum,int on_cartnum);
  void startUpData();
  void upData();
  void downData();
  void quitMainWidget();

 private:
  void UpdateLabelsUp(int last_line);
  void UpdateLabelsDown(int first_line);
  RDConfig *gpi_config;
  QSqlDatabase *gpi_db;
  RDRipc *gpi_ripc;
  RDStation *gpi_station;
  RDMatrix *gpi_matrix;
  QComboBox *gpi_type_box;
  QComboBox *gpi_matrix_box;
  QPushButton *gpi_close_button;
  QPixmap *gpi_rivendell_map;
  GpiLabel *gpi_labels[GPIMON_ROWS*GPIMON_COLS];
  RDTransportButton *gpi_up_button;
  RDTransportButton *gpi_down_button;
  int gpi_first_line;
  int gpi_last_line;
};


#endif 
