//
//  RMLSendViewController.m
//  RMLSend
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: RMLSendViewController.m,v 1.3 2011/05/31 22:47:33 cvs Exp $
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
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
//

#include <unistd.h>
#include <sys/socket.h>

#import "RMLSendViewController.h"
#import "TextFieldDelegate.h"

@implementation RMLSendViewController

@synthesize ipaddr,udpport,cmdline,button;

-(IBAction) processSend: (id) sender
{
	sendto(sock,[cmdline.text UTF8String],cmdline.text.length,0,(struct sockaddr *)(&sa),sizeof(sa));
}


-(BOOL) ValidateFields
{
	struct hostent *host=NULL;

	//
	// IP Address
	//
	if([ipaddr.text length]<1) {
		return NO;
	}
	if((host=gethostbyname([ipaddr.text UTF8String]))==NULL) {
		NSString *err=[[NSString alloc] initWithUTF8String: hstrerror(h_errno)];
		UIAlertView *alert=[[UIAlertView alloc] 
							initWithTitle: @"RMLSend"
							message: err
							delegate:nil
							cancelButtonTitle:nil
							otherButtonTitles:@"OK",nil];
		[alert show];
		[alert release];
		[err release];
		return NO;
	}
	sa.sin_addr.s_addr=*((uint32_t *)host->h_addr_list[0]);
	
	//
	// Command-line
	//
	if([cmdline.text length]<1) {
		return NO;
	}
	if([cmdline.text characterAtIndex:[cmdline.text length]-1]!='!') {
		return NO;
	}

	//
	// UDP Port
	//
	if((udpport.text.intValue<1)||(udpport.text.intValue>0xFFFF)) {
		UIAlertView *alert=[[UIAlertView alloc] 
							initWithTitle: @"RMLSend"
							message: @"Invalid Port Number"
							delegate:nil
							cancelButtonTitle:nil
							otherButtonTitles:@"OK",nil];
		[alert show];
		[alert release];
		return NO;
	}
	sa.sin_port=htons(udpport.text.intValue);
	
	return YES;
}

-(BOOL) SaveFields
{	
	NSMutableString *str=[[NSMutableString alloc] init];
	[str appendString: ipaddr.text];
	[str appendString: @"\n"];
	[str appendString: udpport.text];
	[str appendString: @"\n"];
	[str appendString: cmdline.text];
	[str writeToFile: conf_filename atomically: NO encoding: NSASCIIStringEncoding error: nil];
 return YES;
 }

-(BOOL) RestoreFields
{
	NSString *str=[NSString stringWithContentsOfFile: conf_filename encoding: NSASCIIStringEncoding error: nil];
	NSArray *fields=[NSArray arrayWithArray: [str componentsSeparatedByString: @"\n"]];
    if(fields.count==3) {
		[ipaddr setText: [fields objectAtIndex: 0]];
		[udpport setText: [fields objectAtIndex: 1]];
		[cmdline setText: [fields objectAtIndex: 2]];
	}
 return YES;
}


// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
		NSLog(@"Started up!");
        // Custom initialization
    }
    return self;
}


/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	ipaddr.delegate=self;
	udpport.delegate=self;
	cmdline.delegate=self;

	//
	// Configure the UDP Socket
	//
	if((sock=socket(PF_INET,SOCK_DGRAM,0))<0) {
		NSLog(@"socket: %s",strerror(errno));
	}
	memset(&sa,0,sizeof(sa));
	sa.sin_family=AF_INET;

	//
	// Initialize the configuration file
	//
	NSString *dir=[NSHomeDirectory() stringByAppendingPathComponent: @"Documents/.rivendell"];
	NSFileManager *fm=[[NSFileManager alloc] init];
	if([fm contentsOfDirectoryAtPath: dir error: nil]==nil) {
		[fm createDirectoryAtPath: dir withIntermediateDirectories: YES attributes: nil error: nil];
	}
	conf_filename=[[NSString alloc] initWithString: [dir stringByAppendingPathComponent: @"rmlsend"]];
	if(![fm fileExistsAtPath: conf_filename]) {
		[fm createFileAtPath: conf_filename contents: [[NSData alloc] init] attributes: nil];
	}
/*
 UIAlertView *alert=[[UIAlertView alloc] 
 initWithTitle: @"RMLSend"
 message: conf_filename
 delegate:nil
 cancelButtonTitle:nil
 otherButtonTitles:@"OK",nil];
 [alert show];
 [alert release];
 */
 }



/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


//
// UITextFieldDelegate methods
//
-(BOOL) textFieldShouldBeginEditing:(UITextField *)textField
{
	return YES;
}


-(void) textFieldDidBeginEditing:(UITextField *)textField
{
}


-(BOOL) textFieldShouldEndEditing:(UITextField *)textField
{
	return YES;
}


-(void) textFieldDidEndEditing:(UITextField *)textField
{
}


-(BOOL) textField: (UITextField *) textField shouldChangeCharactersInRange: (NSRange) range 
replacementString: (NSString *) string
{
	return YES;
}


-(BOOL) textFieldShouldClear:(UITextField *)textField
{
	return YES;
}


-(BOOL) textFieldShouldReturn: (UITextField *) textField
{
	switch(textField.tag) {
		case RMLSEND_TAG_IPADDR:
			break;
			
		case RMLSEND_TAG_UDPPORT:
			break;
			
		case RMLSEND_TAG_CMDLINE:
			break;
	}
	button.enabled=[self ValidateFields];
	[textField resignFirstResponder];
	return YES;
}


- (void)dealloc {
	if(sock>=0) {
		close(sock);
	}
    [super dealloc];
}

@end
