// editcut.js
//
// Script for selecting cut label elements for the EditCut web method
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

function EditCut_Field(field,sep)
{
    if(document.getElementById("USE_"+field).checked) {
	return RD_AddMimePart(field,document.getElementById(field).value,sep,false);
    }
    return '';
}


function EditCut_MakePost()
{
    var sep=RD_MakeMimeSeparator();
    form=sep+"\r\n";

    form+=EditCut_Field('EVERGREEN',sep);
    form+=EditCut_Field('DESCRIPTION',sep);
    form+=EditCut_Field('OUTCUE',sep);
    form+=EditCut_Field('ISRC',sep);
    form+=EditCut_Field('ISCI',sep);
    form+=EditCut_Field('START_DATETIME',sep);
    form+=EditCut_Field('END_DATETIME',sep);
    form+=EditCut_Field('MON',sep);
    form+=EditCut_Field('TUE',sep);
    form+=EditCut_Field('WED',sep);
    form+=EditCut_Field('THU',sep);
    form+=EditCut_Field('FRI',sep);
    form+=EditCut_Field('SAT',sep);
    form+=EditCut_Field('SUN',sep);
    form+=EditCut_Field('START_DAYPART',sep);
    form+=EditCut_Field('END_DAYPART',sep);
    form+=EditCut_Field('START_POINT',sep);
    form+=EditCut_Field('END_POINT',sep);
    form+=EditCut_Field('FADEUP_POINT',sep);
    form+=EditCut_Field('FADEDOWN_POINT',sep);
    form+=EditCut_Field('SEGUE_START_POINT',sep);
    form+=EditCut_Field('SEGUE_END_POINT',sep);
    form+=EditCut_Field('HOOK_START_POINT',sep);
    form+=EditCut_Field('HOOK_END_POINT',sep);
    form+=EditCut_Field('TALK_START_POINT',sep);
    form+=EditCut_Field('TALK_END_POINT',sep);
    form+=EditCut_Field('WEIGHT',sep);
    form+=RD_AddMimePart('LOGIN_NAME',document.getElementById('LOGIN_NAME').value,sep,false);
    form+=RD_AddMimePart('PASSWORD',document.getElementById('PASSWORD').value,sep,false);
    form+=RD_AddMimePart('TICKET',document.getElementById('TICKET').value,sep,false);
    form+=RD_AddMimePart('CART_NUMBER',document.getElementById('CART_NUMBER').value,sep,false);
    form+=RD_AddMimePart('CUT_NUMBER',document.getElementById('CUT_NUMBER').value,sep,false);
    form+=RD_AddMimePart('COMMAND','15',sep,true);

    return form;
}


function EditCut_ShowPost()
{
    alert('form: '+EditCut_MakePost());
}


function EditCut_Submit()
{
    RD_PostForm(EditCut_MakePost(),"rdxport.cgi");
}
