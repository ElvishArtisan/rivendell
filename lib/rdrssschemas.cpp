// rdrssschemas.cpp
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

#include "rdrssschemas.h"

RDRssSchemas::RDRssSchemas()
{
  //
  // Names
  //
  c_names.push_back("Custom");     // CustomSchema
  c_names.push_back("RSS 2.0.2");  // Rss202Schema


  //
  // Minimum Images Sizes
  //
  c_minimum_image_sizes.push_back(QSize(0,0));      // CustomSchema
  c_minimum_image_sizes.push_back(QSize(88,31));    // Rss202Schema


  //
  // Maximum Image Sizes
  //
  c_maximum_image_sizes.push_back(QSize(0,0));      // CustomSchema
  c_maximum_image_sizes.push_back(QSize(144,400));  // Rss202Schema


  //
  // Header Templates
  //
  // CustomSchema
  c_header_templates.push_back("");

  // Rss202Schema
  c_header_templates.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");


  //
  // Channel Templates
  //
  // CustomSchema
  c_channel_templates.push_back("");

  // Rs202Schema
  c_channel_templates.push_back("<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<image>\n  <url>%IMAGE_URL%</url>\n  <title>%TITLE%</title>\n  <link>%LINK%</link>\n  <width>%IMAGE_WIDTH%</width>\n  <height>%IMAGE_HEIGHT%</height>\n  <description>%IMAGE_DESCRIPTION%</description>\n</image>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />");


  //
  // Item Templates
  //
  // CustomSchema
  c_item_templates.push_back("");

  // Rss202Schema
  c_item_templates.push_back("<title>%ITEM_TITLE%</title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<author>%ITEM_AUTHOR%</author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\"  type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>");
}


QString RDRssSchemas::name(RssSchema schema) const
{
  return c_names.at(schema);
}


QSize RDRssSchemas::minimumImageSize(RssSchema schema) const
{
  return c_minimum_image_sizes.at(schema);
}


QSize RDRssSchemas::maximumImageSize(RssSchema schema) const
{
  return c_maximum_image_sizes.at(schema);
}


QString RDRssSchemas::headerTemplate(RssSchema schema) const
{
  return c_header_templates.at(schema);
}


QString RDRssSchemas::channelTemplate(RssSchema schema) const
{
  return c_channel_templates.at(schema);
}


QString RDRssSchemas::itemTemplate(RssSchema schema) const
{
  return c_item_templates.at(schema);
}
