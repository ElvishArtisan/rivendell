/* rdxport_interface.h
 *
 * Public Interface for the RDXport Service
 *
 *   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef RDXPORT_INTERFACE_H
#define RDXPORT_INTERFACE_H

#define RDXPORT_COMMAND_EXPORT 1
#define RDXPORT_COMMAND_IMPORT 2
#define RDXPORT_COMMAND_DELETEAUDIO 3
#define RDXPORT_COMMAND_LISTGROUPS 4
#define RDXPORT_COMMAND_LISTGROUP 5
#define RDXPORT_COMMAND_LISTCARTS 6
#define RDXPORT_COMMAND_LISTCART 7
#define RDXPORT_COMMAND_LISTCUT 8
#define RDXPORT_COMMAND_LISTCUTS 9
#define RDXPORT_COMMAND_ADDCUT 10
#define RDXPORT_COMMAND_REMOVECUT 11
#define RDXPORT_COMMAND_ADDCART 12
#define RDXPORT_COMMAND_REMOVECART 13
#define RDXPORT_COMMAND_EDITCART 14
#define RDXPORT_COMMAND_EDITCUT 15
#define RDXPORT_COMMAND_EXPORT_PEAKS 16
#define RDXPORT_COMMAND_TRIMAUDIO 17
#define RDXPORT_COMMAND_COPYAUDIO 18
#define RDXPORT_COMMAND_AUDIOINFO 19
#define RDXPORT_COMMAND_LISTLOGS 20
#define RDXPORT_COMMAND_LISTSERVICES 21
#define RDXPORT_COMMAND_LISTLOG 22

#endif  // RDXPORT_INTERFACE_H
