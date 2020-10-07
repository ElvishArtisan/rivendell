// rdrsscategorybox.h
//
// A Combo Box widget for RSS categories.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRSSCATEGORYBOX_H
#define RDRSSCATEGORYBOX_H

#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <QResizeEvent>

#include <rdwidget.h>

class RDRssCategoryBox : public RDWidget
{
  Q_OBJECT
 public:
  RDRssCategoryBox(QWidget *parent=0);
  ~RDRssCategoryBox();
  RDRssSchemas::RssSchema schema() const;
  void setSchema(RDRssSchemas::RssSchema schema);
  QString category() const;
  QString subCategory() const;

 public slots:
  void setCategory(const QString &category,const QString &sub_category);

 private slots:
  void boxActivatedData(const QString str);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshCategories(RDRssSchemas::RssSchema schema,const QString &category,
			 const QString &sub_category);
  void RefreshSubcategories(RDRssSchemas::RssSchema schema,
			    const QString &category,
			    const QString &sub_category);
  RDRssSchemas::RssSchema c_schema;
  QComboBox *c_box;
  QComboBox *c_sub_box;
  QLabel *c_seperator_label;
  QLineEdit *c_edit;
  QLineEdit *c_sub_edit;
};


#endif  // RDRSSCATEGORYBOX_H
