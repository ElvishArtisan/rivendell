// rdfeedlog.h
//
// Functions for manipulating RSS feed log tables.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdfeedlog.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDFEEDLOG_H
#define RDFEEDLOG_H

#include <qstring.h>

void RDCreateFeedLog(QString keyname);
void RDDeleteFeedLog(QString keyname);
void RDDeleteCastCount(QString keyname,unsigned cast_id);
void RDDeleteCastCount(unsigned feed_id,unsigned cast_id);
void RDIncrementFeedCount(QString keyname);
void RDIncrementCastCount(QString keyname,unsigned cast_id);


#endif  // RDFEEDLOG_H
