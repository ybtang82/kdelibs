# This Makefile.am was taken from the kdelibs distribution and modified for
#  kspell.  David Sweet (3/1/98)
#
#	This file is part of the KDE libraries
#    Copyright (C) 1997 Stephan Kulow (coolo@kde.org)

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this library; see the file COPYING.  If not, write to
#    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.

AM_CPPFLAGS = -DQT_NO_COMPAT

INCLUDES= -I$(top_srcdir)/dcop $(all_includes)

lib_LTLIBRARIES  = libkspell.la
libkspell_la_LDFLAGS = -version-info 3:0 -no-undefined
libkspell_la_LIBADD = ../kdeui/libkdeui.la

include_HEADERS = kspell.h ksconfig.h kspelldlg.h
noinst_HEADERS = version.h
libkspell_la_SOURCES = ksconfig.cpp kspelldlg.cpp kspell.cpp

libkspell_la_METASOURCES = AUTO

check_PROGRAMS = kspelltest
kspelltest_SOURCES = kspelltest.cpp
kspelltest_LDADD = libkspell.la
