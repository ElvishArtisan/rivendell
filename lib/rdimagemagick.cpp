// rdimagemagick.cpp
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

#include <Magick++.h>

#include "rdimagemagick.h"

QByteArray RDIMResizeImage(const QByteArray &src_image,const QSize &size)
{
  Magick::Image img(Magick::Blob(src_image.constData(),src_image.size()));
  Magick::Geometry dst_size(size.width(),size.height());
  Magick::Blob dst_blob;

  img.zoom(Magick::Geometry(size.width(),size.height()));
  img.write(&dst_blob);

  return QByteArray((const char *)dst_blob.data(),dst_blob.length());
}
