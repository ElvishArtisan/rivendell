<!-- rdadmin-back-item-report.xsl

  Stylesheet for generating the Back Item Report in rdadmin(1)
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

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:itunes="http://www.itunes.com/dtds/podcast-1.0.dtd" xmlns:rivendell="http://www.rivendellaudio.org/dtds/rivendell-0.1.dtd">
  <xsl:output method="html" version="1.0" encoding="UTF-8" indent="yes"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>
	  <xsl:value-of select="/rss/channel/title"/> Rivendell Back Item Report
	</title>
        <meta charset="UTF-8" />
        <meta http-equiv="x-ua-compatible" content="IE=edge,chrome=1" />
        <meta name="viewport" content="width=device-width,minimum-scale=1,initial-scale=1,shrink-to-fit=no" />
        <style type="text/css">
         body {
           background-color: darkgray;
         }
         .active-rounded-block {
           background-color: white;
           width: 800px;
           border-style: solid;
           border-width: 2px;
           border-radius: 10px;
           margin-bottom: 5px;
         }
         .missing-rounded-block {
           background-color: red;
           width: 800px;
           border-style: solid;
           border-width: 2px;
           border-radius: 10px;
           margin-bottom: 5px;
         }
         .image {
           vertical-align: text-top;
           margin-left: 5px;
           margin-right: 5px;
         }
         .center-aligned {
           font-family: Verdana,san serif;
           text-align: center;
           margin-top: 5px;
           margin-bottom: 5px;
           margin-left: 5px;
           margin-right: 5px;
         }
         .left-aligned {
           font-family: Helvetica,serif;
           text-align: left;
           margin-top: 5px;
           margin-left: 5px;
           margin-right: 5px;
         }
         .datetime {
           font-family: Helvetica,serif;
           text-align: right;
           margin-top: 5px;
           margin-left: 5px;
           margin-right: 5px;
         }
        </style>
      </head>
      <body>
        <header>
         <table class="active-rounded-block">
           <tr>
             <td width="1024" colspan="2">
               <h1 class="center-aligned">Rivendell Back Item Report</h1>
             </td>
           </tr>
         </table>
        </header>
        <main>
         <!-- Channel Info -->
         <table class="active-rounded-block">
           <tr>
             <td class="image" width="96">
               <img class="image" width="96" height="96">
                 <xsl:attribute name="src">
                   <xsl:value-of select="/rss/channel/image/url" />
                 </xsl:attribute>
               </img>
             </td>
             <td>
               <table>
                 <tr>
                   <td class="left-aligned">
                     <h2>
                       <a target="_blank">
                         <xsl:attribute name="href">
                           <xsl:value-of select="/rss/channel/link"/>
                         </xsl:attribute>
                         <xsl:attribute name="hreflang">
                           <xsl:value-of select="/rss/channel/language"/>
                         </xsl:attribute>
                         <xsl:value-of select="/rss/channel/title"/>
                       </a>
                     </h2>
                   </td>
                 </tr>
                 <tr>
                   <td class="left-aligned">
                   <xsl:value-of select="/rss/channel/description"/></td>
                 </tr>
               </table>
             </td>
           </tr>
         </table>

         <!-- Item Info -->
         <xsl:for-each select="/rss/channel/item">
           <table>
	     <xsl:attribute name="class">
	       <xsl:value-of select="@rivendell:style"/>
	     </xsl:attribute>
             <tr>
               <td><h3>
                 <a class="left-aligned" target="_blank">
                    <xsl:attribute name="href">
                     <xsl:value-of select="enclosure/@url"/>
                    </xsl:attribute>
                   <xsl:attribute name="hreflang">
                     <xsl:value-of select="/rss/channel/language"/>
                   </xsl:attribute>
                    <xsl:value-of select="title"/>
                  </a>
               </h3></td>
               <td class="datetime"><h4>
                    <time class="datetime">
                     <xsl:value-of select="pubDate" />
                    </time>
                 </h4></td>
             </tr>
             <tr>
               <td class="left-aligned" colspan="2">
                 <xsl:value-of select="description"/>
               </td>
             </tr>
           </table>
         </xsl:for-each>
        </main>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
