<!-- rdcastmanager-item-enclosures.xsl

  Stylesheet for generating a list of RSS feed item enclosures.
  Copyright (C) 2002-2023 Fred Gleason <fredg@paravelsystems.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation;

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, 
  Boston, MA  02111-1307  USA
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:itunes="http://www.itunes.com/dtds/podcast-1.0.dtd">
  <xsl:output method="text" version="1.0" encoding="UTF-8" indent="yes"/>
  <xsl:template match="/">
    <xsl:for-each select="/rss/channel/item">
      <xsl:value-of select="enclosure/@url"/>
      <xsl:text>|</xsl:text>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
