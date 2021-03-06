
#	This file is part of the KDE libraries
#    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
#			  (C) 1997 Stephan Kulow (coolo@kde.org)

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

SUBDIRS = . tests

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdeui.la
libkdeui_la_LDFLAGS = -no-undefined -version-info 3:0
# we should get this to work when libtool 1.3 is released
libkdeui_la_LIBADD = ../kdecore/libkdecore.la

picsdir = $(kde_datadir)/kdeui/pics
pics_DATA = aboutkde.png keys.png ktip-bulb.png ktip-background.png

include_HEADERS = kprogress.h kcolordlg.h kcolordialog.h kselect.h \
		kdatepik.h kdatetbl.h kfontdialog.h kpopupmenu.h \
		ktabctl.h kstatusbar.h kimageeffect.h \
		kmainwindow.h ktmainwindow.h ktoolbar.h kmenubar.h \
		knuminput.h kseparator.h klineedit.h \
		krestrictedline.h kcolorbutton.h kcolorbtn.h \
		ksystemtray.h kdockwindow.h kbuttonbox.h \
		kcontainer.h keditcl.h kled.h keditlistbox.h \
		kdbtn.h kwizard.h kkeydialog.h \
		kurllabel.h kruler.h kcursor.h kaccelmenu.h \
		klineeditdlg.h kwidgetid.h kthemebase.h \
		kthemestyle.h kcharselect.h kcolordrag.h qxembed.h \
		knumvalidator.h kdialog.h kdialogbase.h \
		kjanuswidget.h kaboutdialog.h kpixmapeffect.h \
		kauthicon.h kmessagebox.h ksharedpixmap.h \
		kdualcolorbtn.h ktoolbarbutton.h \
		ktoolbarradiogroup.h ktextbrowser.h kaction.h khelpmenu.h \
		kcmodule.h kcmenumngr.h kpanelmenu.h \
		kstdaction.h kdrawutil.h kcombobox.h \
		kiconview.h klistview.h klistbox.h kbugreport.h kpassdlg.h \
		kxmlguiclient.h kxmlgui.h kxmlguibuilder.h kpixmapio.h \
		kedittoolbar.h kdocktabctl.h kdockwidget.h kanimwidget.h \
		krootpixmap.h kaboutkde.h kaboutapplication.h \
		kpanelapplet.h kpushbutton.h kdcopactionproxy.h kcolorcombo.h \
		kpanelextension.h kcompletionbox.h kalphapainter.h ksqueezedtextlabel.h \
		kcommand.h kwindowlistmenu.h kfontcombo.h kaccelgen.h ktip.h \
		kdatewidget.h karrowbutton.h

# the order of the sources isn't randomly (at least not completly).
# the order is predictated by the use of X11 header files
# the files on the first line need to occur first when compiling
# with --enable-final since they use X defines that get undefined
# in ksharedpixmap.cpp
libkdeui_la_SOURCES = \
		qxembed.cpp ksharedpixmap.cpp \
		kthemestyle.cpp kurllabel.cpp \
		knuminput.cpp ktoolbar.cpp klineedit.cpp klistview.cpp kprogress.cpp \
		kimageeffect.cpp kcolordlg.cpp kselect.cpp kdatepik.cpp \
		kdatetbl.cpp kfontdialog.cpp kpopupmenu.cpp ktabctl.cpp \
		kstatusbar.cpp kmainwindow.cpp ktmainwindow.cpp \
		kmenubar.cpp kpixmapio.cpp keditlistbox.cpp \
		kseparator.cpp krestrictedline.cpp  \
		kcolorbtn.cpp ksystemtray.cpp \
		kbuttonbox.cpp keditcl1.cpp keditcl2.cpp \
		kled.cpp kdbtn.cpp kwizard.cpp kcontainer.cpp \
		kkeydialog.cpp kruler.cpp kcursor.cpp kaccelmenu.cpp \
		klineeditdlg.cpp kthemebase.cpp  \
		kcharselect.cpp kcolordrag.cpp  \
		knumvalidator.cpp kdialog.cpp kdialogbase.cpp \
		kjanuswidget.cpp kaboutdialog.cpp kpixmapeffect.cpp \
		kauthicon.cpp kmessagebox.cpp kdualcolorbtn.cpp \
		ktoolbarradiogroup.cpp ktoolbarbutton.cpp \
		ktextbrowser.cpp kaction.cpp khelpmenu.cpp \
		kcmenumngr.cpp kpanelmenu.cpp kstdaction.cpp kdrawutil.cpp \
		kiconview.cpp klistbox.cpp kbugreport.cpp \
		kcombobox.cpp kpassdlg.cpp kxmlguiclient.cpp kxmlgui.cpp \
		kxmlguibuilder.cpp kedittoolbar.cpp \
		kdocktabctl.cpp kdockwidget.cpp kdockwidget_private.cpp \
		kanimwidget.cpp krootpixmap.cpp kaboutkde.cpp \
		kaboutapplication.cpp kpanelapplet.cpp kdcopactionproxy.cpp \
		kcolorcombo.cpp kpushbutton.cpp kpanelextension.cpp \
		kcompletionbox.cpp kalphapainter.cpp ksqueezedtextlabel.cpp \
		kcommand.cpp kwindowlistmenu.cpp kfontcombo.cpp ktip.cpp \
		kdatewidget.cpp karrowbutton.cpp

noinst_HEADERS = kdockwidget_private.h klistviewlineedit.h kkeybutton.h \
		kdialogbase_priv.h kaboutdialog_private.h kcursor_private.h \
		kdepackages.h kdeui-doc.h 

scripts_SCRIPTS = preparetips
scriptsdir = $(kde_bindir)

libkdeui_la_METASOURCES = AUTO

rcdir = $(kde_confdir)/ui
rc_DATA = ui_standards.rc

EXTRA_DIST = CHANGES.kdatepicker USERS.kdatepicker kpixmapio-perf.png dcolorreset.xpm kpartgui.dtd dcolorarrow.xbm

kdepackages:
	-rm $(srcdir)/kdepackages.h
	if test -f $(top_srcdir)/../bugs/Maintainers; then \
		echo "// DO NOT EDIT - EDIT bugs/Maintainers instead" >  $(srcdir)/kdepackages.h ;\
		echo "const char *packages[] = {" >> $(srcdir)/kdepackages.h ;\
		grep -v "^todo-" $(top_srcdir)/../bugs/Maintainers | sed -e "s#^\([^ ]*\).*#\"\1\",#" | sort >> $(srcdir)/kdepackages.h ; \
		echo "0 };" >>  $(srcdir)/kdepackages.h ; \
	fi

