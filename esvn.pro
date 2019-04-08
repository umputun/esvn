TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

HEADERS	+= src/mainwindow.h \
	src/dirs_list.h \
	src/entries_parser.h \
	src/svn_commands.h \
	src/logwindow.h \
	src/reposwindow.h \
	src/list_stat_parser.h \
	src/cmd_lineedit.h \
	src/svn_status.h \
	src/options.h \
	src/check_lbi.h \
	src/commitwindow.h

SOURCES	+= src/esvn.cpp \
	src/mainwindow.cpp \
	src/entries_parser.cpp \
	src/dirs_list.cpp \
	src/reposwindow.cpp \
	src/svn_commands.cpp \
	src/list_stat_parser.cpp \
	src/diffwindow.cpp \
	src/cmd_lineedit.cpp \
	src/svn_status.cpp \
	src/options.cpp \
	src/check_lbi.cpp \
	src/commitwindow.cpp

FORMS	= .ui/frm_main.ui \
	.ui/dlg_commit.ui \
	.ui/dlg_update.ui \
	.ui/dlg_checkout.ui \
	.ui/dlg_copy.ui \
	.ui/dlg_merge.ui \
	.ui/dlg_info.ui \
	.ui/dlg_log.ui \
	.ui/dlg_busy.ui \
	.ui/dlg_switch.ui \
	.ui/dlg_diff_setup.ui \
	.ui/dlg_diff_output.ui \
	.ui/dlg_view.ui \
	.ui/dlg_about.ui \
	.ui/dlg_options.ui \
	.ui/dlg_prop.ui \
	.ui/dlg_propedit.ui \
	.ui/dlg_repository.ui \
	.ui/dlg_auth.ui \
	.ui/dlg_import.ui \
	.ui/dlg_export.ui \
	.ui/dlg_url.ui \
	.ui/dlg_workspace.ui \
	.ui/dlg_ws_edit.ui \
	.ui/dlg_startup.ui \
	.ui/dlg_help.ui \
	.ui/dlg_verupdate.ui \
	.ui/dlg_mapping.ui \
	.ui/dlg_map_edit.ui \
	.ui/dlg_changed.ui

