// rdrssschemas.h
//
// RSS schema definitions for Rivendell
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRSSSCHEMAS_H
#define RDRSSSCHEMAS_H

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qsize.h>
#include <qstringlist.h>

class RDRssSchemas
{
 public:
  enum RssSchema {CustomSchema=0,Rss202Schema=1,AppleSchema=2,
		  AppleSuperfeedSchema=3,LastSchema=4};
  RDRssSchemas();
  QString name(RssSchema schema) const;
  QSize minimumImageSize(RssSchema schema) const;
  QSize maximumImageSize(RssSchema schema) const;
  QString headerTemplate(RssSchema schema) const;
  QString channelTemplate(RssSchema schema) const;
  QString itemTemplate(RssSchema schema) const;
  bool supportsItemImages(RssSchema schema) const;
  bool supportsItemCategories(RssSchema schema) const;
  bool supportsItemLinks(RssSchema schema) const;
  bool supportsItemComments(RssSchema schema) const;
  QStringList categories(RssSchema schema) const;
  QStringList subCategories(RssSchema schema,const QString &category) const;

 private:
  QStringList c_names;
  QList<QSize> c_minimum_image_sizes;
  QList<QSize> c_maximum_image_sizes;
  QStringList c_header_templates;
  QStringList c_channel_templates;
  QStringList c_item_templates;
  QList<bool> c_supports_item_images;
  QList<bool> c_supports_item_categories;
  QList<bool> c_supports_item_links;
  QList<bool> c_supports_item_comments;
  QList<QStringList> c_categories;
  QList<QMap<QString,QStringList> > c_sub_categories;
};


#endif  // RDRSSSCHEMAS_H
