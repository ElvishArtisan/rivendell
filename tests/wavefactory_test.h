// wavefactory_test.h
//
// Test harness for RDWaveFactory
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WAVEFACTORY_TEST_H
#define WAVEFACTORY_TEST_H

#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

#include <rdfontengine.h>
#include <rdtransportbutton.h>
#include <rdwavefactory.h>

#define WAVEFACTORY_TEST_USAGE "--cart-number=<cartnum> --cut-number=<cutnum> --track-mode=single|multi\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void userData();
  void upShrinkData();
  void downShrinkData();
  void gainChangedData(int db);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateWave();
  QGraphicsScene *d_scene;
  QGraphicsView *d_view;
  RDWaveFactory *d_factory;
  unsigned d_cart_number;
  int d_cut_number;
  RDWaveFactory::TrackMode d_track_mode;
  RDFontEngine *d_font_engine;
  RDTransportButton *d_up_button;
  RDTransportButton *d_down_button;
  QLineEdit *d_shrink_factor_edit;
  QGroupBox *d_shrink_factor_group;
  QLabel *d_audio_gain_label;
  QSpinBox *d_audio_gain_spin;
  QLabel *d_audio_gain_unit;
};


#endif  // WAVEFACTORY_TEST_H
