// webget.js
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

function Id(id)
{
    return document.getElementById(id);
}


function MakePost()
{
    var sep=RD_MakeMimeSeparator();
    form=sep+"\r\n";

    form+=RD_AddMimePart('title',Id('title').value,sep,false);
    form+=RD_AddMimePart('preset',Id('preset').value,sep,false);
    form+=RD_AddMimePart('LOGIN_NAME',Id('LOGIN_NAME').value,sep,false);
    form+=RD_AddMimePart('PASSWORD',Id('PASSWORD').value,sep,true);

    return form;
}


function ProcessOkButton()
{
    SendForm(MakePost(),"webget.cgi");
}


function SendForm(form,url)
{
    var http=RD_GetXMLHttpRequest();
    if(http==null) {
	return;
    }

    //
    // Send the form
    //
    http.open("POST",url,true);
    http.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
    http.responseType='blob';
    http.send(form);

    //
    // Process the response
    //
    http.onload=function(e) {
	if(this.status==200) {
	    var blob=new Blob([this.response],
			      {type: http.getResponseHeader('content-type')});
	    let a=document.createElement('a');
	    a.style='display: none';
	    document.body.appendChild(a);
	    let url=window.URL.createObjectURL(blob);
	    a.href=url;
	    a.download=Id('title').value+'.'+FileExtension(Id('preset').value);
	    a.click();
	    window.URL.revokeObjectURL(url);
	}
	else {
	    if(this.status==401) {
		alert('Invalid User Name or Password!');
	    }
	    else {
		if(this.status=404) {
		    alert('No cart with that name found!');
		}
		else {
		    alert('Unable to access WebGet [response code: '+
			  http.status+']!');
		}
	    }
	}
    }
}


function FileExtension(prof_id)
{
    for(id in preset_ids) {
	if(preset_ids[id]==prof_id) {
	    return preset_exts[id];
	}
    }

    return 'dat';
}

