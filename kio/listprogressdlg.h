// $Id$

#ifndef __kio_listprogress_dlg_h__
#define __kio_listprogress_dlg_h__

#include <qlistview.h>

#include <ktmainwindow.h>

#include "kio_job.h" 

class QTimer;
class KIOListView;

class KIOListViewItem : public QObject, public QListViewItem, KIO {

  Q_OBJECT

public:

  KIOListViewItem( KIOListView* view, QListViewItem *after, KIOJob *job );
  ~KIOListViewItem() {}

  void update();

  int id() { return m_pJob->id(); }

  void showSmallGUI();
  void remove() { m_pJob->kill(); }

public slots:

  void slotCanResume( int, bool );

  void slotTotalSize( int, unsigned long _bytes );
  void slotTotalFiles( int, unsigned long _files );
  void slotTotalDirs( int, unsigned long _dirs );
//   void slotProcessedSize( int, unsigned long _bytes );
  void slotPercent( int, unsigned long _bytes );
  void slotProcessedFiles( int, unsigned long _files );
//   void slotProcessedDirs( int, unsigned long _dirs );
  void slotScanningDir( int, const char *_dir );
  void slotSpeed( int, unsigned long _bytes_per_second );
  void slotCopyingFile( int, const char *_from, const char *_to );
  void slotMakingDir( int, const char *_dir );
  void slotGettingFile( int, const char *_url );
  void slotDeletingFile( int, const char *_url );
  void slotRenamed( int, const char* _new_url);

  void slotFinished( int );

protected:

  KIOJob* m_pJob;

  unsigned long m_iTotalSize;
  unsigned long m_iTotalFiles;
  unsigned long m_iTotalDirs;

  KIOListView *listView;

signals:
  void statusChanged( QListViewItem* );
};


class KIOListView : public QListView {

  Q_OBJECT

public:

  KIOListView (QWidget *parent = 0, const char *name = 0 );
  
  virtual ~KIOListView();

  /**
   * Field constants
   */
  enum KIOListViewFields {
    TB_OPERATION = 0,
    TB_LOCAL_FILENAME = 1,
    TB_RESUME = 2,
    TB_COUNT = 3,
    TB_PROGRESS = 4,
    TB_TOTAL = 5,
    TB_SPEED = 6,
    TB_REMAINING_TIME = 7,
    TB_ADDRESS = 8
  };

  friend KIOListViewItem;

protected:

  void readConfig();
  void writeConfig();

  // ListView IDs
  int lv_operation, lv_filename, lv_resume, lv_count, lv_progress;
  int lv_total, lv_speed, lv_remaining, lv_url;
};


/** 
* This class is not meant for a general usage.
*
* It's purpose is to show progress of IO operations.
* However, as opposed to @ref #KIOSimpleProgressDlg or @ref #KIOLittleProgressDlg,
* there is only one instance of this window for all KIOJobs.
* The main advantage is, that all downloads, deletions etc. are shown in one place,
* thus saving precious desktop space.
*
* Class @ref #KIOJob contains one static instance of this class.
* All IO operations ( KIOJobs ) are displayed in this window, one line per operation.
* User can cancel operations with Cancel button on statusbar.
*
* @short All-in-one IO progress window.
*/ 
class KIOListProgressDlg : public KTMainWindow {

  Q_OBJECT

public:

  KIOListProgressDlg();
  ~KIOListProgressDlg();

  // ToolBar field IDs
  enum { TOOL_CANCEL };

  // StatusBar field IDs
  enum { ID_TOTAL_FILES = 1, ID_TOTAL_SIZE, ID_TOTAL_TIME, ID_TOTAL_SPEED };

  void addJob( KIOJob *job );

protected:

  void closeEvent( QCloseEvent * );

  QTimer* updateTimer;
  KIOListView *myListView;

  KToolBar::BarPosition toolbarPos;
  QString properties;

  void readSettings();
  void writeSettings();

protected slots:

  void slotUpdate();

  void cancelCurrent();

  void slotOpenSimple( QListViewItem * );
  void slotSelection();

  void slotStatusChanged( QListViewItem * );
};

#endif
