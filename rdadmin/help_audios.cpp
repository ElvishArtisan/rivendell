// help_audios.cpp
//
// Display help for audio ports (edit_audios.*)
//
// (C) Copyright 2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: help_audios.cpp,v 1.7 2012/02/13 19:26:14 cvs Exp $
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
//

#include <qpushbutton.h>

#include <help_audios.h>


HelpAudioPorts::HelpAudioPorts(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Audio Ports Help"));

  //
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);

  // Help Text
  help_edit=new QTextEdit(this,"help_edit");
  help_edit->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-70);
  help_edit->setTextFormat(RichText);
  help_edit->setReadOnly(true);
// FIXME: add context sensitive help for other options on the Audio Port screen.
  help_edit->setText(tr("Mode - short for Channel Mode, configures the Left and Right behaviour when recording.<BR>\
  Behaviour varies depending on the number of channels to record as summarized in the table below:<BR>\
<TABLE>\
<TR> <TH>Channels</TH>  <TH>Mode</TH>   <TH>Effect</TH> </TR>\
<TR> <TD>Mono</TD>      <TD>Normal</TD> <TD>L+R sum to mono</TD> </TR>\
<TR> <TD>Mono</TD>      <TD>Swap</TD>   <TD>R+L sum to mono (same result as Normal)</TD> </TR>\
<TR> <TD>Mono</TD>      <TD>Left only</TD> <TD>L -> mono</TD> </TR>\
<TR> <TD>Mono</TD>      <TD>Right only</TD> <TD>R -> mono</TD> </TR>\
<TR> <TD>Stereo</TD>    <TD>Normal</TD> <TD>Stereo</TD> </TR>\
<TR> <TD>Stereo</TD>    <TD>Swap</TD>   <TD>Swapped stereo</TD> </TR>\
<TR> <TD>Stereo</TD>    <TD>Left only</TD> <TD>L -> to L channel only, R channel is silent</TD> </TR>\
<TR> <TD>Stereo</TD>    <TD>Right only</TD> <TD>R -> to R channel only, L channel is silent</TD> </TR>\
</TABLE>"));

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize HelpAudioPorts::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy HelpAudioPorts::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void HelpAudioPorts::closeData()
{
  done(0);
}
