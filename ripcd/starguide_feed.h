// starguide_feed.h
//
// A feed data container class for the StarGuide III Satellite Receiver
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: starguide_feed.h,v 1.6 2010/07/29 19:32:38 cvs Exp $
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

#ifndef STARGUIDE_FEED_H
#define STARGUIDE_FEED_H

#include <qstring.h>

#include <rdmatrix.h>

class StarGuideFeed
{
 public:
  StarGuideFeed();
  int providerId() const;
  void setProviderId(int id);
  int serviceId() const;
  void setServiceId(int id);
  RDMatrix::Mode mode() const;
  void setMode(RDMatrix::Mode mode);
  void clear();

 private:
  int sg_provider_id;
  int sg_service_id;
  RDMatrix::Mode sg_mode;
};


#endif  // STARGUIDE_FEED_H
