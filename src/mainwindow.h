/***************************************************************************
 *   Copyright (C) 2004 by Eugene Bort                                     *
 *   esvn@umputun.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
  ***************************************************************************/
  
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define VERSION "0.6.1"

#define MAX_RECENT_FILES 7

#include <vector>
using namespace std;

#include <qvariant.h>

#include "frm_main.h"
#include "cmd_lineedit.h"

#include <stdio.h>

#ifndef _WINDOWS
#include <unistd.h>
#endif

#include <qtextcodec.h> 
#include <qsocket.h>
#include <qvariant.h>
#include <qstring.h>

#include <qlistview.h> 
#include <qsplashscreen.h> 
#include <qwidgetstack.h>
#include <qlabel.h> 
#include <qstatusbar.h> 
#include <qpopupmenu.h> 
#include <qdialog.h> 
#include <qvaluelist.h> 
#include <qsplitter.h> 
#include <qradiobutton.h>
#include <qlineedit.h> 
#include <qmessagebox.h> 
#include <qfiledialog.h> 
#include <qcheckbox.h>
#include <qsettings.h>
#include <qtextstream.h> 
#include <qheader.h> 
#include <qsize.h> 
#include <qrect.h>
#include <qaction.h> 
#include <qinputdialog.h>
#include <qdir.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qhttp.h>
#include <qspinbox.h>


#include "dirs_list.h"
#include "list_stat_parser.h"


#include "svn_commands.h"
#include "logwindow.h"
#include "propwindow.h"
#include "reposwindow.h"
#include "diffwindow.h"
#include "workspacewindow.h"
#include "mergewindow.h"
#include "changedwindow.h"

#include "entries_parser.h"
#include "svn_status.h"

#include "dlg_commit.h"
#include "dlg_update.h"
#include "dlg_info.h"
#include "dlg_copy.h"
#include "dlg_switch.h"
#include "dlg_diff_output.h"
#include "dlg_view.h"
#include "dlg_about.h"
#include "dlg_options.h"
#include "dlg_checkout.h"
#include "dlg_prop.h"
#include "dlg_busy.h"
#include "dlg_import.h"
#include "dlg_export.h"
#include "dlg_startup.h"
#include "dlg_help.h"
#include "dlg_verupdate.h"


#include "options.h"

class C_CommitWindow;

class C_MainWindow : public frm_main
{
	Q_OBJECT

public:
    C_MainWindow(QWidget* parent = 0, const char* name = 0) ;
    virtual ~C_MainWindow();
	
	void loadWorkspace(const C_DirPath& path, QString name="noname", bool append =false);
	bool loadWS(const QString& ws_name) ;

	const C_Path& workspace() {
		return workspace_;
	}

	
	//overided slots ----------------------------------------------------------------


	void list_dirs_doubleClicked(QListViewItem *item);
	void list_dirs_selectionChanged(QListViewItem *item);
	void list_files_selectionChanged(QListViewItem *item);

	void list_dirs_contextMenuRequested( QListViewItem *, const QPoint &xy, int );
	void list_files_contextMenuRequested( QListViewItem *, const QPoint &xy, int );
	void list_files_doubleClicked( QListViewItem *item);

	void modifyUpdateAction_activated();
	void modifyUpdate_revision_Action_activated();

	void modifyCommitAction_activated();
	void modifyEditAction_activated();
	void modifyCleanupAction_activated();
	void modifyRevertAction_activated();
	void modifyRevert_recursiveAction_activated();
	void modifyAddAction_activated();
	void modifyAdd_recursiveAction_activated();
	void modifyDeleteAction_activated();
	void modifyCopyAction_activated();
	void modifyCopy_current_Action_activated();
	void modifyCopy_remote_Action_activated();
	void modifyMoveAction_activated();
	void modifyMake_directory_Action_activated();

	void modifySwitchAction_activated();
	void modifyMegreAction_activated();
	void modifyResolveAction_activated();

	void queryLogAction_activated();
	void queryStatusAction_activated();
	void queryInfoAction_activated();
	void queryDiffAction_activated();
	void queryQuick_DiffAction_activated();
	void queryViewAction_activated();
	void query3_Way_DiffAction_activated();
	void queryProperties_activated();
	void queryBlameAction_activated();

    void viewRefresh_localAction_activated();
	void viewHide_unknownAction_toggled( bool hide);
	void viewRefresh_workspaceAction_activated();
	void viewRefresh_onlineAction_activated();

