// splashscreen.cpp
//
// Splash screen for rdairplay(1)
//
//   (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

#include "splashscreen.h"

#include "../icons/rdairplay-splash.xpm"

SplashScreen::SplashScreen(QWidget *main_win)
  : QSplashScreen(QPixmap(rdairplay_splash_xpm),Qt::WindowStaysOnTopHint)
{
  d_main_window=main_win;
}


void SplashScreen::showMessage(const QString &str)
{
  QSplashScreen::showMessage(str+" ",Qt::AlignRight|Qt::AlignBottom,Qt::white);
}


void SplashScreen::finish()
{
  QSplashScreen::finish(d_main_window);
}
