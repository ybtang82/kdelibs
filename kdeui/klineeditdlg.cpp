#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfiledialog.h>

#include <kbuttonbox.h>
#include <klocale.h>
#include <kapp.h>

#include "klined.h"
#include "klineeditdlg.h"

KLineEditDlg::KLineEditDlg( const QString&_text, const QString& _value, 
			    QWidget *parent, bool _file_mode )
    : QDialog( parent, 0L, true )
{
  QGridLayout *layout = new QGridLayout(this, 4, 3, 10);

  QLabel *label = new QLabel(_text, this);
  layout->addWidget(label, 0, 0, AlignLeft);

  edit = new KLineEdit( this, 0L );
  edit->setMinimumWidth(edit->sizeHint().width() * 3);
  connect( edit, SIGNAL(returnPressed()), SLOT(accept()) );
    
  if ( _file_mode ) {
    completion = new KURLCompletion();
    connect(edit, SIGNAL (completion()),
	     completion, SLOT (make_completion()));
    connect(edit, SIGNAL (rotation()),
	     completion, SLOT (make_rotation()));
    connect(edit, SIGNAL (textChanged(const QString&)),
	     completion, SLOT (edited(const QString&)));
    connect(completion, SIGNAL (setText (const QString&)),
	     edit, SLOT (setText (const QString&)));
  } else
    completion = 0L;

  layout->addMultiCellWidget(edit, 1, 1, 0, _file_mode ? 1 : 2);
  layout->setColStretch(1, 1);

  if (_file_mode) {
    QPushButton *browse = new QPushButton(i18n("&Browse..."), this);
    layout->addWidget(browse, 1, 2, AlignCenter);
    connect(browse, SIGNAL(clicked()),
	    SLOT(slotBrowse()));
  }

  QFrame *hLine = new QFrame(this);
  hLine->setFrameStyle(QFrame::Sunken|QFrame::HLine);
  layout->addMultiCellWidget(hLine, 2, 2, 0, 2);

  KButtonBox *bBox = new KButtonBox(this);
  layout->addMultiCellWidget(bBox, 3, 3, 0, 2);

  QPushButton *ok = bBox->addButton(i18n("&OK"));
  ok->setDefault(true);
  connect( ok, SIGNAL(clicked()), SLOT(accept()));

  bBox->addStretch(1);

  QPushButton *clear = bBox->addButton(i18n("C&lear"));
  connect( clear, SIGNAL(clicked()), SLOT(slotClear()));

  bBox->addStretch(1);

  QPushButton *cancel = bBox->addButton(i18n("&Cancel"));
  connect( cancel, SIGNAL(clicked()), SLOT(reject()));

  bBox->layout();

  layout->activate();

  edit->setText( _value );
  edit->setFocus();
}

KLineEditDlg::~KLineEditDlg()
{
	delete completion;
}

void KLineEditDlg::slotClear()
{
    edit->setText("");
}

void KLineEditDlg::slotBrowse()
{
  QString fn = QFileDialog::getOpenFileName(QString::null, QString::null,
					    this);
  if (!fn.isNull())
    edit->setText("file://" + fn);
}

#include "klineeditdlg.moc"
