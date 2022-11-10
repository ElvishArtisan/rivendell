// rdimagemagick.h
//
// ImageMagick operations for Rivendell
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDIMAGEMAGICK_H
#define RDIMAGEMAGICK_H

#include <QByteArray>
#include <QSize>

QByteArray RDIMResizeImage(const QByteArray &src_image,const QSize &size,
			   QString *err_msg);


#endif  // RDIMAGEMAGICK_H
