// edit_decks.h
//
// Edit Rivendell Netcatcher Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_decks.h,v 1.15.6.1 2012/11/28 18:49:36 cvs Exp $
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

#ifndef EDIT_DECKS_H
#define EDIT_DECKS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <rddeck.h>
#include <rdcardselector.h>
#include <rdstation.h>
#include <rdcatch_conf.h>

class EditDecks : public QDialog
{
  Q_OBJECT
  public:
  EditDecks(RDStation *station,RDStation *cae_station,
	    QWidget *parent=0,const char *name=0);
   ~EditDecks();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void recordDeckActivatedData(int);
   void playDeckActivatedData(int);
   void recordCardChangedData(int card);
   void monitorPortChangedData(int port);
   void formatActivatedData(int);
   void stationActivatedData(const QString &);
   void matrixActivatedData(const QString &);
   void closeData();

  protected:
   void paintEvent(QPaintEvent *e);

  private:
   void ReadRecord(int chan);
   void WriteRecord(int chan);
   int GetMatrix();
   int GetOutput();
   RDCatchConf *edit_catch_conf;
   RDDeck *edit_record_deck;
   RDDeck *edit_play_deck;
   RDDeck *edit_audition_deck;
   RDStation *edit_station;
   int edit_record_channel;
   int edit_play_channel;
   QComboBox *edit_record_deck_box;
   QComboBox *edit_play_deck_box;
   RDCardSelector *edit_record_selector;
   RDCardSelector *edit_play_selector;
   QLabel *edit_monitor_label;
   QSpinBox *edit_monitor_box;
   QLabel *edit_default_on_label;
   QComboBox *edit_default_on_box;
   QComboBox *edit_format_box;
   QComboBox *edit_channels_box;
   QLabel *edit_bitrate_label;
   QComboBox *edit_bitrate_box;
   QLabel *edit_swstation_label;
   QComboBox *edit_swstation_box;
   QLabel *edit_swmatrix_label;
   QComboBox *edit_swmatrix_box;
   QLabel *edit_swoutput_label;
   QComboBox *edit_swoutput_box;
   QLabel *edit_swdelay_label;
   QLabel *edit_swdelay_unit;
   QSpinBox *edit_swdelay_box;
   QSpinBox *edit_threshold_box;
   QLineEdit *edit_errorrml_edit;
};


#endif