	void fileOpen_workspaceAction_activated();
	void fileOptions_Action_activated();
	void fileCheckoutAction_activated();
	void fileBrowse_Repository_Action_activated();
	void fileImport_Action_activated();
	void fileExport_Action_activated();
	void fileWorkspaceAction_activated();
	void fileShow_changed_filedirectoriesAction_activated();

	void helpAboutAction_activated();
	void helpIndexAction_activated();
	void helpWebsiteAction_activated();
	void helpCheck_for_updatesAction_activated();

	void le_cmd_returnPressed();

	void saveSettings();
	void loadSettings();

	void setSvnDir(const QString& dir);
public slots:

	void onTimer();
	void httpDone(bool error);
	void openRecentFile(int param);

private:
	QStringList split(QString &str, char sep, bool is_trim=false);
	void ltrim(QString& str);	
	void rtrim(QString& str);

	void internalDiff(const QString& currwd,
                      const QString& file_dir_name_1, const QString& file_dir_name_2, 
					  const QString& rev, bool non_recursive, bool notice_ancestry, 
					  const QString& extensions);

    // Display svn log window
    void show_Log(const C_Path& pathname, bool show_changed_paths);
    // Query show_changed_paths from options dialog and display log window 
    void show_Log(const C_Path& pathname);

    // Do svn cat on args and display output in view window
    void show_View(const C_Path& pathname);

    void setPopupMenu();
	void updateRecentFileItems();
	void setFonts();
	QString getUrlOnline(const C_Path& localpath);

	bool fillSvnStatus(C_SvnStatus* svn_status);
	void refreshList();

	int getSelectedFiles(vector<QString> &selected_files);

    // selection for either dirs or file view
	int getSelectedPaths(PathSelection& selection);

    // selection for file view
    int getSelectedFiles(PathSelection& selection);

	void revert(bool is_recursive);

	void getFilesAndSize(const C_Path& path, vector<QString>& files_and_size);
	void showHideToolBars();
	void colorizeDirsList(QListViewItem *dir_item);

	void getOpenMap();

	void sendToOutput(const QString& str);

private:

	QPopupMenu	*mnu_popup_;
	C_CommitWindow	*dlg_commit_;
	dlg_update	*dlg_update_;
	dlg_info	*dlg_info_;
	C_LogWindow	*dlg_log_;
	C_MergeWindow *dlg_merge_;

	dlg_copy	*dlg_copy_;
	dlg_switch	*dlg_switch_;

	C_DiffWindow	*dlg_diff_setup_;
	dlg_diff		*dlg_diff_;

	dlg_view		*dlg_view_;
	dlg_about 		*dlg_about_;
	dlg_options 	*dlg_options_;
	dlg_checkout	*dlg_checkout_; 
	dlg_import		*dlg_import_;
	dlg_export		*dlg_export_;
	dlg_startup		*dlg_startup_;
	dlg_help		*dlg_help_;
	dlg_verupdate	*dlg_verupdate_;

	C_PropWindow	*dlg_prop_; 
	C_ReposWindow	*dlg_repos_;
	C_WsWindow		*dlg_workspace_;
	C_ChangedWindow *dlg_changed_;

	QRect geometry_;
	C_DirsList *dirs_list_;
	C_DirPath   workspace_;
	QString diff_tmp_file_;

	bool is_selected_dir_;
	//int scroll_pos_;
	C_FilesList current_file_list_;

	C_SvnCommands *svn_commands_;
	QTimer *refresh_timer_;

	C_SvnStatus *svn_status_;

	//QSettings	settings_;


	bool hide_unknown_;
	bool is_ext_diff_;

	QString subversion_set_;
	QString editor_set_;
	QString diff_set_;
	QString user_, passwd_;

	QHttp http_;

	QStringList recent_files_;
	int recent_file_ids_[MAX_RECENT_FILES];

	QListViewItem *last_item_;

	C_LSParser *list_stat_;
	bool dir_refresh_disable_;

	map<QString, QStringList> ws_;

	bool in_timer_now_;

	vector<QString> last_files_and_size_;
	//QString last_selected_path_;
	time_t last_refresh_;

	QStringList open_map_;
	QString svn_dir_;
	bool auto_refresh_disable_;
    bool show_flat_view_;
    bool show_changed_items_only_;

    QString baseCaption_;

public:
    virtual void viewSwitchFlatViewChanged_toggled(bool);
    virtual void viewSwitchFlatView_toggled(bool);
};


#endif

