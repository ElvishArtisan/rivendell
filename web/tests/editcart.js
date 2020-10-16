// editcart.js
//
// Script for selecting cart label elements for the EditCart web method
//
//   (C) Copyright 2015-2020 Fred Gleason <fredg@paravelsystems.com>
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

function EditCart_Field(field,sep)
{
    if(document.getElementById("USE_"+field).checked) {
	return RD_AddMimePart(field,document.getElementById(field).value,sep,false);
    }
    return '';
}


function EditCart_MakePost()
{
    var sep=RD_MakeMimeSeparator();
    form=sep+"\r\n";

    form+=EditCart_Field('ASYNCHRONOUS',sep);
    form+=EditCart_Field('ENFORCE_LENGTH',sep);
    form+=EditCart_Field('GROUP_NAME',sep);
    form+=EditCart_Field('TITLE',sep);
    form+=EditCart_Field('ARTIST',sep);
    form+=EditCart_Field('YEAR',sep);
    form+=EditCart_Field('SONG_ID',sep);
    form+=EditCart_Field('ALBUM',sep);
    form+=EditCart_Field('LABEL',sep);
    form+=EditCart_Field('CLIENT',sep);
    form+=EditCart_Field('AGENCY',sep);
    form+=EditCart_Field('PUBLISHER',sep);
    form+=EditCart_Field('COMPOSER',sep);
    form+=EditCart_Field('CONDUCTOR',sep);
    form+=EditCart_Field('USER_DEFINED',sep);
    form+=EditCart_Field('OWNER',sep);
    form+=EditCart_Field('NOTES',sep);
    form+=RD_AddMimePart('LOGIN_NAME',document.getElementById('LOGIN_NAME').value,sep,false);
    form+=RD_AddMimePart('PASSWORD',document.getElementById('PASSWORD').value,sep,false);
    form+=RD_AddMimePart('TICKET',document.getElementById('TICKET').value,sep,false);
    form+=RD_AddMimePart('CART_NUMBER',document.getElementById('CART_NUMBER').value,sep,false);
    if(document.getElementById("INCLUDE_CUTS").value.length==0) {
	form+=RD_AddMimePart('INCLUDE_CUTS',document.getElementById('INCLUDE_CUTS').value,sep,false);
    }
    form+=RD_AddMimePart('COMMAND','14',sep,true);

    return form;
}


function EditCart_ShowPost()
{
    alert('form: '+EditCart_MakePost());
}


function EditCart_Submit()
{
    RD_PostForm(EditCart_MakePost(),"rdxport.cgi");
}
