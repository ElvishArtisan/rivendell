// rdmacrocartmodel.h
//
// Data model for Rivendell macro cart contents
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

#ifndef RDMACROCARTMODEL_H
#define RDMACROCARTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>

class RDMacroCartModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDMacroCartModel(unsigned cartnum,QObject *parent=0);
  ~RDMacroCartModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  unsigned cartNumber() const;
  int lineCount() const;
  bool isEndHandle(const QModelIndex &row) const;
  QString code(const QModelIndex &row) const;
  QString allCode() const;
  int totalLength() const;
  QModelIndex addLine(const QModelIndex &row,const QString &rml);
  void removeLine(const QModelIndex &row);
  void refresh(const QModelIndex &row,const QString &rml);
  void save() const;

 protected:
  void updateModel();


 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  unsigned d_cart_number;
};


#endif  // RDMACROCARTMODEL_H
