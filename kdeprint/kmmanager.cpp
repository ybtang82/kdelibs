#include "kmmanager.h"
#include "kmprinter.h"
#include "kmdbentry.h"
#include "kmfactory.h"
#include "kmvirtualmanager.h"

#include <zlib.h>
#include <qfile.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kapp.h>

KMManager::KMManager(QObject *parent, const char *name)
: QObject(parent,name)
{
	m_printers.setAutoDelete(true);
}

KMManager::~KMManager()
{
}

bool KMManager::notImplemented()
{
	setErrorMsg("This operation is not implemented.");
	return false;
}

bool KMManager::modifyPrinter(KMPrinter *oldp, KMPrinter *newp)
{
	if (oldp->printerName() != newp->printerName())
	{
		// remove the old one, then create the new one. But before removing
		// the old one, retrieve its driver so that the new printer has the
		// same.
		DrMain	*driver = loadPrinterDriver(oldp, true);
		newp->setDriver(driver);
		if (!removePrinter(oldp))
			return false;
	}
	return createPrinter(newp);
}

bool KMManager::createPrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::removePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::removePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? removePrinter(p) : false);
}

bool KMManager::enablePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::enablePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? enablePrinter(p) : false);
}

bool KMManager::disablePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::disablePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? disablePrinter(p) : false);
}

bool KMManager::completePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::completePrinterShort(KMPrinter *p)
{
	return completePrinter(p);
}

bool KMManager::completePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? completePrinter(p) : false);
}

bool KMManager::setDefaultPrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::setDefaultPrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? setDefaultPrinter(p) : false);
}

bool KMManager::testPrinter(KMPrinter*)
{
	return notImplemented();
}

KMPrinter* KMManager::findPrinter(const QString& name)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		if (it.current()->name() == name) return it.current();
	setErrorMsg(QString::fromLatin1("%1: printer not found.").arg(name));
	return 0;
}

QList<KMPrinter>* KMManager::printerList(bool reload)
{
	if (reload || m_printers.count() == 0)
	{
		// first discard all printers
		QListIterator<KMPrinter>	it(m_printers);
		for (;it.current();++it)
			it.current()->setDiscarded(true);

		// make sure virtual printers will be reloaded if we don't have
		// any printer (for example is settings are wrong)
		if (m_printers.count() == 0)
			KMFactory::self()->virtualManager()->reset();

		// List real printers (in subclasses)
		listPrinters();
                // list virtual printers (and undiscard virtual printers if necessary)
                KMFactory::self()->virtualManager()->refresh();

		// remove discarded printers
		for (uint i=0; i<m_printers.count(); i++)
			if (m_printers.at(i)->isDiscarded())
			{
				m_printers.remove(i);
				i--;
			}
	}

	return &m_printers;
}

void KMManager::listPrinters()
{
	// do nothing
}

void KMManager::addPrinter(KMPrinter *p)
{
	if (p)
	{
		KMPrinter	*other = findPrinter(p->name());
		if (other)
		{
			other->copy(*p);
			delete p;
		}
		else
		{
			p->setDiscarded(false);
			m_printers.inSort(p);
		}
	}
}

QString KMManager::driverDbCreationProgram()
{
	return QString::null;
}

QString KMManager::driverDirectory()
{
	return QString::null;
}

DrMain* KMManager::loadPrinterDriver(KMPrinter*, bool)
{
	return NULL;
}

DrMain* KMManager::loadDbDriver(KMDBEntry *entry)
{
	return loadFileDriver(entry->file);
}

DrMain* KMManager::loadFileDriver(const QString&)
{
	return NULL;
}

bool KMManager::savePrinterDriver(KMPrinter*,DrMain*)
{
	return false;
}

bool KMManager::uncompressFile(const QString& filename, QString& destname)
{
	QFile	f(filename);
	bool	result(true);
	destname = QString::null;
	if (f.exists() && f.open(IO_ReadOnly))
	{
		char	buf[1024] = {0};
		f.readBlock(buf,2);
		if ((uchar)(buf[0]) == 037 && (uchar)(buf[1]) == 0213)
		{
			f.close();
			destname = locateLocal("tmp","kdeprint_") + KApplication::randomString(8);
			f.setName(destname);

			if (f.open(IO_WriteOnly))
			{
				gzFile	in = gzopen(filename.latin1(),"r");
				int	n(0);
				while ((n=gzread(in,buf,1024)) > 0)
					if (f.writeBlock(buf,n) != n)
						break;
				if (n != 0) result = false;
				gzclose(in);
				f.close();
			}
		}
	}
	return result;
}

void KMManager::setHardDefault(KMPrinter *p)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		it.current()->setHardDefault(false);
	if (p) p->setHardDefault(true);
}

void KMManager::setSoftDefault(KMPrinter *p)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
        {
		it.current()->setSoftDefault(false);
		it.current()->setOwnSoftDefault(false);
        }
	if (p)
        {
                p->setSoftDefault(true);
                KMPrinter       *pp = findPrinter(p->printerName());
                if (pp)
                        pp->setOwnSoftDefault(true);
        }
}

bool KMManager::configure(QWidget*)
{
	return notImplemented();
}