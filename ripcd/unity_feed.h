// unity_feed.h
//
// A feed data container class for the Unity4000
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: unity_feed.h,v 1.6 2010/07/29 19:32:38 cvs Exp $
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

#ifndef UNITY_FEED_H
#define UNITY_FEED_H

#include <qstring.h>

#include <rdmatrix.h>

class UnityFeed
{
 public:
  UnityFeed();
  QString feed() const;
  void setFeed(QString feed);
  RDMatrix::Mode mode() const;
  void setMode(RDMatrix::Mode mode);
  void clear();

 private:
  QString unity_feed;
  RDMatrix::Mode unity_mode;
};


#endif  // UNITY_FEED_H
