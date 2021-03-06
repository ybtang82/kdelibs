/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef KMPRINTERVIEW_H
#define KMPRINTERVIEW_H

#include <qwidgetstack.h>
#include <qlist.h>

class KMIconView;
class KMListView;
class KMPrinter;
class QIconViewItem;

class KMPrinterView : public QWidgetStack
{
	Q_OBJECT
public:
	enum ViewType { Icons = 0, List, Tree };

	KMPrinterView(QWidget *parent = 0, const char *name = 0);
	~KMPrinterView();

	void setPrinterList(QList<KMPrinter> *list);
	void setViewType(ViewType t);
	ViewType viewType() const 	{ return m_type; }

signals:
	void printerSelected(KMPrinter*);
	void rightButtonClicked(KMPrinter*, const QPoint&);

protected slots:
	void slotPrinterSelected(KMPrinter*);

private:
	KMIconView		*m_iconview;
	KMListView		*m_listview;
	ViewType		m_type;
	QList<KMPrinter>	*m_printers;
	KMPrinter		*m_current;
};

#endif
