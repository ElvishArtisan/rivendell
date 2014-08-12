// rdcastmanager.js
//
// Script for displaying an upload progress dialog in a web browser.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastmanager.js,v 1.2 2010/07/29 19:32:40 cvs Exp $
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

function StartProgressbar()
{
  var html='<table cellpadding=\"10\" cellspacing=\"0\" border=\"0\" width=\"600\" height=\"400\">\n'+
      '<tr height=\"200\" bgcolor=\"#e0e0e0\">'+
      '<td align=\"center\" valign=\"bottom\">\n'+
      '<big><big>File uploading, please stand by...</big></big></td></tr>\n'+
      '<tr bgcolor=\"#e0e0e0\"><td align=\"center\" valign=\"top\">\n'+
      '<img src=\"progressbar.gif\" border=\"1\"></td></tr>\n'+
      '</table>\n';

  document.getElementById("bigframe").innerHTML=html;
}

function PostCast()
{
  window.setTimeout('StartProgressbar()',10);
}
