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

function RD_PostForm(form,url)
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
    var html=http.responseText;
    document.open(http.getResponseHeader("Content-Type"));
    document.write(html);
    document.close();
}


function RD_MakeMimeSeparator()
{
    sep='----------------------------';
    for(var i=0;i<27;i++) {
	num=Math.floor(Math.random()*10);
	sep+=num.toString();
    }
    return sep;
}


function RD_AddMimePart(name,value,sep,is_last)
{
    var form='Content-Disposition: form-data; name="'+name+'"\r\n';
    form+='\r\n';
    form+=value+'\r\n';
    form+=sep;
    if(is_last) {
	form+='--';
    }
    form+='\r\n';

    return form;
}


function RD_UrlEncode(str) {
    var ret=new String;

    for(i=0;i<str.length;i++) {
	switch(str.charAt(i)) {
	case '$':
	case '&':
	case '+':
	case ',':
	case '/':
	case ':':
	case ';':
	case '=':
	case '?':
	case '@':
	case ' ':
	case '"':
	case '<':
	case '>':
	case '#':
	case '%':
	case '{':
	case '}':
	case '|':
	case '\\':
	case '^':
	case '~':
	case '[':
	case ']':
	case '`':
	    ret+=EncodeChar(str.charCodeAt(i));
	    break;

	default:
	    if((str.charCodeAt(i)<0x20)||(str.charCodeAt(i)>=0x7F)) {
		ret+=EncodeChar(str.charCodeAt(i));
	    }
	    else {
		ret+=str.charAt(i);
	    }
	    break;
	}
    }
    return ret;
}


function RD_EncodeChar(c) {
    var ret=new String;
    ret="%";
    if(c<16) {
	ret+="0";
    }
    ret+=c.toString(16);
    return ret;
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


function RD_GetXMLHttpRequest() {
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
