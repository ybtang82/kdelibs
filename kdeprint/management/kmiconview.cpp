#include "kmiconview.h"
#include "kmprinter.h"

#include <kiconloader.h>

KMIconViewItem::KMIconViewItem(QIconView *parent, KMPrinter *p)
: QIconViewItem(parent)
{
	m_state = 0;
	updatePrinter(p);
	m_size = parent->itemTextPos();
	m_pixmap = QString::null;
}

void KMIconViewItem::updatePixmap(int pos)
{
	if (m_printer)
	{
		int	s((pos == -1 ? iconView()->itemTextPos() : pos));
		QString	str(m_printer->pixmap());
		if (s != m_size || str != m_pixmap)
		{
			m_size = s;
			m_pixmap = str;
			if (m_size == QIconView::Right)
				setPixmap(SmallIcon(m_pixmap));
			else
				setPixmap(DesktopIcon(m_pixmap));
		}
	}
}

void KMIconViewItem::paintItem(QPainter *p, const QColorGroup& cg)
{
	if (m_state != 0)
	{
		QFont	f(p->font());
		if (m_state & 0x1) f.setBold(true);
		if (m_state & 0x2) f.setItalic(true);
		p->setFont(f);
	}
	QIconViewItem::paintItem(p,cg);
}

void KMIconViewItem::updatePrinter(KMPrinter *p)
{
	m_printer = p;
	bool	update(false);
	if (p)
	{
		int	oldstate = m_state;
		m_state = ((p->isHardDefault() ? 0x1 : 0x0) | (p->ownSoftDefault() ? 0x2 : 0x0));
		update = (oldstate != m_state);
		if (p->name() != text() || oldstate != m_state)
			setText(p->name());
		setKey(QString::fromLatin1("%1_%2").arg((p->isClass(false) ? "class" : "printer")).arg(p->name()));
	}
	else
		setKey(QString::fromLatin1("aaa"));
	updatePixmap();
	if (update) repaint();
	setDiscarded(false);
}

KMIconView::KMIconView(QWidget *parent, const char *name)
: KIconView(parent,name)
{
	setMode(KIconView::Select);
	setSelectionMode(QIconView::Single);
	setItemsMovable(false);
	setResizeMode(QIconView::Adjust);

	m_items.setAutoDelete(false);
	setViewMode(KMIconView::Big);

	connect(this,SIGNAL(rightButtonClicked(QIconViewItem*,const QPoint&)),SLOT(slotRightButtonClicked(QIconViewItem*,const QPoint&)));
	connect(this,SIGNAL(selectionChanged()),SLOT(slotSelectionChanged()));
}

KMIconView::~KMIconView()
{
}

KMIconViewItem* KMIconView::findItem(KMPrinter *p)
{
	if (p)
	{
		QListIterator<KMIconViewItem>	it(m_items);
		for (;it.current();++it)
			if (it.current()->printer() && it.current()->printer()->name() == p->name()
			    && it.current()->printer()->isClass() == p->isClass())
				return it.current();
	}
	return 0;
}

void KMIconView::setPrinterList(QList<KMPrinter> *list)
{
	bool	changed(false);

	QListIterator<KMIconViewItem>	it(m_items);
	for (;it.current();++it)
		it.current()->setDiscarded(true);

	if (list)
	{
		QListIterator<KMPrinter>	it(*list);
		KMIconViewItem			*item(0);
		for (;it.current();++it)
		{
                        // only keep real printers (no instances)
                        if (!it.current()->instanceName().isEmpty())
                                continue;
			item = findItem(it.current());
			if (!item)
			{
				item = new KMIconViewItem(this,it.current());
				m_items.append(item);
				changed = true;
			}
			else
				item->updatePrinter(it.current());
		}
	}

	for (uint i=0; i<m_items.count(); i++)
		if (m_items.at(i)->isDiscarded())
		{
			delete m_items.take(i);
			i--;
			changed = true;
		}

	if (changed) sort();
	emit selectionChanged();
}

void KMIconView::setViewMode(ViewMode m)
{
	m_mode = m;
	bool	big = (m == KMIconView::Big);
	QIconView::Arrangement	arr = (!big ? QIconView::TopToBottom : QIconView::LeftToRight);

	QListIterator<KMIconViewItem>	it(m_items);
	for (;it.current();++it)
		it.current()->updatePixmap(arr);

	setArrangement(arr);
	setItemTextPos((big ? QIconView::Bottom : QIconView::Right));
	setGridX((big ? 60 : -1));
	setWordWrapIconText(big);
}

void KMIconView::slotRightButtonClicked(QIconViewItem *item, const QPoint& p)
{
	KMIconViewItem	*citem = dynamic_cast<KMIconViewItem*>(item);
	emit rightButtonClicked((citem ? citem->printer() : NULL),p);
}

void KMIconView::slotSelectionChanged()
{
	KMIconViewItem	*item = (KMIconViewItem*)currentItem();
	emit printerSelected((item ? item->printer() : NULL));
}

void KMIconView::setPrinter(KMPrinter *p)
{
	QListIterator<KMIconViewItem>	it(m_items);
	for (;it.current();++it)
		if (it.current()->printer() == p)
		{
			setSelected(it.current(),true);
			break;
		}
}
#include "kmiconview.moc"