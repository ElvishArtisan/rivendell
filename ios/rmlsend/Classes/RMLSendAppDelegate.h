//
//  RMLSendAppDelegate.h
//  RMLSend
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: RMLSendAppDelegate.h,v 1.2 2011/05/27 17:20:14 cvs Exp $
//                                                                             
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//                                                                              //   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#import <UIKit/UIKit.h>

@class RMLSendViewController;

@interface RMLSendAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RMLSendViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet RMLSendViewController *viewController;

@end

