// edit_superfeed.h
//
// Edit Rivendell Superfeed
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SUPERFEED_H
#define EDIT_SUPERFEED_H

#include <rddialog.h>
#include <rdlistselector.h>

#include <rdfeed.h>

class EditSuperfeed : public RDDialog
{
  Q_OBJECT
 public:
  EditSuperfeed(RDFeed *feed,QWidget *parent=0);
  ~EditSuperfeed();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 private:
  RDListSelector *feed_host_sel;
  RDFeed *feed_feed;
};


#endif  // EDIT_SUPERFEED_H
