/**********************************************************************

	--- Qt Architect generated file ---

	File: AssertDialogData.h
	Last generated: Mon Sep 28 17:05:43 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef AssertDialogData_included
#define AssertDialogData_included

#include <qdialog.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qframe.h>

class AssertDialogData : public QDialog
{
    Q_OBJECT

public:

    AssertDialogData
    (
        QWidget* parent = 0L,
        const char* name = 0L
    );

    virtual ~AssertDialogData();

public slots:


protected slots:

    virtual void setAction(int);

protected:
    QFrame* frameBase;
    QButtonGroup* bgActions;
    QLabel* labelImage;
    QLabel* labelHeadline;
    QLabel* labelError;
    QPushButton* buttonOK;

};

#endif // AssertDialogData_included
