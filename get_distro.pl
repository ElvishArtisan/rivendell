#!/usr/bin/perl -W

# get_distro.pl
# 
#   Read various fields from 'os-release'.
#   Used as part of the AR_GET_DISTRO() macro.
#
#   See https://www.freedesktop.org/software/systemd/man/os-release.html
#   for a description of the various fields.
#
#   (C) Copyright 2012-2021 Fred Gleason <fredg@salemradiolabs.com>
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

my $usage="USAGE: get_distro.pl NAME|PRETTY_NAME|ID|ID_LIKE|VERSION|MAJOR|MINOR|POINT";

if($ARGV[0] eq "NAME") {
    print &Extract("NAME");
    exit 0;
}

if($ARGV[0] eq "PRETTY_NAME") {
    print &Extract("PRETTY_NAME");
    exit 0;
}

if($ARGV[0] eq "ID") {
    print &Extract("ID");
    exit 0;
}

if($ARGV[0] eq "ID_LIKE") {
    print &Extract("ID_LIKE");
    exit 0;
}

if($ARGV[0] eq "VERSION") {
    print &Extract("VERSION_ID");
    exit 0;
}

if($ARGV[0] eq "MAJOR") {
    my $ver=&Extract("VERSION_ID");
    my @f0=split '\.',$ver;
    print $f0[0];
    exit 0;
}

if($ARGV[0] eq "MINOR") {
    my $ver=&Extract("VERSION_ID");
    my @f0=split '\.',$ver;
    if(scalar(@f0)>=2) {
	print $f0[1];
	exit 0;
    }
    print "0";
    exit 0;
}

if($ARGV[0] eq "POINT") {
    my $ver=&Extract("VERSION_ID");
    my @f0=split '\.',$ver;
    if(scalar(@f0)>=3) {
	print $f0[2];
	exit 0;
    }
    print "0";
    exit 0;
}

print $usage;
exit 256;


sub Extract
{
    if((open RELEASE,"<","/etc/os-release") ||
       (open RELEASE,"<","/usr/lib/os-release")) {
	while(<RELEASE>) {
	    my @f0=split "\n",$_;
	    for(my $i=0;$i<@f0;$i++) {
		my @f1=split "=",$f0[$i];
		if($f1[0] eq $_[0]) {
		    $f1[1]=~s/^"(.*)"$/$1/;
		    return $f1[1];
#		    return substr($f1[1],1,length($f1[1])-2);
		}
	    }
	}
    }
    return "";
}


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
