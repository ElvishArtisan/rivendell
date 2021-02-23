// rdcombobox.h
//
// A Combo Box widget for Rivendell.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCOMBOBOX_H
#define RDCOMBOBOX_H

#include <vector>

#include <qcombobox.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QMouseEvent>

class RDComboBox : public QComboBox
{
  Q_OBJECT
 public:
  RDComboBox(QWidget *parent=0);
  void insertItem(const QString &str,bool unique=false);
  void setSetupMode(bool state);
  void addIgnoredKey(int key);

 public slots:
  bool setCurrentText(const QString &str);

 signals:
  void setupClicked();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);

 private:
  bool IsItemUnique(const QString &str);
  bool combo_setup_mode;
  std::vector<int> ignored_keys;
};


#endif  // RDCOMBOBOX_H
