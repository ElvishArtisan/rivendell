#!/usr/bin/perl -W

# get_distro.pl
# 
# Try to determine the distribution name and version of the host machine.
# Used as part of the AR_GET_DISTRO() macro.
#
#   (C) Copyright 2012,2016 Fred Gleason <fredg@salemradiolabs.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of
#   the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

#USAGE: get_distro.pl NAME|VERSION|MAJOR|MINOR|POINT

if($ARGV[0] eq "NAME") {
    if(!system("test","-f","/etc/SuSE-release")) {
	print "SuSE";
	exit 0;
    }
    if(!system("test","-f","/etc/debian_version")) {
	print "Debian";
	exit 0;
    }
    if(!system("test","-f","/etc/redhat-release")) {
	print "RedHat";
	exit 0;
    }
}

if($ARGV[0] eq "VERSION") {
    if(!system("test","-f","/etc/SuSE-release")) {
	print &GetVersion("/etc/SuSE-release");
	exit 0;
    }
    if(!system("test","-f","/etc/debian_version")) {
	print &GetVersion("/etc/debian_version");
	exit 0;
    }
    if(!system("test","-f","/etc/redhat-release")) {
	print &GetVersion("/etc/redhat-release");
	exit 0;
    }
}

if($ARGV[0] eq "MAJOR") {
    if(!system("test","-f","/etc/SuSE-release")) {
	print &GetMajor("/etc/SuSE-release");
	exit 0;
    }
    if(!system("test","-f","/etc/debian_version")) {
	print &GetMajor("/etc/debian_version");
	exit 0;
    }
    if(!system("test","-f","/etc/redhat-release")) {
	print &GetMajor("/etc/redhat-release");
	exit 0;
    }
}

if($ARGV[0] eq "MINOR") {
    if(!system("test","-f","/etc/SuSE-release")) {
	print &GetMinor("/etc/SuSE-release");
	exit 0;
    }
    if(!system("test","-f","/etc/debian_version")) {
	print &GetMinor("/etc/debian_version");
	exit 0;
    }
    if(!system("test","-f","/etc/redhat-release")) {
	print &GetMinor("/etc/redhat-release");
	exit 0;
    }
}

if($ARGV[0] eq "POINT") {
    if(!system("test","-f","/etc/SuSE-release")) {
	print &GetPoint("/etc/SuSE-release");
	exit 0;
    }
    if(!system("test","-f","/etc/debian_version")) {
	print &GetPoint("/etc/debian_version");
	exit 0;
    }
    if(!system("test","-f","/etc/redhat-release")) {
	print &GetPoint("/etc/redhat-release");
	exit 0;
    }
}

exit 256;


sub GetVersion
{
    if(open VERSION,"<",$_[0]) {
	my $version=<VERSION>;
	my @f0=split " ",$version;
	for(my $i=0;$i<@f0;$i++) {
	    my @f1=split "[.]",$f0[$i];
	    if(@f1>1) {
		return $f0[$i];
	    }
	}
    }
    return "";
}


sub GetMajor
{
    my @f0=split "[.]",&GetVersion($_[0]);

    return $f0[0];
}


sub GetMinor
{
    my @f0=split "[.]",&GetVersion($_[0]);

    if(@f0 ge 2) {
	return $f0[1];
    }
    return "";
}


sub GetPoint
{
    my @f0=split "[.]",&GetVersion($_[0]);

    if(@f0 ge 3) {
	return $f0[2];
    }
    return "";
}
