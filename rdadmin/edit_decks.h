// edit_decks.h
//
// Edit Rivendell Netcatcher Configuration
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <vector>

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPushButton>
#include <QSpinBox>

#include <rdcardselector.h>
#include <rdcatch_conf.h>
#include <rdcombobox.h>
#include <rddeck.h>
#include <rdstation.h>

class EditDecks : public QDialog
{
  Q_OBJECT
  public:
  EditDecks(RDStation *station,RDStation *cae_station,QWidget *parent=0);
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
   void playSettingsChangedData(int id,int card,int port);
   void eventCartSelectedData(int n);
   void closeData();

  protected:
   void paintEvent(QPaintEvent *e);

  private:
   void ReadRecord(int chan);
   void WriteRecord(int chan);
   int GetMatrix();
   int GetOutput();
   QStringList GetActiveOutputMatrices();
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
   RDComboBox *edit_swmatrix_box;
   //   std::vector<int> edit_matrix_ids;
   QLabel *edit_swoutput_label;
   QComboBox *edit_swoutput_box;
   QLabel *edit_swdelay_label;
   QLabel *edit_swdelay_unit;
   QSpinBox *edit_swdelay_box;
   QSpinBox *edit_threshold_box;
   QLineEdit *edit_errorrml_edit;
   QLabel *edit_event_section_label;
   QLabel *edit_event_labels[RD_CUT_EVENT_ID_QUAN];
   QLineEdit *edit_event_edits[RD_CUT_EVENT_ID_QUAN];
   QPushButton *edit_event_buttons[RD_CUT_EVENT_ID_QUAN];
};


#endif  // EDIT_DECKS_H
