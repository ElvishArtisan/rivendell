// editcut.js
//
// Script for selecting cut label elements for the EditCut web method
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

function EditCut_Field(field)
{
    if(document.getElementById("USE_"+field).checked) {
	return '&'+field+'='+UrlEncode(document.getElementById(field).value);
    }
    return '';
}


function EditCut_MakePost()
{
    var form='COMMAND=15';
    form+='&LOGIN_NAME='+document.getElementById("LOGIN_NAME").value;
    form+='&PASSWORD='+document.getElementById("PASSWORD").value;
    form+='&CART_NUMBER='+document.getElementById("CART_NUMBER").value;
    form+='&CUT_NUMBER='+document.getElementById("CUT_NUMBER").value;

    form+=EditCut_Field("EVERGREEN");
    form+=EditCut_Field("DESCRIPTION");
    form+=EditCut_Field("OUTCUE");
    form+=EditCut_Field("ISRC");
    form+=EditCut_Field("ISCI");
    form+=EditCut_Field("START_DATETIME");
    form+=EditCut_Field("END_DATETIME");
    form+=EditCut_Field("MON");
    form+=EditCut_Field("TUE");
    form+=EditCut_Field("WED");
    form+=EditCut_Field("THU");
    form+=EditCut_Field("FRI");
    form+=EditCut_Field("SAT");
    form+=EditCut_Field("SUN");
    form+=EditCut_Field("START_DAYPART");
    form+=EditCut_Field("END_DAYPART");
    form+=EditCut_Field("WEIGHT");
    form+=EditCut_Field("START_POINT");
    form+=EditCut_Field("END_POINT");
    form+=EditCut_Field("FADEUP_POINT");
    form+=EditCut_Field("FADEDOWN_POINT");
    form+=EditCut_Field("SEGUE_START_POINT");
    form+=EditCut_Field("SEGUE_END_POINT");
    form+=EditCut_Field("HOOK_START_POINT");
    form+=EditCut_Field("HOOK_END_POINT");
    form+=EditCut_Field("TALK_START_POINT");
    form+=EditCut_Field("TALK_END_POINT");

    return form;
}

function EditCut_ShowPost()
{
    alert('form: '+EditCut_MakePost());
}


function EditCut_Submit()
{
    PostForm(EditCut_MakePost(),"http://localhost/rd-bin/rdxport.cgi");
}