IMAGES	= images/fileopen.png \
	images/filesave.png \
	images/undo.png \
	images/redo.png \
	images/editcopy.png \
	images/searchfind.png \
	images/BranchTag30x16.xpm \
	images/CmdWorkbench32x32.xpm \
	images/Connect32x16-00.xpm \
	images/Connect32x16-01.xpm \
	images/Connect32x16-02.xpm \
	images/Connect32x16-03.xpm \
	images/Delete16x16.xpm \
	images/Disable16x16.xpm \
	images/Enable16x16.xpm \
	images/FileASCII16x16.xpm \
	images/FileAdded16x16.xpm \
	images/FileBinary16x16.xpm \
	images/FileConflict16x16.xpm \
	images/FileLocked16x16.xpm \
	images/FileModified16x16.xpm \
	images/FileNeedsCheckout16x16.xpm \
	images/FileNeedsMerge16x16.xpm \
	images/FileNeedsPatch16x16.xpm \
	images/FileRemoved16x16.xpm \
	images/FileStatus16x16.xpm \
	images/FileUnchanged16x16.xpm \
	images/FileUnknown16x16.xpm \
	images/FileWriteable16x16.xpm \
	images/FolderClosed16x16.xpm \
	images/FolderClosedAuto26x16.xpm \
	images/FolderClosedLocked16x16.xpm \
	images/FolderClosedLockedModified16x16.xpm \
	images/FolderClosedLockedNonCVS16x16.xpm \
	images/FolderClosedLockedProblem16x16.xpm \
	images/FolderClosedLockedUncommitted16x16.xpm \
	images/FolderClosedLockedWarn16x16.xpm \
	images/FolderClosedModified16x16.xpm \
	images/FolderClosedModifiedAuto26x16.xpm \
	images/FolderClosedNonCVS16x16.xpm \
	images/FolderClosedNonCVSAuto26x16.xpm \
	images/FolderClosedProblem16x16.xpm \
	images/FolderClosedProblemAuto26x16.xpm \
	images/FolderClosedUncommitted16x16.xpm \
	images/FolderClosedUncommittedAuto26x16.xpm \
	images/FolderClosedWarn16x16.xpm \
	images/FolderClosedWarnAuto26x16.xpm \
	images/FolderDisabled16x16.xpm \
	images/FolderMissing16x16.xpm \
	images/FolderOpened16x16.xpm \
	images/FolderOpenedProblem16x16.xpm \
	images/FolderStatus16x16.xpm \
	images/FolderStatusAuto26x16.xpm \
	images/LogTree16x16.xpm \
	images/LogTree32x32.xpm \
	images/QueryUpdate16x16.xpm \
	images/Tag16x16.xpm \
	images/Tag30x16.xpm \
	images/Tag32x32.xpm \
	images/TagTag30x16.xpm \
	images/arrow_down_16x16.xpm \
	images/arrow_left_16x16.xpm \
	images/arrow_right_16x16.xpm \
	images/arrow_up_16x16.xpm \
	images/checkout_16x16.xpm \
	images/commit_16x16.xpm \
	images/cvs_annotate_16x16.xpm \
	images/cvs_watch_add_16x16.xpm \
	images/cvs_watch_off_16x16.xpm \
	images/cvs_watch_on_16x16.xpm \
	images/cvs_watch_remove_16x16.xpm \
	images/merge_16x16.xpm \
	images/module_add_16x16.xpm \
	images/module_autoupdate_16x16.xpm \
	images/module_status_16x16.xpm \
	images/quit_16x16.xpm \
	images/refresh_16x16.xpm \
	images/stop_16x16.xpm \
	images/update_16x16.xpm \
	images/who_changed_line_16x16.xpm \
	images/who_changed_lines_16x16.xpm \
	images/apply.png \
	images/backimage.png \
	images/blank.png \
	images/bookmark.png \
	images/bookmark_add.png \
	images/cancel.png \
	images/clear_left.png \
	images/edit.png \
	images/exec.png \
	images/exit.png \
	images/folder_blue.png \
	images/folder_blue_open.png \
	images/folder_exec_3.png \
	images/folder_green.png \
	images/folder_green_open.png \
	images/folder_grey_3.png \
	images/folder_grey_open_3.png \
	images/folder_home.png \
	images/folder_new.png \
	images/folder_red.png \
	images/folder_red_3.png \
	images/hotlistadd.png \
	images/kfm_home.png \
	images/knotify.png \
	images/looknfeel.png \
	images/misc.png \
	images/ok.png \
	images/reload.png \
	images/revert.png \
	images/run.png \
	images/services.png \
	images/tux.png \
	images/window_list.png \
	images/esvn-48.png \
	images/b-16x16.xpm \
	images/t-16x16.xpm \
	images/CmdExport32x32.xpm \
	images/CmdImport32x32.xpm \
	images/CmdTimer32x32.xpm \
	images/CmdTimerDisabled32x32.xpm \
	images/Compress32x32.xpm \
	images/add.png \
	images/configure.png \
	images/dirsynch.png \
	images/editcut.png \
	images/editpaste.png \
	images/edittrash.png \
	images/headline.png \
	images/hi16-action-make.png \
	images/kdar.png \
	images/key2.png \
	images/kpowersave.png \
	images/lockoverlay.png \
	images/news_subscribe.png \
	images/openterm.png \
	images/package_utilities.png \
	images/print.png \
	images/query-view-table_16x16.png \
	images/rebuild.png \
	images/view_text.png \
	images/visoel.xpm \
	images/wizard.png \
	images/xsldbg_enable.png \
	images/folder_grey.png \
	images/folder_grey_open.png \
	images/folder_home_2.png \
	images/xsldbg_delete.png \
	images/folder_dark.png \
	images/folder_orange.png \
	images/folder_violet.png \
	images/trash.png \
	images/blockdevice.png \
	images/colorscm.png \
	images/esvn.png \
	images/ktip.png \
	images/lock.png \
	images/u.xpm \
	images/module_add_recursive_16x16.xpm \
	images/update_recursive_16x16.xpm \
	images/editcopy-recursive.png \
	images/revert-recursive.png \
	images/away.png \
	images/enhanced_browsing.png \
	images/entry-bool.png \
	images/entry-list.png \
	images/entry-number.png \
	images/entry-schema.png \
	images/entry-string.png \
	images/folder-closed.png \
	images/folder-open.png \
	images/folder_green_3.png \
	images/folder_grey1.png \
	images/folder_yellow.png \
	images/folder_yellow_open.png \
	images/kompare.png \
	images/note-indicator.png \
	images/project.xpm \
	images/stock-rotate-180-16.png \
	images/syntax.xpm \
	images/flat_view.png

	
	
RC_FILE = src/esvn.rc	
unix {


  MAKEFILE = esvn.mak
  VER=$$system(cat VERSION)

  target.path  = /usr/bin
  target.files = esvn
  INSTALLS += target

  icons.path = /usr/share/pixmaps
  icons.files = images/esvn.png
  INSTALLS += icons

  desktop.path = /usr/share/applications
  desktop.files = eSvn.desktop
  INSTALLS += desktop

  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  DEFINES+= VERSION=\"$$VER\"
}

win32 {
   VER=$$system(type VERSION)

   UI_DIR      = tmp\src
   MOC_DIR     = tmp\moc
   DEFINES+= VERSION=\"$$VER\"
}












