// rdchunk.h
//
// Utility for examining chunk data in WAV files.
//
//   (C) Copyright 2002-2004,2008 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdchunk.h,v 1.3.8.1 2013/12/05 17:37:48 cvs Exp $
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


#ifndef RDCHUNK_H
#define RDCHUNK_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdwavefile.h>

#define MIXER_X 150
#define MIXER_Y 200
#define METER_UPDATE_INTERVAL 50


class MainWidget : public QWidget
{
  Q_OBJECT
  public:
   MainWidget(QWidget *parent=0,const char *name=0);
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  protected:
   void paintEvent(QPaintEvent *);

  private slots:
   void loadWaveFile();
   void quitMainWidget();
   void displayFmt();
   void displayData();
   void displayFact();
   void displayCart();
   void displayBext();
   void displayMext();
   void displayLevl();
   void displayAIR1();

  private:
   void createChunkButtons();
   void destroyChunkButtons();
   QTimer *meter_timer;
   RDWaveFile *wavefile;
   int y_chunk_button;
   bool wave_loaded;
   QString wave_name;
   QString wave_path;
   QString wave_base;
   QPushButton *fmt_button;
   QPushButton *data_button;
   QPushButton *fact_button;
   QPushButton *cart_button;
   QPushButton *bext_button;
   QPushButton *mext_button;
   QPushButton *levl_button;
   QPushButton *AIR1_button;
   QLabel *wave_chunk_label;
   int play_port;
};


#endif 
