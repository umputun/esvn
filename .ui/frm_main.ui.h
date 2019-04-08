/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/



void frm_main::mn_NewWorkspace_activated()
{

}



void frm_main::list_dirs_clicked( QListViewItem* )
{
// if (item)
//  list_dirs_selectionChanged(item);
}


void frm_main::list_dirs_doubleClicked( QListViewItem * )
{

}


void frm_main::list_dirs_collapsed( QListViewItem *item)
{
 list_dirs_doubleClicked(item);
}


void frm_main::list_dirs_expanded( QListViewItem *item)
{
 list_dirs_doubleClicked(item);
}


void frm_main::list_dirs_selectionChanged( QListViewItem * )
{

}


void frm_main::fileExitAction_activated()
{
 close();
}


void frm_main::list_dirs_contextMenuRequested( QListViewItem *, const QPoint &, int )
{
}


void frm_main::list_files_contextMenuRequested( QListViewItem *, const QPoint &, int )
{

}


void frm_main::modifyUpdateAction_activated()
{

}


void frm_main::modifyCommitAction_activated()
{

}



void frm_main::queryStatusAction_activated()
{

}


void frm_main::queryInfoAction_activated()
{

}


void frm_main::queryLogAction_activated()
{

}


void frm_main::list_files_selectionChanged( QListViewItem * )
{

}


void frm_main::list_files_currentChanged( QListViewItem *item )
{
 list_files_selectionChanged(item);
}


void frm_main::list_files_clicked( QListViewItem *item )
{
 if (item)
  list_files_selectionChanged(item);
}


void frm_main::modifyEditAction_activated()
{

}


void frm_main::viewRefresh_localAction_activated()
{

}


void frm_main::onRefreshTimer()
{

}


void frm_main::modifyUpdate_revision_Action_activated()
{

}


void frm_main::list_dirs_dropped( QDropEvent * )
{

}


void frm_main::viewHide_unknownAction_toggled( bool )
{

}


void frm_main::modifyCleanupAction_activated()
{

}


void frm_main::modifyRevertAction_activated()
{

}


void frm_main::fileOpen_workspaceAction_activated()
{

}


void frm_main::modifyAddAction_activated()
{

}


void frm_main::viewRefresh_workspaceAction_activated()
{

}


void frm_main::modifyAdd_recursiveAction_activated()
{

}


void frm_main::modifyDeleteAction_activated()
{

}


void frm_main::modifyCopyAction_activated()
{

}


void frm_main::modifyCopy_current_Action_activated()
{

}


void frm_main::modifySwitchAction_activated()
{

}


void frm_main::modifyMegreAction_activated()
{

}


void frm_main::le_cmd_returnPressed()
{

}


void frm_main::viewClear_output_logAction_activated()
{
 textBrowser->clear();
} 


void frm_main::queryDiffAction_activated()
{

}


void frm_main::queryViewAction_activated()
{

}


void frm_main::helpAboutAction_activated()
{

}


void frm_main::fileOptions_Action_activated()
{

}


void frm_main::queryQuick_DiffAction_activated()
{

}


void frm_main::modifyCopy_remote_Action_activated()
{

}


void frm_main::fileOpenAction_activated()
{

}


void frm_main::fileCheckoutAction_activated()
{

}


void frm_main::list_files_doubleClicked( QListViewItem * )
{

}


void frm_main::modifyResolveAction_activated()
{

}


void frm_main::modifyMoveAction_activated()
{

}



void frm_main::list_dirs_mouseButtonPressed( int, QListViewItem *item, const QPoint &, int )
{
 if (item)
  list_dirs_selectionChanged(item);
}


void frm_main::list_dirs_rightButtonPressed( QListViewItem *item, const QPoint &, int )
{
 if (item)
  list_dirs_selectionChanged(item);
}


void frm_main::query3_Way_DiffAction_activated()
{

}


void frm_main::queryBlameAction_activated()
{

}


void frm_main::queryProperties_activated()
{

}


void frm_main::fileBrowse_Repository_Action_activated()
{

}


void frm_main::fileImport_Action_activated()
{

}


void frm_main::fileExport_Action_activated()
{

}


void frm_main::modifyMake_directory_Action_activated()
{

}


void frm_main::viewRefresh_onlineAction_activated()
{

}


void frm_main::fileWorkspaceAction_activated()
{

}


void frm_main::modifyRevert_recursiveAction_activated()
{

}


void frm_main::helpIndexAction_activated()
{

}


void frm_main::viewToolbarsFileAction_toggled( bool show )
{
 if (show) tlb_file->show();
 else  tlb_file->hide();
}


void frm_main::viewToolbarsViewAction_toggled( bool show)
{
 if (show) tlb_view->show();
 else  tlb_view->hide();
}


void frm_main::viewToolbarsModifyAction_toggled( bool show)
{
 if (show) tlb_modify->show();
 else  tlb_modify->hide();
}


void frm_main::viewToolbarsQueryAction_toggled( bool show)
{
 if (show) tlb_query->show();
 else  tlb_query->hide();
}


void frm_main::helpWebsiteAction_activated()
{

}


void frm_main::helpCheck_for_updatesAction_activated()
{

}


void frm_main::fileShow_changed_filedirectoriesAction_activated()
{

}


void frm_main::viewSwitchFlatView_toggled( bool )
{

}


void frm_main::viewSwitchFlatViewChanged_toggled( bool )
{

}


void frm_main::list_files_mouseButtonClicked( int, QListViewItem *item, const QPoint &, int )
{
 list_files_clicked( item );
}
