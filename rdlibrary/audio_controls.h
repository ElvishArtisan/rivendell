// audio_controls.h
//
// A Container Class for RDLibrary Cart Label Controls
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audio_controls.h,v 1.6.8.1 2013/12/11 18:51:48 cvs Exp $
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

#ifndef AUDIO_CONTROLS_H
#define AUDIO_CONTROLS_H

#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qspinbox.h>

#include <rdtimeedit.h>

class AudioControls
{
 public:
  QCheckBox *enforce_length_box;
  RDTimeEdit *forced_length_edit;
  QLineEdit *song_id_edit;
  QSpinBox *bpm_spin;
  QLineEdit *title_edit;
  QLineEdit *artist_edit;
  QLineEdit *album_edit;
  QLineEdit *year_edit;
  QLineEdit *label_edit;
  QLineEdit *client_edit;
  QLineEdit *agency_edit;
  QLineEdit *publisher_edit;
  QLineEdit *conductor_edit;
  QLineEdit *composer_edit;
  QLineEdit *user_defined_edit;
};


#endif  // AUDIO_CONTROLS_H

