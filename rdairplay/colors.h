// colors.h
//
// The color definitions for RDAirPlay.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef COLORS_H
#define COLORS_H

//
// Mode Titles
//
#define STOP_MODE_TITLE QObject::tr("START")
#define PLAY0_MODE_TITLE QObject::tr("STOP")
#define PLAY1_MODE_TITLE QObject::tr("PAUSE")
#define PAUSE_MODE_TITLE QObject::tr("RESUME")
#define ADD_TO_MODE_TITLE QObject::tr("WHERE?")
#define DELETE_FROM_MODE_TITLE QObject::tr("DELETE?")
#define MOVE_FROM_MODE_TITLE QObject::tr("MOVE?")
#define MOVE_TO_MODE_TITLE QObject::tr("TO?")
#define COPY_FROM_MODE_TITLE QObject::tr("COPY?")
#define COPY_TO_MODE_TITLE QObject::tr("TO?")
#define DISABLED_MODE_TITLE ""
#define ERROR_MODE_TITLE QObject::tr("ERROR")

//
// Wall Clock
//
#define BUTTON_TIME_SYNC_LOST_COLOR Qt::red

//
// Mode Button
//
#define BUTTON_MODE_LIVE_ASSIST_COLOR Qt::yellow
#define BUTTON_MODE_AUTO_COLOR Qt::green
#define BUTTON_MODE_MANUAL_COLOR Qt::red

//
// Log Selector Buttons
//
#define BUTTON_LOG_ACTIVE_TEXT_COLOR Qt::white
#define BUTTON_LOG_ACTIVE_BACKGROUND_COLOR Qt::blue

//
// Full Log Colors
//
#define LOG_NEXT_COLOR "#CCFFCC"
#define LOG_EVERGREEN_COLOR "#008000"

//
// Pie Counter Color
//
#define PIE_COUNTER_COLOR Qt::darkGreen
#define PIE_TALK_COLOR Qt::blue
#define PIE_FINAL_COLOR Qt::black
#define PIE_FINAL_BG_COLOR Qt::white
#define PIE_ONAIR_COLOR Qt::red

//
// Widget Colors
//
#define AIR_FLASH_COLOR Qt::blue
#define AIR_ERROR_COLOR Qt::red
#define AIR_WIDGET_BACKGROUND_COLOR "#D0D0D0"

//
// LogLineBox Colors
//
#define LOGLINEBOX_BACKGROUND_COLOR Qt::white
#define LOGLINEBOX_MISSING_COLOR Qt::red
#define LOGLINEBOX_MARKER_COLOR Qt::cyan
#define LOGLINEBOX_CHAIN_COLOR Qt::magenta
#define LOGLINEBOX_EVERGREEN_COLOR "#008000"
#define LOGLINEBOX_TIMESCALE_COLOR Qt::green

//
// Post Point Widget
//
#define POSTPOINT_EARLY_COLOR Qt::yellow
#define POSTPOINT_ONTIME_COLOR Qt::green
#define POSTPOINT_LATE_COLOR Qt::red

#endif  // COLORS_H
