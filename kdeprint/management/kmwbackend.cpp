#include "kmwbackend.h"
#include "kmwizard.h"
#include "kmprinter.h"

#include <qlayout.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qradiobutton.h>

#include <kcursor.h>
#include <klocale.h>

class KRadioButton : public QRadioButton
{
public:
	KRadioButton(const QString& txt, QWidget *parent = 0, const char *name = 0);
};

KRadioButton::KRadioButton(const QString& txt, QWidget *parent, const char *name)
: QRadioButton(txt,parent,name)
{
	setCursor(KCursor::handCursor());
}

//********************************************************************************************************

KMWBackend::KMWBackend(QWidget *parent, const char *name)
: KMWizardPage(parent,name)
{
	m_ID = KMWizard::Backend;
	m_title = i18n("Backend selection");

	m_buttons = new QButtonGroup(this);
	m_buttons->hide();

	m_layout = new QVBoxLayout(this, 10, 10);
}

bool KMWBackend::isValid(QString& msg)
{
	if (!m_buttons->selected())
	{
		msg = i18n("You must select a backend !");
		return false;
	}
	return true;
}

void KMWBackend::initPrinter(KMPrinter *p)
{
	QString	s = p->option("kde-backend");
	int	ID(-1);

	if (!s.isEmpty())
		ID = s.toInt();
	else
	{
		s = p->device().protocol();
		// extract protocol
		if (s == "parallel" || s == "serial" || s == "usb") ID = KMWizard::Local;
		else if (s == "smb") ID = KMWizard::SMB;
		else if (s == "ipp" || s == "http") ID = KMWizard::IPP;
		else if (s == "lpd") ID = KMWizard::LPD;
		else if (s == "socket") ID = KMWizard::TCP;
		else if (s == "file") ID = KMWizard::File;
		else if (p->members().count() > 0) ID = KMWizard::Class;
	}

	if (m_buttons->find(ID))
		m_buttons->setButton(ID);
}

void KMWBackend::updatePrinter(KMPrinter *p)
{
	int	ID = m_buttons->id(m_buttons->selected());
	if (ID == KMWizard::Class) p->setType(KMPrinter::Class);
	else p->setType(KMPrinter::Printer);
	p->setOption("kde-backend",QString::number(ID));
	p->setOption("kde-backend-description",m_buttons->selected()->text());
	setNextPage((m_map.contains(ID) ? m_map[ID] : KMWizard::Error));
}

void KMWBackend::addBackend(int ID, const QString& txt, bool on, int nextpage)
{
	if (ID == -1)
	{
		QFrame	*fr = new QFrame(this);
		fr->setFrameStyle(QFrame::HLine|QFrame::Sunken);
		fr->setLineWidth(1);
		m_layout->addWidget(fr);
	}
	else
	{
		KRadioButton	*btn = new KRadioButton(txt, this);
		btn->setEnabled(on);
		m_buttons->insert(btn, ID);
		m_map[ID] = (nextpage == -1 ? ID : nextpage);	// use nextpage if specified, default to ID
		m_layout->addWidget(btn);
	}
}

void KMWBackend::enableBackend(int ID, bool on)
{
	QButton	*btn = m_buttons->find(ID);
	if (btn)
		btn->setEnabled(on);
}