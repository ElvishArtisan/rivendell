// starguide_feed.cpp
//
// A feed data container class for the StarGuide III Satellite Receiver
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: starguide_feed.cpp,v 1.5 2010/07/29 19:32:38 cvs Exp $
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

#include <starguide_feed.h>


StarGuideFeed::StarGuideFeed()
{
}


int StarGuideFeed::providerId() const
{
  return sg_provider_id;
}


void StarGuideFeed::setProviderId(int id)
{
  sg_provider_id=id;
}


int StarGuideFeed::serviceId() const
{
  return sg_service_id;
}


void StarGuideFeed::setServiceId(int id)
{
  sg_service_id=id;
}


RDMatrix::Mode StarGuideFeed::mode() const
{
  return sg_mode;
}


void StarGuideFeed::setMode(RDMatrix::Mode mode)
{
  sg_mode=mode;
}


void StarGuideFeed::clear()
{
  sg_provider_id=-1;
  sg_service_id=-1;
  sg_mode=RDMatrix::Stereo;
}
