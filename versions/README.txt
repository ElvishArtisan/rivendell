This directory contains the canonical version strings for Rivendell
and its various components. Each version definition consists of a
file containing the version string on a single line. THERE MUST BE
NO WHITESPACE,NEWLINE OR CARRIAGE RETURN CHARACTERS IN THESE STRINGS!

PACKAGE_VERSION - Primary version string for Rivendell itself.
                  See https://semver.org/.

RIVWEBCAPI_CURRENT -  ABI versioning for rivwebcapi. Each file should contain
RIVWEBCAPI_REVISION - a single integer, updated as follows:
RIVWEBCAPI_AGE

 From http://www.gnu.org/software/libtool/manual.html#Updating-version-info

   1. Start with version information of 0:0:0 for each libtool library.
   2. Update the version information only immediately before a public
      release of your software. More frequent updates are unnecessary,
      and only guarantee that the current interface number gets larger
      faster.
   3. If the library source code has changed at all since the last update,
      then increment 'RIVWEBCAPI_REVISION' (c:r:a becomes c:r+1:a).
   4. If any interfaces have been added, removed, or changed since the last
      update, increment 'RIVWEBCAPI_CURRENT', and set 'RIVWEBCAPI_REVISION'
      to 0.
   5. If any interfaces have been added since the last public release, then
      increment 'RIVWEBCAPI_AGE'.
   6. If any interfaces have been removed since the last public release,
      then set 'RIVWEBCAPI_AGE' to 0.

PYTHONAPI_VERSION - Version string for the Python components beneath '/apis/'.
		    See the Python Packaging User Guide - Version specifiers
		    for formatting rules, at:
		    https://packaging.python.org/en/latest/specifications/version-specifiers/
