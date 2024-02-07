// add_feed.h
//
// Add a Rivendell Feed
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>

#include <rddialog.h>
#include <rdfeedlistmodel.h>

class AddFeed : public RDDialog
{
  Q_OBJECT
 public:
  AddFeed(unsigned *id,QString *keyname,QStringList *usernames,QWidget *parent);
  ~AddFeed();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void keynameChangedData(const QString &str);
  void buttonToggledData(QAbstractButton *rbutton,bool checked);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void AuthorizeUser(const QString &keyname,const QString &login_name) const;
  QLabel *feed_keyname_label;
  QLineEdit *feed_keyname_edit;
  QGroupBox *feed_usernames_groupbox;
  QButtonGroup *feed_usernames_group;
  QRadioButton *feed_none_radio;
  QRadioButton *feed_all_radio;
  QRadioButton *feed_some_radio;
  QComboBox *feed_keyname_box;
  RDFeedListModel *feed_keyname_model;
  QPushButton *feed_ok_button;
  QPushButton *feed_cancel_button;
  QString *feed_keyname;
  unsigned *feed_id;
  QStringList *feed_usernames;
};


#endif  // ADD_FEED_H
