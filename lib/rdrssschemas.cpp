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

#include <stdio.h>

#include "rdrssschemas.h"

RDRssSchemas::RDRssSchemas()
{
  //
  // Names
  //
  c_names.push_back("Custom");                    // CustomSchema
  c_names.push_back("RSS 2.0.2");                 // Rss202Schema
  c_names.push_back("Apple iTunes");              // AppleSchema
  c_names.push_back("Apple iTunes + Superfeed");  // AppleSuperfeedSchema


  //
  // Minimum Images Sizes
  //
  c_minimum_image_sizes.push_back(QSize(0,0));       // CustomSchema
  c_minimum_image_sizes.push_back(QSize(88,31));     // Rss202Schema
  c_minimum_image_sizes.push_back(QSize(1400,1400)); // AppleSchema
  c_minimum_image_sizes.push_back(QSize(1400,1400)); // AppleSuperfeedSchema


  //
  // Maximum Image Sizes
  //
  c_maximum_image_sizes.push_back(QSize(0,0));        // CustomSchema
  c_maximum_image_sizes.push_back(QSize(144,400));    // Rss202Schema
  c_maximum_image_sizes.push_back(QSize(3000,3000));  // AppleSchema
  c_maximum_image_sizes.push_back(QSize(3000,3000));  // AppleSuperfeedSchema

  //
  // Header Templates
  //
  // CustomSchema
  c_header_templates.push_back("");

  // Rss202Schema
  c_header_templates.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");

  // AppleSchema
  c_header_templates.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:itunes=\"http://www.itunes.com/dtds/podcast-1.0.dtd\" xmlns:content=\"http://purl.org/rss/1.0/modules/content/\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");

  // AppleSuperfeedSchema
  c_header_templates.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<rss version=\"2.0\" xmlns:itunes=\"http://www.itunes.com/dtds/podcast-1.0.dtd\" xmlns:content=\"http://purl.org/rss/1.0/modules/content/\" xmlns:atom=\"http://www.w3.org/2005/Atom\" xmlns:superfeed=\"http://www.rivendellaudio.org/dtds/superfeed-0.1.dtd\">");

  //
  // Channel Templates
  //
  // CustomSchema
  c_channel_templates.push_back("");

  // Rs202Schema
  c_channel_templates.push_back("<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<image>\n  <url>%IMAGE_URL%</url>\n  <title>%TITLE%</title>\n  <link>%LINK%</link>\n  <width>%IMAGE_WIDTH%</width>\n  <height>%IMAGE_HEIGHT%</height>\n  <description>%IMAGE_DESCRIPTION%</description>\n</image>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />");

  // AppleSchema
  c_channel_templates.push_back("<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<itunes:summary>%DESCRIPTION%</itunes:summary>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<image>\n  <url>%IMAGE_URL%</url>\n  <title>%TITLE%</title>\n  <link>%LINK%</link>\n  <width>%IMAGE_WIDTH%</width>\n  <height>%IMAGE_HEIGHT%</height>\n  <description>%IMAGE_DESCRIPTION%</description>\n</image>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />\n<itunes:author>%AUTHOR%</itunes:author>\n<itunes:type>episodic</itunes:type>\n<itunes:owner>\n <itunes:name>%OWNER_NAME%</itunes:name>\n  <itunes:email>%OWNER_EMAIL%</itunes:email>\n</itunes:owner>\n<itunes:image href=\"%IMAGE_URL%\" />\n%ITUNES_CATEGORY%\n<itunes:explicit>%EXPLICIT%</itunes:explicit>");

  // AppleSuperfeedSchema
  c_channel_templates.push_back("<title>%TITLE%</title>\n<description>%DESCRIPTION%</description>\n<itunes:summary>%DESCRIPTION%</itunes:summary>\n<category>%CATEGORY%</category>\n<link>%LINK%</link>\n<language>%LANGUAGE%</language>\n<copyright>%COPYRIGHT%</copyright>\n<lastBuildDate>%BUILD_DATE%</lastBuildDate>\n<pubDate>%PUBLISH_DATE%</pubDate>\n<managingEditor>%EDITOR%</managingEditor>\n<webMaster>%WEBMASTER%</webMaster>\n<generator>%GENERATOR%</generator>\n<image>\n  <url>%IMAGE_URL%</url>\n  <title>%TITLE%</title>\n  <link>%LINK%</link>\n  <width>%IMAGE_WIDTH%</width>\n  <height>%IMAGE_HEIGHT%</height>\n  <description>%IMAGE_DESCRIPTION%</description>\n</image>\n<atom:link href=\"%FEED_URL%\" rel=\"self\" type=\"application/rss+xml\" />\n<itunes:author>%AUTHOR%</itunes:author>\n<itunes:type>episodic</itunes:type>\n<itunes:owner>\n <itunes:name>%OWNER_NAME%</itunes:name>\n  <itunes:email>%OWNER_EMAIL%</itunes:email>\n</itunes:owner>\n<itunes:image href=\"%IMAGE_URL%\" />\n%ITUNES_CATEGORY%\n<itunes:explicit>%EXPLICIT%</itunes:explicit>");


  //
  // Item Image Support
  //
  // CustomSchema
  c_supports_item_images.push_back(true);

  // Rss202Schema
  c_supports_item_images.push_back(false);

  // AppleSchema
  c_supports_item_images.push_back(true);

  // AppleSuperfeedSchema
  c_supports_item_images.push_back(true);


  //
  // Item Category Support
  //
  // CustomSchema
  c_supports_item_categories.push_back(true);

  // Rss202Schema
  c_supports_item_categories.push_back(true);

  // AppleSchema
  c_supports_item_categories.push_back(false);

  // AppleSuperfeedSchema
  c_supports_item_categories.push_back(false);


  //
  // Item Link Support
  //
  // CustomSchema
  c_supports_item_links.push_back(true);

  // Rss202Schema
  c_supports_item_links.push_back(true);

  // AppleSchema
  c_supports_item_links.push_back(false);

  // AppleSuperfeedSchema
  c_supports_item_links.push_back(false);


  //
  // Item Comments Support
  //
  // CustomSchema
  c_supports_item_comments.push_back(true);

  // Rss202Schema
  c_supports_item_comments.push_back(true);

  // AppleSchema
  c_supports_item_comments.push_back(false);

  // AppleSuperfeedSchema
  c_supports_item_comments.push_back(false);


  //
  // Item Templates
  //
  // CustomSchema
  c_item_templates.push_back("");

  // Rss202Schema
  c_item_templates.push_back("<title>%ITEM_TITLE%</title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<author>%ITEM_AUTHOR%</author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\"  type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>");

  // AppleSchema
  c_item_templates.push_back("<title>%ITEM_TITLE%</title>\n<itunes:title>%ITEM_TITLE%</itunes:title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<itunes:summary>%ITEM_DESCRIPTION%</itunes:summary>\n<author>%ITEM_AUTHOR%</author>\n<itunes:author>%ITEM_AUTHOR%</itunes:author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\" type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>\n<itunes:duration>%ITEM_AUDIO_SECONDS%</itunes:duration>\n<itunes:image href=\"%ITEM_IMAGE_URL%\" />\n<itunes:explicit>%ITEM_EXPLICIT%</itunes:explicit>");

  // AppleSuperfeedSchema
  c_item_templates.push_back("<superfeed:channelTitle>%ITEM_CHANNEL_TITLE%</superfeed:channelTitle>\n<superfeed:channelDescription>%ITEM_CHANNEL_DESCRIPTION%</superfeed:channelDescription>\n<title>%ITEM_TITLE%</title>\n<itunes:title>%ITEM_TITLE%</itunes:title>\n<link>%ITEM_LINK%</link>\n<guid isPermaLink=\"false\">%ITEM_GUID%</guid>\n<description>%ITEM_DESCRIPTION%</description>\n<itunes:summary>%ITEM_DESCRIPTION%</itunes:summary>\n<author>%ITEM_AUTHOR%</author>\n<itunes:author>%ITEM_AUTHOR%</itunes:author>\n<comments>%ITEM_COMMENTS%</comments>\n<source url=\"%ITEM_SOURCE_URL%\">%ITEM_SOURCE_TEXT%</source>\n<enclosure url=\"%ITEM_AUDIO_URL%\" length=\"%ITEM_AUDIO_LENGTH%\" type=\"audio/mpeg\" />\n<category>%ITEM_CATEGORY%</category>\n<pubDate>%ITEM_PUBLISH_DATE%</pubDate>\n<itunes:duration>%ITEM_AUDIO_SECONDS%</itunes:duration>\n<itunes:image href=\"%ITEM_IMAGE_URL%\" />\n<itunes:explicit>%ITEM_EXPLICIT%</itunes:explicit>");

  //
  // Categories
  //
  QMap<QString,QStringList> empty_sub_category;

  c_categories.push_back(QStringList());  // CustomSchema
  c_sub_categories.push_back(empty_sub_category);

  c_categories.push_back(QStringList());  // Rss202Schema
  c_sub_categories.push_back(empty_sub_category);

  QStringList apple_categories;
  QMap<QString,QStringList> apple_sub_categories;

  //
  // Apple categories are from
  // https://help.apple.com/itc/podcasts_connect/#/itc9267a2f12
  //
  apple_categories.push_back("Arts");
  apple_sub_categories["Arts"]=QStringList();
  apple_sub_categories["Arts"].push_back("Books");
  apple_sub_categories["Arts"].push_back("Design");
  apple_sub_categories["Arts"].push_back("Fashion & Beauty");
  apple_sub_categories["Arts"].push_back("Food");
  apple_sub_categories["Arts"].push_back("Performing Arts");
  apple_sub_categories["Arts"].push_back("Visual Arts");

  apple_categories.push_back("Business");
  apple_sub_categories["Business"]=QStringList();
  apple_sub_categories["Business"].push_back("Careers");
  apple_sub_categories["Business"].push_back("Entrepreneurship");
  apple_sub_categories["Business"].push_back("Investing");
  apple_sub_categories["Business"].push_back("Management");
  apple_sub_categories["Business"].push_back("Marketing");
  apple_sub_categories["Business"].push_back("Non-Profit");

  apple_categories.push_back("Comedy");
  apple_sub_categories["Comedy"]=QStringList();
  apple_sub_categories["Comedy"].push_back("Comedy Interviews");
  apple_sub_categories["Comedy"].push_back("Improv");
  apple_sub_categories["Comedy"].push_back("Stand-Up");

  apple_categories.push_back("Education");
  apple_sub_categories["Education"]=QStringList();
  apple_sub_categories["Education"].push_back("Courses");
  apple_sub_categories["Education"].push_back("How To");
  apple_sub_categories["Education"].push_back("Language Learning");
  apple_sub_categories["Education"].push_back("Self-Improvement");

  apple_categories.push_back("Fiction");
  apple_sub_categories["Fiction"]=QStringList();
  apple_sub_categories["Fiction"].push_back("Comedy Fiction");
  apple_sub_categories["Fiction"].push_back("Drama");
  apple_sub_categories["Fiction"].push_back("Science Fiction");

  apple_categories.push_back("Government");
  apple_sub_categories["Government"]=QStringList();

  apple_categories.push_back("History");
  apple_sub_categories["History"]=QStringList();

  apple_categories.push_back("Health & Fitness");
  apple_sub_categories["Health & Fitness"]=QStringList();
  apple_sub_categories["Health & Fitness"].push_back("Alternative Health");
  apple_sub_categories["Health & Fitness"].push_back("Fitness");
  apple_sub_categories["Health & Fitness"].push_back("Medicine");
  apple_sub_categories["Health & Fitness"].push_back("Mental Health");
  apple_sub_categories["Health & Fitness"].push_back("Nutrition");
  apple_sub_categories["Health & Fitness"].push_back("Sexuality");

  apple_categories.push_back("Kids & Family");
  apple_sub_categories["Kids & Family"]=QStringList();
  apple_sub_categories["Kids & Family"].push_back("Education for Kids");
  apple_sub_categories["Kids & Family"].push_back("Parenting");
  apple_sub_categories["Kids & Family"].push_back("Pets & Animals");
  apple_sub_categories["Kids & Family"].push_back("Stories for Kids");

  apple_categories.push_back("Leisure");
  apple_sub_categories["Leisure"]=QStringList();
  apple_sub_categories["Leisure"].push_back("Animation & Manga");
  apple_sub_categories["Leisure"].push_back("Automotive");
  apple_sub_categories["Leisure"].push_back("Aviation");
  apple_sub_categories["Leisure"].push_back("Crafts");
  apple_sub_categories["Leisure"].push_back("Games");
  apple_sub_categories["Leisure"].push_back("Hobbies");
  apple_sub_categories["Leisure"].push_back("Home & Garden");
  apple_sub_categories["Leisure"].push_back("Video Games");

  apple_categories.push_back("Music");
  apple_sub_categories["Music"]=QStringList();
  apple_sub_categories["Music"].push_back("Music Commentary");
  apple_sub_categories["Music"].push_back("Music History");
  apple_sub_categories["Music"].push_back("Music Interviews");

  apple_categories.push_back("News");
  apple_sub_categories["News"]=QStringList();
  apple_sub_categories["News"].push_back("Business News");
  apple_sub_categories["News"].push_back("Daily News");
  apple_sub_categories["News"].push_back("Entertainment News");
  apple_sub_categories["News"].push_back("News Commentary");
  apple_sub_categories["News"].push_back("Politics");
  apple_sub_categories["News"].push_back("Sports News");
  apple_sub_categories["News"].push_back("Tech News");

  apple_categories.push_back("Religion & Spirituality");
  apple_sub_categories["Religion & Spirituality"]=QStringList();
  apple_sub_categories["Religion & Spirituality"].push_back("Buddhism");
  apple_sub_categories["Religion & Spirituality"].push_back("Christianity");
  apple_sub_categories["Religion & Spirituality"].push_back("Hinduism");
  apple_sub_categories["Religion & Spirituality"].push_back("Islam");
  apple_sub_categories["Religion & Spirituality"].push_back("Judaism");
  apple_sub_categories["Religion & Spirituality"].push_back("Religion");
  apple_sub_categories["Religion & Spirituality"].push_back("Spirituality");

  apple_categories.push_back("Science");
  apple_sub_categories["Science"]=QStringList();
  apple_sub_categories["Science"].push_back("Astronomy");
  apple_sub_categories["Science"].push_back("Chemistry");
  apple_sub_categories["Science"].push_back("Earth Sciences");
  apple_sub_categories["Science"].push_back("Life Sciences");
  apple_sub_categories["Science"].push_back("Mathematics");
  apple_sub_categories["Science"].push_back("Natural Sciences");
  apple_sub_categories["Science"].push_back("Nature");
  apple_sub_categories["Science"].push_back("Physics");
  apple_sub_categories["Science"].push_back("Social Sciences");

  apple_categories.push_back("Society & Culture");
  apple_sub_categories["Society & Culture"]=QStringList();
  apple_sub_categories["Society & Culture"].push_back("Documentary");
  apple_sub_categories["Society & Culture"].push_back("Personal Journals");
  apple_sub_categories["Society & Culture"].push_back("Philosophy");
  apple_sub_categories["Society & Culture"].push_back("Places & Travel");
  apple_sub_categories["Society & Culture"].push_back("Relationships");

  apple_categories.push_back("Sports");
  apple_sub_categories["Sports"]=QStringList();
  apple_sub_categories["Sports"].push_back("Baseball");
  apple_sub_categories["Sports"].push_back("Basketball");
  apple_sub_categories["Sports"].push_back("Cricket");
  apple_sub_categories["Sports"].push_back("Fantasy Sports");
  apple_sub_categories["Sports"].push_back("Football");
  apple_sub_categories["Sports"].push_back("Golf");
  apple_sub_categories["Sports"].push_back("Hockey");
  apple_sub_categories["Sports"].push_back("Rugby");
  apple_sub_categories["Sports"].push_back("Running");
  apple_sub_categories["Sports"].push_back("Soccer");
  apple_sub_categories["Sports"].push_back("Swimming");
  apple_sub_categories["Sports"].push_back("Tennis");
  apple_sub_categories["Sports"].push_back("Volleyball");
  apple_sub_categories["Sports"].push_back("Wilderness");
  apple_sub_categories["Sports"].push_back("Wrestling");

  apple_categories.push_back("Technology");
  apple_sub_categories["Technology"]=QStringList();

  apple_categories.push_back("True Crime");
  apple_sub_categories["True Crime"]=QStringList();

  apple_categories.push_back("TV & Film");
  apple_sub_categories["TV & Film"]=QStringList();
  apple_sub_categories["TV & Film"].push_back("After Shows");
  apple_sub_categories["TV & Film"].push_back("Film History");
  apple_sub_categories["TV & Film"].push_back("Film Interviews");
  apple_sub_categories["TV & Film"].push_back("Film Reviews");
  apple_sub_categories["TV & Film"].push_back("TV Reviews");

  c_categories.push_back(apple_categories);  // AppleSchema
  c_sub_categories.push_back(apple_sub_categories);
  
  c_categories.push_back(apple_categories);  // AppleSuperfeedSchema
  c_sub_categories.push_back(apple_sub_categories);
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


bool RDRssSchemas::supportsItemImages(RssSchema schema) const
{
  return c_supports_item_images.at(schema);
}


bool RDRssSchemas::supportsItemCategories(RssSchema schema) const
{
  return c_supports_item_categories.at(schema);
}


bool RDRssSchemas::supportsItemLinks(RssSchema schema) const
{
  return c_supports_item_links.at(schema);
}


bool RDRssSchemas::supportsItemComments(RssSchema schema) const
{
  return c_supports_item_comments.at(schema);
}


QStringList RDRssSchemas::categories(RDRssSchemas::RssSchema schema) const
{
  return c_categories.at((int)schema);
}


QStringList RDRssSchemas::subCategories(RssSchema schema,
					const QString &category) const
{
  return c_sub_categories.at((int)schema).value(category);
}
