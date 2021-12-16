#!/bin/sh

#
# (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
#
#   Build a PyPI package.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
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

PKG_NAME=$1

rm -rf pypi
mkdir -p pypi/src/$PKG_NAME
touch pypi/src/$PKG_NAME/__init__.py
cp api/$PKG_NAME.py pypi/src/$PKG_NAME/
cp ../../LICENSES/LGPLv2.txt pypi/LICENSE
cp pyproject.toml pypi/
cp setup.cfg pypi/

mkdir -p pypi/tests
cp tests/*.py pypi/tests/

python3 -m build pypi

