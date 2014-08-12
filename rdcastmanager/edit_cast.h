// edit_cast.h
//
// Edit a Rivendell Cast
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cast.h,v 1.8 2011/09/09 20:23:28 cvs Exp $
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

#ifndef EDIT_CAST_H
#define EDIT_CAST_H

#include <vector>

#include <qdialog.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

#include <rdfeed.h>
#include <rdpodcast.h>


class EditCast : public QDialog
{
 Q_OBJECT
 public:
  EditCast(unsigned cast_id,QWidget *parent=0,const char *name=0);
  ~EditCast();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void expirationSelectedData(int state);
  void effectiveSelectData();
  void expirationSelectData();
  void reportData();
  void okData();
  void cancelData();
  
 private:
  RDFeed *cast_feed;
  RDPodcast *cast_cast;
  QLineEdit *cast_item_medialink_edit;
  QLineEdit *cast_item_title_edit;
  QLineEdit *cast_item_author_edit;
  QLineEdit *cast_item_origin_edit;
  QTextEdit *cast_item_description_edit;
  QLineEdit *cast_item_category_edit;
  QLineEdit *cast_item_comments_edit;
  QLineEdit *cast_item_sourcetext_edit;
  QLineEdit *cast_item_sourceurl_edit;
  QLineEdit *cast_item_link_edit;
  QComboBox *cast_item_expiration_box;
  QLabel *cast_item_expiration_label;
  QPushButton *cast_item_expiration_button;
  QDateEdit *cast_item_expiration_edit;
  QDateTimeEdit *cast_item_effective_edit;
  QButtonGroup *cast_item_status_group;
  std::vector<QString> cast_aux_varnames;
  std::vector<QLineEdit *> cast_aux_edits;
  int cast_ypos;
  RDPodcast::Status cast_status;
};


#endif  // EDIT_CAST_H

