// utils.js
//
// Common java script utility functions.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

function PostForm(form,url)
{
    var http=GetXMLHttpRequest();
    if(http==null) {
	return;
    }

    //
    // Send the form
    //
    http.open("POST",url,false);
    http.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
    http.send(form);

    //
    // Process the response
    //
    if(http.status==200) {
	var html=http.responseText;
	document.open();
	document.write(html);
	document.close();
    }
}


var http_factory=null;
var http_factories=[
    function() {
	return new XMLHttpRequest();
    },
    function() {
	return new ActiveXObject("Microsoft.XMLHTTP");
    },
    function() {
	return new ActiveXObject("MSXML2.XMLHTTP.3.0");
    },
    function() {
	return new ActiveXObject("MSXML2.XMLHTTP");
    }
];


function GetXMLHttpRequest() {
    for(var i=0;i<http_factories.length;i++) {
	try {
	    var factory=http_factories[i];
	    var request=factory();
	    if(request!=null) {
		http_factory=factory;
		return request;
	    }
	}
	catch(e) {
	    continue;
	}
    }
    return null;
}
