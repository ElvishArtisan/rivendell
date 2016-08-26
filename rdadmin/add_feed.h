// add_feed.h
//
// Add a Rivendell Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_FEED_H
#define ADD_FEED_H

#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>

class AddFeed : public QDialog
{
  Q_OBJECT
  public:
   AddFeed(unsigned *id,QString *keyname,QWidget *parent=0);
   ~AddFeed();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QCheckBox *feed_users_box;
   QLineEdit *feed_keyname_edit;
   QString *feed_keyname;
   unsigned *feed_id;
};


#endif  // ADD_FEED_H
