// unity_feed.cpp
//
// A feed data container class for the Unity4000
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: unity_feed.cpp,v 1.5 2010/07/29 19:32:38 cvs Exp $
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

#include <unity_feed.h>


UnityFeed::UnityFeed()
{
}


QString UnityFeed::feed() const
{
  return unity_feed;
}


void UnityFeed::setFeed(QString feed)
{
  unity_feed=feed;
}


RDMatrix::Mode UnityFeed::mode() const
{
  return unity_mode;
}


void UnityFeed::setMode(RDMatrix::Mode mode)
{
  unity_mode=mode;
}


void UnityFeed::clear()
{
  unity_feed="";
  unity_mode=RDMatrix::Stereo;
}
