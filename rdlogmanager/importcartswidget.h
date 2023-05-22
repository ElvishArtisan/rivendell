// importcartswidget.h
//
// Widget for listing import events in rdlogmanager(1)
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef IMPORTCARTSWIDGET_H
#define IMPORTCARTSWIDGET_H

#include <QLineEdit>

#include <rdevent.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#include "importcartsmodel.h"
#include "importcartsview.h"

class ImportCartsWidget : public RDWidget
{
 Q_OBJECT
 public:
 ImportCartsWidget(ImportCartsModel::ImportType type,QWidget *parent=0);
  ~ImportCartsWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  ImportCartsModel::ImportType importType() const;
  void setEventName(const QString &str);
  void load(RDEvent *evt);
  void save(RDEvent *evt,RDLogLine::TransType first_trans=RDLogLine::NoTrans)
    const;

 private slots:
  void lengthChangedData(int msecs);
  void upData();
  void downData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  ImportCartsModel::ImportType d_import_type;
  QLabel *d_title_label;
  QLabel *d_length_label;
  QLineEdit *d_length_edit;
  ImportCartsView *d_view;
  ImportCartsModel *d_model;
  RDTransportButton *d_up_button;
  RDTransportButton *d_down_button;
};


#endif  // IMPORTCARTSWIDGET_H
