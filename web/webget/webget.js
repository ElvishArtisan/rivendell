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


function ProcessGet()
{
    form=new FormData();

    form.append('TICKET',Id('TICKET').value);
    form.append('direction','get');
    form.append('title',Id('title').value);
    form.append('preset',Id('preset').value);

    SendForm(form,'webget.cgi','get_spinner');
}


function ProcessPut()
{
    form=new FormData();

    form.append('TICKET',Id('TICKET').value);
    form.append('direction','put');
    form.append('group',Id('group').value);
    form.append('filename',Id('filename').files[0]);

    SendForm(form,'webget.cgi','put_spinner');
}


function SendForm(form,url,spinner_id)
{
    var http=new XMLHttpRequest();
    if(http==null) {
	return;
    }

    //
    // Send the form
    //
    Id(spinner_id).innerHTML='<img src="/rd-bin/donut-spinner.gif">';
    http.open("POST",url,true);
    http.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
    http.responseType='blob';
    http.send(form);

    //
    // Process the response
    //
    http.onload=function(e) {
	Id(spinner_id).innerHTML='';
	var blob=new Blob([this.response],
			  {type: http.getResponseHeader('content-type')});
	var f0=blob.type.split(';');
	if(f0[0]=='text/html') {
	    reader=new FileReader();
	    reader.addEventListener('loadend',()=> {
		if(http.status==403) {  // Ticket expired, display the login
		    document.open(http.getResponseHeader("Content-Type"));
		    document.write(reader.result);
		    document.close();
		}
		else {
		    alert(reader.result);
}
	    });
	    reader.readAsText(blob);
	}
	else {
	    if((f0[0]=='audio/x-mpeg')||
	       (f0[0]=='audio/x-wav')||
	       (f0[0]=='audio/ogg')||
	       (f0[0]=='audio/flac')) {
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
		alert('Unknown mimetype: '+f0[0]);
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


function TitleChanged()
{
    Id('get_button').disabled=Id('title').value.length==0;
}


function FilenameChanged()
{
    Id('put_button').disabled=Id('filename').value.length==0;
}
