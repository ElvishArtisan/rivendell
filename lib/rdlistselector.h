//   rdlistselector.h
//
//   An listselector widget with word wrap.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdlistselector.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDLISTSELECTOR_H
#define RDLISTSELECTOR_H

#include <qwidget.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcolor.h>
#include <qlistbox.h>
#include <qpushbutton.h>


class RDListSelector : public QHBox
{
  Q_OBJECT

 public:
  RDListSelector(QWidget *parent=0,const char *name=0);
  uint sourceCount() const;
  uint destCount() const;
  void sourceSetLabel(QString label);
  void destSetLabel(QString label);
  void sourceInsertItem(const QString &text,int index=-1);
  void destInsertItem(const QString &text,int index=-1);
  void sourceRemoveItem(int index);
  void destRemoveItem(int index);
  QString sourceText(int index) const;
  QString destText(int index) const;
  void sourceChangeItem(const QString &text,int index);
  void destChangeItem(const QString &text,int index);
  int sourceNumItemsVisible() const;
  int destNumItemsVisible() const;
  int sourceCurrentItem() const;
  int destCurrentItem() const;
  QString sourceCurrentText() const;
  QString destCurrentText() const;
  void sourceSetCurrentItem(int item);
  void destSetCurrentItem(int item);
  QListBoxItem *sourceFindItem(const QString &text,
			       ComparisonFlags compare=ExactMatch) const;
  QListBoxItem *destFindItem(const QString &text,
			     ComparisonFlags compare=ExactMatch) const;
  void clear();
  
 private slots:
  void addData();
  void removeData();

 private:
  void CheckButtons();
  QListBox *list_source_box;
  QLabel *list_source_label;
  QListBox *list_dest_box;
  QLabel *list_dest_label;
  QPushButton *list_add_button;
  QPushButton *list_remove_button;
};


#endif  // RDLISTSELECTOR_H
