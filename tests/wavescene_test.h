// wavescene_test.h
//
// Test harness for RDWaveScene
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

#ifndef WAVESCENE_TEST_H
#define WAVESCENE_TEST_H

#include <QGraphicsView>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include <rdfontengine.h>
#include <rdtransportbutton.h>
#include <rdwavescene.h>

#define WAVESCENE_TEST_USAGE "[options]\n"

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

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void LoadWave();
  void LoadEnergy();
  RDWaveScene *d_scene;
  QGraphicsView *d_view;
  RDCut *d_cut;
  unsigned d_cart_number;
  int d_cut_number;
  QList<uint16_t> d_energy_data;
  RDWaveScene::Channel d_channel;
  RDFontEngine *d_font_engine;
  RDTransportButton *d_up_button;
  RDTransportButton *d_down_button;
  QLineEdit *d_shrink_factor_edit;
  QLabel *d_shrink_factor_label;
};


#endif  // WAVESCENE_TEST_H
