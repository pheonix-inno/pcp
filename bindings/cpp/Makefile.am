#
#    This file is part of Pretty Curved Privacy (pcp1).
#
#    Copyright (C) 2013 T.Linden.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    You can contact me by mail: <tlinden AT cpan DOT org>.
#
AM_CXXFLAGS = -I../../include

EXTRA_DIST = *.h

lib_LTLIBRARIES      = libpcp1++.la

libpcp1___la_SOURCES = pcp++.h ptx.cpp key.cpp vault.cpp crypto.cpp sign.cpp buffer.cpp
include_HEADERS      = pcp++.h

libpcp1___la_CXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
                       --mode=link $(CXXLD) $(AM_LDFLAGS) \
                       $(LDFLAGS) -o $@
