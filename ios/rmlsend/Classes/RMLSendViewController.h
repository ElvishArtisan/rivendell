//
//  RMLSendViewController.h
//  RMLSend
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: RMLSendViewController.h,v 1.3 2011/05/31 22:47:33 cvs Exp $
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

#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#import <UIKit/UIKit.h>

#define RMLSEND_TAG_IPADDR 0
#define RMLSEND_TAG_UDPPORT 1
#define RMLSEND_TAG_CMDLINE 2
#define RMLSEND_TAG_BUTTON 3

@interface RMLSendViewController : UIViewController <UITextFieldDelegate> {
	UITextField *ipaddr;
	UITextField *udpport;
	UITextField *cmdline;
	UIButton *button;
	int sock;
	struct sockaddr_in sa;
	NSString *conf_filename;
}

@property(nonatomic,retain) IBOutlet UITextField *ipaddr;
@property(nonatomic,retain) IBOutlet UITextField *udpport;
@property(nonatomic,retain) IBOutlet UITextField *cmdline;
@property(nonatomic,retain) IBOutlet UIButton *button;

-(IBAction) processSend: (id) sender;

-(BOOL) ValidateFields;
-(BOOL) SaveFields;
-(BOOL) RestoreFields;

//
// UITextFieldDelegate methods
//
-(BOOL) textFieldShouldBeginEditing:(UITextField *)textField;
-(void) textFieldDidBeginEditing:(UITextField *)textField;
-(BOOL) textFieldShouldEndEditing:(UITextField *)textField;
-(void) textFieldDidEndEditing:(UITextField *)textField;
-(BOOL) textField:(UITextField *)textField shouldChangeCharactersInRange: (NSRange) range 
	replacementString: (NSString *) string;
-(BOOL) textFieldShouldClear:(UITextField *)textField;
-(BOOL) textFieldShouldReturn: (UITextField *) textField;

@end

