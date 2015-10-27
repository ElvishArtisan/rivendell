// editcart.js
//
// Script for selecting cart label elements for the EditCart web method
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

function EditCart_Field(field)
{
    if(document.getElementById("USE_"+field).checked) {
	return '&'+field+'='+document.getElementById(field).value;
    }
    return '';
}


function EditCart_MakePost()
{
    var form='COMMAND=14';
    form+='&LOGIN_NAME='+document.getElementById("LOGIN_NAME").value;
    form+='&PASSWORD='+document.getElementById("PASSWORD").value;
    form+='&CART_NUMBER='+document.getElementById("CART_NUMBER").value;
    form+=EditCart_Field("GROUP_NAME");
    form+=EditCart_Field("TITLE");
    form+=EditCart_Field("ARTIST");
    form+=EditCart_Field("YEAR");
    form+=EditCart_Field("SONG_ID");
    form+=EditCart_Field("ALBUM");
    form+=EditCart_Field("LABEL");
    form+=EditCart_Field("CLIENT");
    form+=EditCart_Field("AGENCY");
    form+=EditCart_Field("PUBLISHER");
    form+=EditCart_Field("COMPOSER");
    form+=EditCart_Field("CONDUCTOR");
    form+=EditCart_Field("USER_DEFINED");
    form+=EditCart_Field("NOTES");

    return form;
}

function EditCart_ShowPost()
{
    alert('form: '+EditCart_MakePost());
}


function EditCart_Submit()
{
    PostForm(EditCart_MakePost(),"http://localhost/rd-bin/rdxport.cgi");
}
