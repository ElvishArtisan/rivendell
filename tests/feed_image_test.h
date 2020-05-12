// feed_image_test.h
//
// Test Rivendell image storage.
//
//   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef FEED_IMAGE_TEST_H
#define FEED_IMAGE_TEST_H

#include <qobject.h>

#include <rdfeed.h>

#define FEED_IMAGE_TEST_USAGE "[options]\n\nTest the Rivendell binary image routines\n\nOptions are:\n--push | --pop | --list\n\n--description=<str>\n\n--name=<str>\n\n--feed=<key-name>\n\n--image-id=<id>\n\n"

class MainObject : public QObject
{
 public:
  enum Command {None=0,List=1,Push=2,Pop=3};
  MainObject(QObject *parent=0);

 private:
  void RunList();
  void RunPush();
  void RunPop();
  QString test_filename;
  QString test_description;
  QString test_name;
  RDFeed *test_feed;
  int test_image_id;
};


#endif  // FEED_IMAGE_TEST_H
