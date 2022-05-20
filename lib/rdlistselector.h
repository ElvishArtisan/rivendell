//   rdlistselector.h
//
//   An listselector widget with word wrap.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLISTSELECTOR_H
#define RDLISTSELECTOR_H

#include <QPushButton>
#include <QListWidget>

#include <rdwidget.h>

class RDListSelector : public RDWidget
{
  Q_OBJECT
 public:
  RDListSelector(QWidget *parent=0);
  void setItemIcon(const QIcon &icon);
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
  int sourceCurrentItem() const;
  int destCurrentItem() const;
  QString sourceCurrentText() const;
  QString destCurrentText() const;
  void sourceSetCurrentItem(int item);
  void destSetCurrentItem(int item);
  QListWidgetItem *sourceFindItem(const QString &text,
				  Qt::MatchFlags flags=Qt::MatchExactly);
  QListWidgetItem *destFindItem(const QString &text,
				Qt::MatchFlags flags=Qt::MatchExactly);
		   void clear();
  
 private slots:
  void addData();
  void removeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void CheckButtons();
  QListWidget *list_source_list;
  QLabel *list_source_label;
  QListWidget *list_dest_list;
  QLabel *list_dest_label;
  QPushButton *list_add_button;
  QPushButton *list_remove_button;
  QIcon list_item_icon;		   
};


#endif  // RDLISTSELECTOR_H
