/***************************************************************************
 *   Copyright (C) 2004 by umputun                                         *
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

#include "mainwindow.h"
#include "commitwindow.h"
#include "esvn_utils.h"

C_MainWindow::C_MainWindow(QWidget* parent , const char* name )
: frm_main(parent, name), hide_unknown_(true), show_flat_view_(false), show_changed_items_only_(false)
{
	in_timer_now_ = false;

	baseCaption_ = QString(caption() + " " + (char*)VERSION).stripWhiteSpace();
	setCaption(baseCaption_);

	setPopupMenu();
	diff_tmp_file_ = "";
	//workspace_ = ""; // moved to C_DirPath which should give empty path

	for (int i = 0; i < MAX_RECENT_FILES; ++i)
		recent_file_ids_[i] = -1;

	/* style for non resizable dlgs */
	int dlg_Style = WStyle_Customize|WStyle_DialogBorder|WStyle_SysMenu|WStyle_Title|WStyle_ContextHelp;

	last_item_ = NULL;
	dirs_list_ = new C_DirsList(svn_dir_);

	dlg_update_ = new dlg_update(this);
	dlg_info_   = new dlg_info(this);


	dlg_copy_   = new dlg_copy(this, NULL, TRUE, dlg_Style);
	dlg_switch_ = new dlg_switch(this, NULL, TRUE, dlg_Style);

	dlg_merge_  = new C_MergeWindow(this);

	dlg_diff_setup_ = new C_DiffWindow(this, NULL, TRUE, dlg_Style);
	dlg_diff_       = new dlg_diff(this);

	dlg_view_       = new dlg_view(this);

	dlg_about_      = new dlg_about(this);
	dlg_options_    = new dlg_options(this, NULL, TRUE, dlg_Style);
	dlg_checkout_   = new dlg_checkout(this, NULL, TRUE, dlg_Style);
	dlg_import_		= new dlg_import(this);
	dlg_export_		= new dlg_export(this, NULL, TRUE, dlg_Style);
	dlg_startup_ 	= new dlg_startup(this);
	dlg_help_		= new dlg_help(this);

	dlg_help_->tx_help->mimeSourceFactory()->addFilePath("/usr/share/doc/esvn/html-docs");

	dlg_verupdate_	= new dlg_verupdate(this);
	dlg_verupdate_->lb_stable_url->hide();
	dlg_verupdate_->lb_testing_url->hide();
	dlg_verupdate_->lb_browser->hide();

	dlg_workspace_	= new C_WsWindow(this);

	svn_commands_ = new C_SvnCommands(textBrowser, this);
	svn_commands_->setUserPassw(user_, passwd_);

	svn_status_ = new C_SvnStatus(svn_commands_);

	dlg_commit_ = new C_CommitWindow(svn_status_, this);


	dlg_log_ = new C_LogWindow();
	dlg_log_->setParams(svn_commands_, dlg_view_, dlg_diff_);

	dlg_prop_ = new C_PropWindow(this);

	dlg_repos_ = new C_ReposWindow(this);
	dlg_repos_->setParams(svn_commands_, dlg_view_, dlg_checkout_);

	dlg_diff_setup_->setParams(svn_commands_, dlg_view_);

	dlg_merge_->setParams(svn_commands_, dlg_view_);

	dlg_changed_ = new C_ChangedWindow(this);
	dlg_changed_->setParams(svn_commands_, dlg_view_, dlg_diff_);

	refresh_timer_ = new QTimer(this);
	connect(refresh_timer_, SIGNAL(timeout()), this, SLOT(onTimer()) );

	connect(&http_, SIGNAL(done(bool)), this, SLOT(httpDone(bool)) );

	//selected_path_ = ""; // now C_DirPath which is cleaned


	list_stat_ = new C_LSParser(C_LSParser::LSP_STATUS, svn_commands_, list_dirs);


	list_dirs->header()->hide();

	list_files->setColumnAlignment(2, Qt::AlignRight);
	list_files->setColumnAlignment(3, Qt::AlignRight);
	list_files->setColumnAlignment(4, Qt::AlignRight);
	list_files->setColumnAlignment(5, Qt::AlignRight);
	list_files->setColumnAlignment(6, Qt::AlignHCenter);
	list_files->setColumnAlignment(7, Qt::AlignHCenter);

	list_files->setColumnWidthMode(0, QListView::Manual);
	list_files->setColumnWidthMode(1, QListView::Manual);
	list_files->setColumnWidthMode(2, QListView::Manual);
	list_files->setColumnWidthMode(3, QListView::Manual);
	list_files->setColumnWidthMode(4, QListView::Manual);
	list_files->setColumnWidthMode(5, QListView::Manual);
	list_files->setColumnWidthMode(6, QListView::Manual);
	list_files->setColumnWidthMode(7, QListView::Manual);


	C_Options::instance()->setDlg(dlg_options_);

	dir_refresh_disable_ = true;
	auto_refresh_disable_ = false;

	refresh_timer_->start(1000);
}

C_MainWindow::~C_MainWindow() {
}

void C_MainWindow::setPopupMenu() {
	mnu_popup_ = new QPopupMenu;

	mnu_popup_->insertItem(QPixmap::fromMimeSource("update_16x16.xpm"), "Update      ",this,
						   SLOT(modifyUpdateAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("update_16x16.xpm"), "Update revision ...",this,
						   SLOT(modifyUpdate_revision_Action_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("commit_16x16.xpm"), "Commit ...",this,
						   SLOT(modifyCommitAction_activated()));

	mnu_popup_->insertSeparator();

	mnu_popup_->insertItem(QPixmap::fromMimeSource("edit.png"), "Edit",  this,
						   SLOT(modifyEditAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("kdar.png"), "Properties ...",  this,
						   SLOT(queryProperties_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("module_add_16x16.xpm"),"Add", this,
						   SLOT(modifyAddAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("Delete16x16.xpm"),"Delete", this,
						   SLOT(modifyDeleteAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("revert.png"),"Revert", this,
						   SLOT(modifyRevertAction_activated()));

	mnu_popup_->insertSeparator();

	mnu_popup_->insertItem(QPixmap::fromMimeSource("ktip.png"), "Quick Diff",  this,
						   SLOT(queryQuick_DiffAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("services.png"), "Diff ...",  this,
						   SLOT(queryDiffAction_activated()));

	mnu_popup_->insertSeparator();
	mnu_popup_->insertItem(QPixmap::fromMimeSource("merge_16x16.xpm"), "Merge ...",  this,
						   SLOT(modifyMegreAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("backimage.png"),"Switch ...", this,
						   SLOT(modifySwitchAction_activated()));

	mnu_popup_->insertSeparator();
	mnu_popup_->insertItem(QPixmap::fromMimeSource("cvs_annotate_16x16.xpm"), "Info",  this,
						   SLOT(queryInfoAction_activated()));

	mnu_popup_->insertItem(QPixmap::fromMimeSource("LogTree16x16.xpm"), "Log", this,
						   SLOT(queryLogAction_activated()));



}


void C_MainWindow::loadWorkspace(const C_DirPath& path, QString name, bool append)
{
	if (path.FullPath().isEmpty() && workspace_.FullPath().isEmpty()) {

		workspace_ = QDir::homeDirPath();

		if (dlg_startup_->exec()  == QDialog::Accepted ) {
			if (dlg_startup_->rb_open_ws->isChecked()) {
				fileOpen_workspaceAction_activated();
				return;
			}
			if (dlg_startup_->rb_checkout->isChecked()) {
				fileCheckoutAction_activated();
				return;
			}
			if (dlg_startup_->rb_import->isChecked()) {
				fileImport_Action_activated();
				return;
			}
		}
	}

	if (path.FullPath().isEmpty() || !QDir(path.FullPath()).exists()) {
		QMessageBox::information(this, "Open failed", "Workspace/directory does not exist");
		return;
	}

	QDir old_workspace_dir(workspace_.FullPath());

	workspace_ = path;
//	selected_path_ = path; // now dynamic-cast selected dir item to C_DirPath and use the result

	if (name == "noname") {
		//int pos = workspace_.findRev('/');
        if (workspace_.Name().isEmpty()) {
            name = workspace_.Path();
        } else {
            name = workspace_.Name();
        }
	}

	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	dir_refresh_disable_ = true;

	qApp->processEvents(); qApp->processEvents(); qApp->processEvents(); qApp->processEvents();qApp->processEvents();
	qApp->processEvents(); qApp->processEvents(); qApp->processEvents(); qApp->processEvents();qApp->processEvents();

	list_dirs->blockSignals(true);

	dirs_list_->setSvnStatus(svn_status_);
	//dirs_list_->load(path, list_dirs, NULL, true, dlg_options_->ch_disablesl->isChecked(), append);

	if (!dirs_list_->load(workspace_, list_dirs, NULL, true, dlg_options_->ch_disablesl->isChecked(), append)) {
		workspace_ = old_workspace_dir;
		QMessageBox::information(this, "Open failed", "Can't open workspace/directory");
		dir_refresh_disable_ = false;
		QApplication::restoreOverrideCursor();
		list_dirs->blockSignals(false);
		return;
	}


	list_dirs->setColumnText(0, name);

	dir_refresh_disable_ = false;

	list_dirs->setSelected(list_dirs->firstChild(), true);
	//last_selected_path_ = "";
	viewRefresh_localAction_activated();

	viewHide_unknownAction->setOn(hide_unknown_);

	QApplication::restoreOverrideCursor();
	list_dirs->blockSignals(false);

	setCaption(workspace_.FullPath() + " - " + baseCaption_);

}


void C_MainWindow::updateRecentFileItems() {

	while ((int)recent_files_.size() > MAX_RECENT_FILES)
		recent_files_.pop_back();

	for (int i = 0; i < (int)recent_files_.size(); ++i) {
		QString text = recent_files_[i];

		if (recent_file_ids_[i] == -1) {
			if (i == 0)
				fileMenu->insertSeparator(fileMenu->count() - 2);
			recent_file_ids_[i] = fileMenu->insertItem(text, this, SLOT(openRecentFile(int)),
													   0, -1,
													   fileMenu->count() - 2);

			fileMenu->setItemParameter(recent_file_ids_[i], i);
		}
		else {
			fileMenu->changeItem(recent_file_ids_[i], text);
		}
	}
}

void C_MainWindow::openRecentFile(int param)
{
	QString recent_name = recent_files_[param];
	int pos = recent_name.find(" [ws]");
	if (pos == -1) {
        C_DirPath wsPath = QDir(recent_name);
		loadWorkspace(wsPath);
		QSettings settings_;
		settings_.setPath("umputun.com", "esvn", QSettings::User);
		settings_.beginGroup("/esvn");
		settings_.writeEntry("/state/active_workspace", "");
		settings_.endGroup();
	}
	else {
		recent_name = recent_name.left(pos);
		loadWS(recent_name);
	}


}



//----------------------------- SLOTS (events) -------------------------------
void C_MainWindow::list_dirs_doubleClicked(QListViewItem *item)
{
	//cout << "dbl" << endl;
    C_DirPath dir;
    dir.SetPath(workspace_.FullPath());
    dir.SetName(item->text(0));

	C_DirViewItem* dir_item = dynamic_cast<C_DirViewItem*>(item);
	if (!dir_item) {
		// should not happen as all dir view items are of type C_DirViewItem
		return;
	}
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
	dirs_list_->load(dir, list_dirs, dir_item);
	QApplication::restoreOverrideCursor();
}

void C_MainWindow::list_dirs_selectionChanged(QListViewItem *item) {

    const C_DirPath *sel_path = dynamic_cast<C_DirPath*>(item);
    if (!sel_path) {
        statusBar()->message("No directory selection");
        return;
    }

	statusBar()->message(sel_path->FullPath());

	is_selected_dir_ = true;

	if (last_item_ == item)
		return;

	last_item_ = item;

	viewRefresh_localAction_activated();
}



void C_MainWindow::list_files_selectionChanged(QListViewItem *item)
{
    const C_Path *sel_path = dynamic_cast<const C_Path*>(item);
    if (!sel_path) {
        sendToOutput("No selection");
    }

    is_selected_dir_ = false;

    sendToOutput("selected: " + sel_path->Name());
}

void C_MainWindow::list_dirs_contextMenuRequested( QListViewItem *, const QPoint &xy, int ) {
	mnu_popup_->popup(xy);
}

void C_MainWindow::list_files_contextMenuRequested( QListViewItem *, const QPoint &xy, int ) {
	mnu_popup_->popup(xy);
}

void C_MainWindow::list_files_doubleClicked( QListViewItem *view_item)
{
    C_NumSortedFileListItem* item = dynamic_cast<C_NumSortedFileListItem*>(view_item);
    if (!item) {
        // not our selection
        return;
    }

    if ( item->isDir() ) {

		QListViewItem *parent = list_dirs->currentItem();

        if (parent) {

			//colorizeDirsList(parent);

			if (!parent->isOpen())
				list_dirs_doubleClicked(parent);

			QListViewItemIterator iter(parent, QListViewItemIterator::Visible);
			while (iter.current()) {
                C_DirViewItem *dir_item = dynamic_cast<C_DirViewItem*>(iter.current());
                //cout << EsvnLVI::getFullPath(item) << " " <<  curr_item->text(1) << endl;

				if (dir_item && item->FullPath() == dir_item->FullPath()) {
					emit list_files_selectionChanged(dir_item);
					//parent->setSelected(false);
					//curr_item->setSelected(true);
					list_dirs->setCurrentItem(dir_item);
					viewRefresh_localAction_activated();

					return;
				}
				++iter;
			}
		}
		else
            show_Log(*item);
	}
	else {

		if (dlg_options_->rb_cat->isChecked())
			show_View(*item);

		if (dlg_options_->rb_extedit->isChecked())
			modifyEditAction_activated();

		if (dlg_options_->rb_open->isChecked()) {

			QFileInfo fi( item->FullPath() );

			//find prog
			for (size_t i=0; i<open_map_.size(); i++) {
				QString str = open_map_[i];
				QStringList ext_prog = QStringList::split("|", str);
				if (ext_prog.size() < 2)
					continue;
				QStringList exts = QStringList::split(",", ext_prog[0]);
				for (size_t j=0; j<exts.size(); j++) {
					if (exts[j] == fi.extension() ) {
						QProcess proc;
                        proc.setWorkingDirectory(item->Path());
						proc.addArgument(ext_prog[1]);
						proc.addArgument(item->Name());
						textBrowser->append(ext_prog[1] + " " + item->Name());
						proc.start();
						return;
					}
				}
			}

            show_View(*item); //can be here if action not found

		}

	}
}

void C_MainWindow::colorizeDirsList(QListViewItem *dir_item)
{
	C_NumSortedFileListItem *curr_item = dynamic_cast<C_NumSortedFileListItem*>(list_files->firstChild());
	while (curr_item) {
		C_DirViewItem *item = dynamic_cast<C_DirViewItem*>(dir_item->firstChild());
		while (item) {
			//cout << EsvnLVI::getFullPath(item) << " " <<  EsvnLVI::getFullPath(curr_item) << endl;
			if (item->FullPath() == curr_item->FullPath()) {
				item->setPixmap(0, *curr_item->pixmap(0));
				break;
			}
			item = dynamic_cast<C_DirViewItem*>(item->nextSibling());
		}
		curr_item = dynamic_cast<C_NumSortedFileListItem*>(curr_item->nextSibling());
	}
}


void C_MainWindow::modifyUpdateAction_activated()
{
	sendToOutput("update");

	in_timer_now_ = true;

    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "update: No selection available" << std::endl;
        return;
    }

    svn_commands_->update(selection);

	in_timer_now_ = false;
}

void C_MainWindow::modifyUpdate_revision_Action_activated() {

	sendToOutput("update revision");

	if (dlg_update_->exec() == QDialog::Accepted) {
		QString rev;
		if (dlg_update_->rb_revision->isChecked())
			rev = (dlg_update_->le_revision->text());
		else
			rev	= "{" + dlg_update_->le_date->text() + "}";

        PathSelection selection;

        getSelectedPaths(selection);

        if (selection.empty()) {
            std::cout << "update revision: No selection available" << std::endl;
            return;
        }

		svn_commands_->update(selection, rev);
	}
}


void C_MainWindow::modifyRevertAction_activated()
{
	revert(false);
}

void C_MainWindow::modifyRevert_recursiveAction_activated()
{
	revert(true);
}

void C_MainWindow::revert(bool is_recursive)
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "revert: No selection available" << std::endl;
        return;
    }

    QString text_string ="You are about to revert the following items:\n\n";
    for (size_t i=0; i< selection.size(); i++)
        text_string = text_string + selection[i].Name() + "\n";

    QMessageBox mb( "Confirmation",text_string,
        QMessageBox::Question,
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No  | QMessageBox::Escape,
        QMessageBox::NoButton );

    if ( mb.exec() == QMessageBox::Yes )
        svn_commands_->revert(selection, is_recursive);
}

void C_MainWindow::modifyResolveAction_activated()
{
    // Get current view
    QListView* current_view = is_selected_dir_?list_dirs:list_files;
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(current_view);

    const C_Path* sel_path = dynamic_cast<C_Path*>(item);

    if (!sel_path) {
        std::cout << "modifyResolveAction_activated: No path available" << std::endl;
        return;
    }

    QMessageBox mb( "Confirmation",
					"You are about to resolve the following item:\n\n" + sel_path->Name() +"\n",
					QMessageBox::Question,
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No  | QMessageBox::Escape,
					QMessageBox::NoButton );

	if ( mb.exec() == QMessageBox::Yes )
		svn_commands_->resolve(*sel_path);
}


void C_MainWindow::modifyCommitAction_activated() {
    // Get current view
    //QListView* current_view = is_selected_dir_?list_dirs:list_files;
    //QListViewItem* item = EsvnLVI::getFirstSelectedItem(current_view);
    const C_DirPath *sel_path = dynamic_cast<C_DirPath*>(EsvnLVI::getFirstSelectedItem(list_dirs));
    if (!sel_path) {
		QMessageBox::information(this, "Commit rejected", "No directory selection available");
        return;
    }
    QString currwd = sel_path->FullPath();

	if (dlg_options_->ch_disable_cotag->isChecked() && getUrlOnline(*sel_path).find("/tags/") != -1) {
		QMessageBox::information(this, "Commit rejected", "Can't commit to tag");
		return;
	}

	if (dlg_options_->ch_hide_commit->isChecked())
		dlg_commit_->gr_files->hide();
	else
		dlg_commit_->gr_files->show();

    vector<QString> selected_files;

	int commit_res = -1;
        if( is_selected_dir_ ) {
            dlg_commit_->setCommitPath( *sel_path, !dlg_options_->ch_hide_commit->isChecked());
        }
        else {
            getSelectedFiles(selected_files);
            dlg_commit_->setCommitFiles( selected_files );
        }

	if (dlg_commit_->exec() == QDialog::Accepted) {

        selected_files = dlg_commit_->commitList();

        // user may have accepted no files to commit, so check if list is empty
        if (!selected_files.empty())
            commit_res = svn_commands_->commit(currwd, selected_files, dlg_commit_->msgEdit->text());
    }
}

void C_MainWindow::modifyEditAction_activated()
{
    // Check if there is any selection in file list
    if (!list_files->currentItem()) {
        //QMessageBox::information(this, "Edit action rejected", "Can not edit directory ");
        return;
    }

    // Get list of all selected paths
    PathSelection selection;
    if (!getSelectedFiles(selection)) {
        QMessageBox::information(this, "Edit action rejected", "No selection");
        return;
    }

    for (PathSelection::const_iterator it = selection.begin(); it != selection.end(); ++it) {
        const C_Path& path = *it;
        if (!path.isDir()) {
            QProcess proc;
            proc.setWorkingDirectory(path.Path());
            proc.addArgument(editor_set_);
            proc.addArgument(path.Name());
            proc.start();
        } else {
            std::cout << "Cannot edit directory" << path.Name() << std::endl;
        }
    }
}

void C_MainWindow::modifyCleanupAction_activated() {
    PathSelection selection;
    if (!getSelectedPaths(selection)) {
        QMessageBox::information(this, "Cleanup action rejected", "No selection");
        return;
    }

    // Better extract directories then call cleanup on list
    for (PathSelection::const_iterator it = selection.begin(); it != selection.end(); ++it) {
        const C_Path& path = *it;
        if (path.isDir()) {
            svn_commands_->cleanup(path);
        } else {
            std::cout << "Cannot cleanup non-directory" << path.Name() << std::endl;
        }
    }
}


void C_MainWindow::modifyAddAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyAddAction_activated: No selection available" << std::endl;
        return;
    }

    svn_commands_->add(selection, false);
}

void C_MainWindow::modifyAdd_recursiveAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyAdd_recursiveAction_activated: No selection available" << std::endl;
        return;
    }

    svn_commands_->add(selection, true);
}

void C_MainWindow::modifyDeleteAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyDeleteAction_activated: No selection available" << std::endl;
        return;
    }

    // note that for file selection working directory is also available in each selection item
    // though it should refer to sel_path that we already extracted from dir selection
    svn_commands_->del(selection);
}

void C_MainWindow::modifyCopyAction_activated()
{
    // current working directory is stored in dlg_copy_->lb_url

    if (dlg_copy_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_copy_->rb_revision->isChecked())
			rev = (dlg_copy_->le_revision->text());
		else
			rev	= "{" + dlg_copy_->le_date->text() + "}";

		QStringList files = QStringList::split(';', dlg_copy_->le_source->text());

        QString currwd = dlg_copy_->lb_url->text();

        for (size_t i=0; i<files.size(); i++)
			svn_commands_->copy(currwd, files[i], dlg_copy_->le_dest->text(), dlg_copy_->te_msg->text(), rev);
	}
}

void C_MainWindow::modifyCopy_remote_Action_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyCopy_remote_Action_activated: No path selected" << std::endl;
        return;
    }

	dlg_copy_->lb_url->hide();
	dlg_copy_->setCaption("Copy (remote)");
	dlg_copy_->lb_url->setText(getUrlOnline(selection[0]));

	dlg_copy_->tb_src->setText("U");
	dlg_copy_->tb_dst->setText("U");

    QString sel_names;
    for (size_t i=0; i<selection.size(); i++) {
        QString url = getUrlOnline(selection[i]);
        sel_names = sel_names + url;
        if (i != selection.size()-1)
            sel_names = sel_names + ';';
    }
	dlg_copy_->le_source->setText(sel_names);
	//dlg_copy_->le_dest->setText(sel_names);

	modifyCopyAction_activated();
}

void C_MainWindow::modifyCopy_current_Action_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyCopy_current_Action_activated: No selection available" << std::endl;
        return;
    }

	dlg_copy_->lb_url->hide();
	dlg_copy_->setCaption("Copy (local)");
	dlg_copy_->lb_url->setText(selection[0].Path());

	dlg_copy_->tb_src->setText("...");
	dlg_copy_->tb_dst->setText("...");

    QString sel_names;
    for (size_t i=0; i<selection.size(); i++) {
        sel_names = sel_names + selection[i].Name();
        if (i != selection.size()-1)
            sel_names = sel_names + ';';
    }
    dlg_copy_->le_source->setText(sel_names);

	modifyCopyAction_activated();
}

void C_MainWindow::modifyMoveAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "modifyDeleteAction_activated: No selection available" << std::endl;
        return;
    }

    dlg_copy_->setCaption("Move");

    // Prepare list of selected names
	dlg_copy_->tb_src->setText("...");
	dlg_copy_->tb_dst->setText("...");

    dlg_copy_->le_source->setText("");
    QString sel_names;
    for (size_t i=0; i<selection.size(); i++) {
        sel_names = sel_names + selection[i].Name();
        if (i != selection.size()-1)
            sel_names = sel_names + ';';
    }
    dlg_copy_->le_source->setText(sel_names);

	if (dlg_copy_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_copy_->rb_revision->isChecked())
			rev = (dlg_copy_->le_revision->text());
		else
			rev	= "{" + dlg_copy_->le_date->text() + "}";

		QStringList files = QStringList::split(';', dlg_copy_->le_source->text());

		for (size_t i=0; i<files.size(); i++)
			svn_commands_->move(selection[0].Path(), files[i], dlg_copy_->le_dest->text(),dlg_copy_->te_msg->text(), rev);
	}
}


void C_MainWindow::modifySwitchAction_activated() {

    // Get current view
    QListView* current_view = is_selected_dir_?list_dirs:list_files;
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(current_view);
    const C_Path* path = dynamic_cast<C_Path*>(item);
    if (!path) {
        std::cout << "modifySwitchAction_activated: No path available" << std::endl;
        return;
    }

	QString url = getUrlOnline(*path);
	dlg_switch_->le_url->setText(url);

	if (dlg_switch_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_switch_->rb_revision->isChecked())
			rev = (dlg_switch_->le_revision->text());
		else
			rev	= "{" + dlg_switch_->le_date->text() + "}";

		svn_commands_->switch_cmd(*path, dlg_switch_->le_url->text(), rev, dlg_switch_->ch_recur->isChecked());
	}
}


void C_MainWindow::modifyMegreAction_activated()
{
    // Get current view
    QListView* current_view = is_selected_dir_?list_dirs:list_files;
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(current_view);

    const C_Path* sel_path = dynamic_cast<C_Path*>(item);

    if (!sel_path) {
        std::cout << "modifyMergeAction_activated: No path available" << std::endl;
        return;
    }

	dlg_merge_->setFileDirUrl(*sel_path, getUrlOnline(*sel_path));

	if (dlg_merge_->exec() == QDialog::Accepted) {
		QString first = "";
		QString second= "";

		if (dlg_merge_->rb_revision->isChecked()) {
			first = dlg_merge_->le_rev1->text();
			second= dlg_merge_->le_rev2->text();
		}
		else {
			first = dlg_merge_->le_url1->text();
			second= dlg_merge_->le_url2->text();
		}

        QString file_url = sel_path->Name();
		if (dlg_merge_->le_file->text() != "")
			file_url = dlg_merge_->le_file->text();

        QString wcpath;
        if (dlg_merge_->le_wcpath->text() != "") {
            wcpath = dlg_merge_->le_wcpath->text();
        }
        else if (sel_path->isDir()) {
            // Provide correct working path for selected directories
            wcpath = sel_path->Name();
        }

        svn_commands_->merge(sel_path->Path(),
                             file_url, first, second, wcpath,
							 dlg_merge_->rb_revision->isChecked(),
							 dlg_merge_->ch_nonrecursive->isChecked(),
							 dlg_merge_->ch_dryrun->isChecked());
	}
}

void C_MainWindow::modifyMake_directory_Action_activated()
{
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(list_dirs);
    const C_Path* sel_path = dynamic_cast<C_Path*>(item);

    if (!sel_path) {
        std::cout << "modifyMake_directory_Action_activated: No path available" << std::endl;
        return;
    }

    bool ok;
    QString text = QInputDialog::getText("Make directory",
            "Enter directory name:", QLineEdit::Normal,
            QString::null, &ok, this );

	if ( ok && !text.isEmpty() )
		svn_commands_->mkdir(sel_path->FullPath(), text);
}

///------------------- VIEW actions -------------------------------------------------

void C_MainWindow::refreshList()
{
	auto_refresh_disable_ = true;

	int saved_pos =  list_files->contentsY(); // ist_files->verticalScrollBar()->value();

    //QString sel_path = list_dirs->selectedItem() ? list_dirs->selectedItem()->text(EsvnLVI::ITEM_FULL_PATH) : workspace_;

    const C_Path *sel_path = NULL;

    // Try dir selection first, then revert to workspace
    if (list_dirs->selectedItem()) {
        sel_path = dynamic_cast<C_Path*>(list_dirs->selectedItem());
    }
    if (!sel_path || sel_path->FullPath().isEmpty()) {
        sel_path = &workspace_;
    }

	if (sel_path->FullPath().isEmpty()) {
		sendToOutput("refresh: invalid path");
		list_files->clear();
		auto_refresh_disable_ = false;
		return;
	} else {
		sendToOutput("refresh: " +  sel_path->FullPath() );
	}

	vector<QString> selected_files;
	getSelectedFiles(selected_files);

	//for (map<QString, S_StatusInfo>::iterator iter_i =svn_status_->getData()->begin(); iter_i != svn_status_->getData()->end(); iter_i++) {
	//	cout << "***" << iter_i->first << " " << iter_i->second.status_ << endl;
	//}

    {
        C_FilesList file_list;
        file_list.load(*sel_path, list_files, hide_unknown_, show_flat_view_, show_changed_items_only_, dlg_options_->ch_showdir->isChecked(), svn_status_);
    }

	//restore selection

	QListViewItem *item = list_files->firstChild();
	while (item != 0) {
		for (size_t i=0; i<selected_files.size(); i++) {
			if (item->text(0) == selected_files[i]) {
				item->setSelected(true);
				list_files->setCurrentItem(item);
				break;
			}
		}
		item = item->itemBelow() ;
	}

	//list_files->verticalScrollBar()->setValue(saved_pos); //restore scrool
	//scroll_pos_ = saved_pos;

	if (saved_pos)
		list_files->setContentsPos (0,  saved_pos);

	//cout << "restored:" << list_files->verticalScrollBar()->value() << endl;

	time_t now; time(&now);
	last_refresh_ = now;

	getFilesAndSize(*sel_path, last_files_and_size_);

	auto_refresh_disable_ = false;
}

void C_MainWindow::viewRefresh_localAction_activated()
{
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	time(&last_refresh_);
	if (dlg_options_->rb_online->isChecked())
		fillSvnStatus(svn_status_);


	refreshList();
	QApplication::restoreOverrideCursor();
}

void C_MainWindow::viewRefresh_onlineAction_activated()
{
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	fillSvnStatus(svn_status_);
	refreshList();

	QApplication::restoreOverrideCursor();
}

void C_MainWindow::viewHide_unknownAction_toggled(bool hide) {
	//cout <<"hide:" << hide << endl;
	hide_unknown_ = hide;
	dirs_list_->setHide(hide);
	viewRefresh_localAction_activated();
}

void C_MainWindow::viewRefresh_workspaceAction_activated()
{

	QSettings settings_;
	settings_.setPath("umputun.com", "esvn", QSettings::User);
	settings_.beginGroup("/esvn");

	QString ws_name = settings_.readEntry("/state/active_workspace", "");
	if (ws_name.isEmpty()) {
		loadWorkspace(workspace_);
	}
	else {
		dir_refresh_disable_ = false;
		loadWS(ws_name);
	}

	settings_.endGroup();

}



void C_MainWindow::le_cmd_returnPressed()
{
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(list_dirs);
    const C_Path* sel_path = dynamic_cast<C_Path*>(item);

    if (!sel_path) {
        std::cout << "command: No path available" << std::endl;
        return;
    }

    QString cmd = cmd_LineEdit->text();
	cmd_LineEdit->setText("");

	QStringList elems = QStringList::split(' ', cmd);

	svn_commands_->command(sel_path->FullPath(), elems);
}

void C_MainWindow::show_Log(const C_Path& pathname, bool show_changed_paths)
{
	dlg_log_->setFileDir(pathname);
	dlg_log_->load(show_changed_paths);
	dlg_log_->show();
}

void C_MainWindow::show_Log(const C_Path& pathname)
{
    bool show_changed_paths = dlg_options_->ch_showchp->isChecked();
    show_Log(pathname, show_changed_paths);
}

void C_MainWindow::queryLogAction_activated()
{
    // Get current view
    QListView* current_view = is_selected_dir_?list_dirs:list_files;
    QListViewItem* item = EsvnLVI::getFirstSelectedItem(current_view);

    const C_Path* sel_path = dynamic_cast<C_Path*>(item);

    if (!sel_path) {
        std::cout << "modifyCopy_remote_Action_activated: No path available" << std::endl;
        return;
    }

    show_Log(*sel_path);
}

void C_MainWindow::queryStatusAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryStatusAction_activated: No selection available" << std::endl;
        return;
    }

    svn_commands_->status(selection);
}

void C_MainWindow::show_View(const C_Path& pathname)
{
    QStringList cat;
	bool res = svn_commands_->cat(pathname,"", cat);
	if (res == 0) {
		dlg_view_->tb_view->clear();
		for (size_t i=0; i<cat.size(); i++) {
			if (cat[i].isEmpty())
				dlg_view_->tb_view->append(" ");
			else
				dlg_view_->tb_view->append(cat[i]);
		}

		dlg_view_->tb_view->verticalScrollBar()->setValue(0);

		dlg_view_->setCaption("View");
		dlg_view_->show();
	}
}

void C_MainWindow::queryViewAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryViewAction_activated: No selection available" << std::endl;
        return;
    }

    show_View(selection[0]);
}

void C_MainWindow::queryBlameAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryBlameAction_activated: No selection available" << std::endl;
        return;
    }

	QMessageBox mb( "Confirmation",
					"You are about to execute \"blame\" command for the following item. \n"
                    "It may take a while!\n\n" + selection[0].Name() +"\n",
					QMessageBox::Question,
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No  | QMessageBox::Escape,
					QMessageBox::NoButton );

	if ( mb.exec() != QMessageBox::Yes )
		return;


	QStringList blame;
	bool res = svn_commands_->blame(selection[0],"", blame);
	if (res == 0) {
		dlg_view_->tb_view->clear();
		for (size_t i=0; i<blame.size(); i++)
			dlg_view_->tb_view->append(blame[i]);

		dlg_view_->tb_view->verticalScrollBar()->setValue(0);

		dlg_view_->setCaption("Blame");
		dlg_view_->show();
	}
}



void C_MainWindow::queryInfoAction_activated()
{
    bool res = false;
	//cout << "info" << endl;

    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryInfoAction_activated: No selection available" << std::endl;
        return;
    }

	QStringList info;
    res = svn_commands_->info(selection, info);

	if (res == 0) {
		dlg_info_->text_info->clear();
		for (size_t i=0; i<info.size(); i++)
			dlg_info_->text_info->append(info[i]);

		dlg_info_->show();
	}
}


void C_MainWindow::queryDiffAction_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryDiffAction_activated: No selection available" << std::endl;
        return;
    }

    for (size_t i = 0; i < selection.size(); ++i) {
        const C_Path& current = selection[i];

        dlg_diff_setup_->lb_url->hide();
        QString url = getUrlOnline(current);
        dlg_diff_setup_->lb_url->setText(url);

        dlg_diff_setup_->le_url1->setText(current.Name());

        dlg_diff_setup_->setFileDir(current);

        if (dlg_diff_setup_->exec() == QDialog::Accepted) {
            QString rev;
            if (dlg_diff_setup_->rb_revision->isChecked())
                rev = (dlg_diff_setup_->le_revision->text());
            else
                rev	= "{" + dlg_diff_setup_->le_date->text() + "}";

            QString first = dlg_diff_setup_->le_url1->text();


            if (first.isEmpty())
                first = current.Name();


            if (!dlg_diff_setup_->ch_extdiff->isChecked())
                internalDiff(current.Path(),
                    first, dlg_diff_setup_->le_url2->text(), rev,
                    dlg_diff_setup_->ch_nonrecursive->isChecked(), dlg_diff_setup_->ch_noticea->isChecked(),
                    dlg_diff_setup_->le_ext->text());
            else {
                QStringList out;
                svn_commands_->diff(current.Path(),
                    first, dlg_diff_setup_->le_url2->text(), rev, true,
                    dlg_diff_setup_->ch_nonrecursive->isChecked(), dlg_diff_setup_->ch_noticea->isChecked(),
                    dlg_diff_setup_->le_ext->text(), out);
            }
        }
    }
}



void C_MainWindow::query3_Way_DiffAction_activated()
{
    PathSelection selection;
    getSelectedFiles(selection);

    if (selection.empty()) {
        std::cout << "Diff3: No file selected" << std::endl;
        return;
    }

    for (PathSelection::const_iterator it = selection.begin(); it != selection.end(); ++it)
    {
		const C_Path& path = *it;
		if (path.isDir()) {
			QString text = "Diff3: " + path.Name() + " is a directory";
			textBrowser->append(text);
			continue;
		}

		// Need to load entries for each selected item current directory
		QString full_path = path.FullPath();
		C_DirPath file_level;

		// Descend to file current directory

		int pos = full_path.findRev('/');

		if (pos == -1)
		{
			file_level.SetPath("");
			file_level.SetName(full_path);
		}
		else
		{
			file_level.SetPath(full_path.left(pos));
			file_level.SetName(full_path.mid(pos+1));
		}

		// Load entries

		C_SvnEntries svn_entries;
		C_DirPath currwd;
		currwd.SetPath(file_level.Path());
		svn_entries.setDir(currwd, false);

        C_SvnFile svn_file = svn_entries.get(file_level.Name());

        if(svn_file.conflict_new_.isNull() || svn_file.conflict_old_.isNull()
            || svn_file.conflict_wrk_.isNull()){

			// Display item name as in flat view panel - use original selection
            QString text = "Diff3: " + path.Name() + " is not in conflict";
            //cout << text << endl;
            textBrowser->append(text);
            continue;
        }

        QStringList args;
        args.push_back(QDir::convertSeparators(qApp->applicationDirPath() + "/esvn-diff-wrapper") );
        args.push_back(dlg_options_->le_diff_3way->text());
        args.push_back(svn_file.conflict_wrk_);
        args.push_back(svn_file.conflict_old_);
        args.push_back(svn_file.conflict_new_);
		args.push_back(svn_file.name_);

        sendToOutput("Diff3: " + svn_file.conflict_wrk_ +  " " + svn_file.conflict_old_ + " " + svn_file.conflict_new_ );
        svn_commands_->command(file_level.Path(), args);
    }
}



void C_MainWindow::queryQuick_DiffAction_activated()
{
    // Get current view
    QListView* current_view = is_selected_dir_?list_dirs:list_files;

    // Get selection from current view
    QListViewItem *selected_file_item = EsvnLVI::getFirstSelectedItem(current_view);//current_view->selectedItem();

    const C_Path *sel_path = dynamic_cast<C_Path*>(selected_file_item);

    if (!sel_path) {
        sendToOutput("quick diff: no selection available");
        return;
    }

    QString diff_rev = "HEAD";
	dlg_diff_->setCaption("Diff [HEAD]");
	if (dlg_options_->rb_online->isChecked() && list_files->currentItem()->text(1) == "up-to-date") {
		diff_rev = "PREV";
		dlg_diff_->setCaption("Diff [PREV]");
	}

	internalDiff(sel_path->Path(), sel_path->Name(), "", diff_rev,  false, false, "");
}

void C_MainWindow::internalDiff(const QString& currwd,
    const QString& file_dir_name_1, const QString& file_dir_name_2,
	const QString& rev, bool non_recursive, bool notice_ancestry,
	const QString& extensions)
{
    QStringList diff_res;
	int res = svn_commands_->diff(currwd,
                                  file_dir_name_1, file_dir_name_2, rev, false,
								  non_recursive, notice_ancestry, extensions, diff_res);

	if (res == 0) {
		dlg_diff_->tx_diff->clear();
		for (size_t i=0; i<diff_res.size(); i++) {
			QString str = diff_res[i];

			str.replace("&",  "&amp;");
			str.replace("<",  "&lt;" );
			str.replace(">",  "&gt;" );
			str.replace(" ",  "&nbsp;");
			str.replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");

			if ( str.length() > 0 && str[0] == '+') {
				dlg_diff_->tx_diff->append("<font color=blue>" + str + " </font>");
				continue;
			}
			if (str.length() > 0 && str[0] == '-') {
				dlg_diff_->tx_diff->append("<font color=red>" + str + " </font>");
				continue;
			}
			dlg_diff_->tx_diff->append(str);

		}
		dlg_diff_->tx_diff->verticalScrollBar()->setValue(0);
		dlg_diff_->show();
	}
}


void C_MainWindow::queryProperties_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (selection.empty()) {
        std::cout << "queryProperties_activated: No selection available" << std::endl;
        return;
    }

	dlg_prop_->setFileDirName(selection[0]);
	dlg_prop_->refresh();
	dlg_prop_->exec();
}





void C_MainWindow::onTimer()
{
	if (in_timer_now_)
		return;

	in_timer_now_ = true;

	//refresh after svn command
	if (svn_commands_->needs_refresh_) {
		svn_commands_->needs_refresh_ = false;
		viewRefresh_localAction_activated();
//		getFilesAndSize(selected_path_, last_files_and_size_);
		in_timer_now_ = false;
		return;
	}

    const C_DirPath* sel_path = dynamic_cast<C_DirPath*>(list_dirs->selectedItem());

	//refresh if directory content changed
	if (sel_path && !sel_path->FullPath().isEmpty()) {
		vector<QString> curr_files_and_size;
		getFilesAndSize(*sel_path, curr_files_and_size);

		if (curr_files_and_size != last_files_and_size_) {
			//cout <<selected_path_ <<  last_files_and_size_.size() << " " << curr_files_and_size.size() << endl;
			sendToOutput("auto-refresh: " + sel_path->FullPath() +  " changed" );
			last_files_and_size_ = curr_files_and_size;
			//if (last_selected_path_ == selected_path_ || last_selected_path_.isEmpty())
				viewRefresh_localAction_activated();

			//last_selected_path_ = selected_path_;
		}
	}


	//auto-refresh
	if (sel_path && !auto_refresh_disable_ && dlg_options_->sb_refresh->value() > 0 && dlg_options_->rb_online->isChecked()) {
		time_t now;
		time(&now);
		if (now - last_refresh_ > dlg_options_->sb_refresh->value() ) {
			C_SvnStatus curr_svn_status(svn_commands_);
			fillSvnStatus(&curr_svn_status);
			if (curr_svn_status.cmpData(svn_status_) ) {
				sendToOutput("auto-refresh on-line: " + sel_path->FullPath() +  "changed");
				svn_status_->setData(&curr_svn_status);
				viewRefresh_localAction_activated();
				//workspace_rev_list_ = curr_rev_list_;
			}
			last_refresh_ = now;
		}

	}

	in_timer_now_ = false;

}


void C_MainWindow::fileOpen_workspaceAction_activated() {

	QFileDialog* fd = new QFileDialog( this, "open workspace", TRUE );
	fd->setMode(QFileDialog::DirectoryOnly);
	if (fd->exec() == QDialog::Accepted) {
		QString path = fd->selectedFile();
		if (path.endsWith("/"))
			path.remove(path.length()-1, 1);

		//cout << path << endl;

		recent_files_.remove(path);
		recent_files_.push_front(path);
		updateRecentFileItems();

        C_DirPath wsPath = QDir(path);
		loadWorkspace(wsPath);

		QSettings settings_;
		settings_.setPath("umputun.com", "esvn", QSettings::User);
		settings_.beginGroup("/esvn");
		settings_.writeEntry("/state/active_workspace", "");
		settings_.endGroup();
	}
}

void C_MainWindow::fileOptions_Action_activated() {
	if (dlg_options_->exec() == QDialog::Accepted) {
		subversion_set_ = dlg_options_->le_svn->text();

		svn_commands_->setSvn(subversion_set_);

		editor_set_ = dlg_options_->le_editor->text();
		diff_set_ = dlg_options_->le_diff->text();
		is_ext_diff_ = dlg_options_->ch_extdiff->isChecked();
		dlg_diff_setup_->ch_extdiff->setChecked(is_ext_diff_);

		//hide_unknown_ = dlg_options_->ch_hide->isChecked();

		viewHide_unknownAction->setOn(hide_unknown_);
		svn_commands_->setUserPassw(dlg_options_->le_user->text(), dlg_options_->le_passw->text());

		dlg_log_->useExtDiff(is_ext_diff_);
		dlg_log_->gr_filters->setHidden(dlg_options_->ch_hide_filters->isChecked());

		dlg_prop_->setParams(svn_commands_, dlg_options_->ch_disable_sprop->isChecked());

		setFonts();
		saveSettings();
		getOpenMap();
	}
}

void C_MainWindow::fileCheckoutAction_activated() {
	sendToOutput("checkout");
	if (dlg_checkout_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_checkout_->rb_revision->isChecked())
			rev = (dlg_checkout_->le_revision->text());
		else
			rev	= "{" + dlg_checkout_->le_date->text() + "}";

		svn_commands_->checkout(QDir::currentDirPath(),
                                dlg_checkout_->le_url->text(),
								dlg_checkout_->le_lpath->text(),
								dlg_checkout_->le_user->text(),
								dlg_checkout_->le_password->text(),
								rev,  dlg_checkout_->ch_nonrecursive->isChecked());

	}
}


void C_MainWindow::fileBrowse_Repository_Action_activated()
{
    PathSelection selection;

    getSelectedPaths(selection);

    if (!selection.empty()) {
        dlg_repos_->setReposURL(getUrlOnline(selection[0]));
    }
	dlg_repos_->show();
}

void C_MainWindow::fileImport_Action_activated()
{
	if (dlg_import_->exec() == QDialog::Accepted) {
		svn_commands_->import(QDir::currentDirPath(),
                              dlg_import_->le_url->text(), dlg_import_->le_path->text(),
							  dlg_import_->te_msg->text(),
							  dlg_import_->ch_nonrecursive->isChecked());

	}
}

void C_MainWindow::fileExport_Action_activated()
{
	if (dlg_export_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_export_->rb_revision->isChecked())
			rev = (dlg_export_->le_revision->text());
		else
			rev	= "{" + dlg_export_->le_date->text() + "}";

		svn_commands_->export_cmd(QDir::currentDirPath(),
                                  dlg_export_->le_url->text(), dlg_export_->le_path->text(), rev);
	}
}


void C_MainWindow::fileWorkspaceAction_activated()
{
	dir_refresh_disable_ = false;

	dlg_workspace_->fillWsInfo();
	if (dlg_workspace_->exec() == QDialog::Accepted ) {
		QListViewItem *sel_item = dlg_workspace_->lv_wslist->selectedItem();
		if (sel_item && ws_.find(sel_item->text(0)) != ws_.end() )
			loadWS(sel_item->text(0));
	}
}


bool C_MainWindow::loadWS(const QString& ws_name)
{
	QStringList dirs = ws_[ws_name];

	if (dirs.size() ==0)
		return false;

	for (size_t i=0; i<dirs.size(); i++ ) {
        C_DirPath currDir = QDir(dirs[i]);
        dirs_list_->load(currDir, list_dirs, NULL, true, dlg_options_->ch_disablesl->isChecked(),  i>0 );
    }

	list_dirs->setSelected(list_dirs->firstChild(), true);

	QSettings settings_;
	settings_.setPath("umputun.com", "esvn", QSettings::User);
	settings_.beginGroup("/esvn");
	settings_.writeEntry("/state/active_workspace", ws_name);
	settings_.endGroup();

	QString recent_ws = ws_name + " [ws]";
	recent_files_.remove(recent_ws);
	recent_files_.push_front(recent_ws);

	updateRecentFileItems();
	viewRefresh_localAction_activated();

	setCaption(ws_name + " - " + baseCaption_);

	return true;
}

void C_MainWindow::fileShow_changed_filedirectoriesAction_activated()
{
    const C_DirPath *sel_path = dynamic_cast<C_DirPath*>(list_dirs->selectedItem());
    if (!sel_path) {
        return;
    }

    dlg_changed_->setFileDir(*sel_path);
	dlg_changed_->useExtDiff(dlg_options_->ch_extdiff->isChecked());
	dlg_changed_->bt_refresh_clicked();
	dlg_changed_->show();

	return;


	if (dlg_changed_->exec() == QDialog::Accepted) {
		QString selected_text = dlg_changed_->lv_status->currentItem()->text(0);

		//search changed item in dir panel
		QListViewItem *parent = list_dirs->currentItem();
		if (parent) {

			if (!parent->isOpen())
				list_dirs_doubleClicked(parent);

			QListViewItem *curr_item = parent->firstChild();
			do {
				//cout << EsvnLVI::getFullPath(item) << " " <<  curr_item->text(1) << endl;
                const C_Path* curr_path = dynamic_cast<const C_Path*>(curr_item);
				if (selected_text == curr_path->FullPath()) {
					emit list_files_selectionChanged(curr_item);
					list_dirs->setCurrentItem(curr_item);
					viewRefresh_localAction_activated();
					break;
				}
				curr_item = curr_item->nextSibling();
			} while (curr_item);
		}

		//search changed item in file panel
	}
}




//---------------------------------------------------------------------------
void C_MainWindow::helpAboutAction_activated() {
	dlg_about_->exec();
}

void C_MainWindow::helpIndexAction_activated()
{
	dlg_help_->tx_help->setSource("index.html");
	dlg_help_->show();
}

void C_MainWindow::helpWebsiteAction_activated()
{
	QProcess process(this);
	process.addArgument(dlg_options_->le_browser->text());
	process.addArgument("http://zoneit.free.fr");
	process.start();
}

void C_MainWindow::helpCheck_for_updatesAction_activated()
{
	http_.setHost("zoneit.free.fr");
	http_.get("/esvn/update.shtml");
}

void C_MainWindow::httpDone(bool error)
{
	if (error) {
		QMessageBox::warning(this, "HTTP request",
			tr("Error while fetching update.shtml: %1") .arg(http_.errorString()) );
		return;
	}

	QString response(http_.readAll());
	//textBrowser->append(response);
	QStringList elems = QStringList::split(";", response);
	if (elems.count() < 4 ) {
		QMessageBox::warning(this, "Update",
			"Wrong response: \n\n" + response);
	}
	else {
		QStringList ver_stable_elems = QStringList::split(".", elems[0]);
		QStringList ver_testing_elems = QStringList::split(".", elems[2]);
		QStringList ver_curr_elems = QStringList::split(".", QString(VERSION) );

		if (ver_stable_elems.size() < 3 || ver_testing_elems.size() < 3 || ver_curr_elems.size() < 3)
			return;

		unsigned long ver_stable  = ver_stable_elems[0].toLong()*10000 + ver_stable_elems[1].toLong()*100 + ver_stable_elems[2].toLong();
		unsigned long ver_testing = ver_testing_elems[0].toLong()*10000 + ver_testing_elems[1].toLong()*100 + ver_testing_elems[2].toLong();
		unsigned long ver_curr = ver_curr_elems[0].toLong()*10000 + ver_curr_elems[1].toLong()*100 + ver_curr_elems[2].toLong();

		dlg_verupdate_->lb_browser->setText(dlg_options_->le_browser->text());
		dlg_verupdate_->le_curr->setText(VERSION);
		dlg_verupdate_->le_stable->setText(elems[0]);
		dlg_verupdate_->le_testing->setText(elems[2]);
		dlg_verupdate_->lb_stable_url->setText(elems[1]);
		dlg_verupdate_->lb_testing_url->setText(elems[3]);

		dlg_verupdate_->bt_upd_stable->setEnabled(ver_stable > ver_curr);
		dlg_verupdate_->bt_upd_testing->setEnabled(ver_testing > ver_curr);
		dlg_verupdate_->exec();
	}

}

QStringList C_MainWindow::split(QString &str, char sep, bool is_trim)
{
    QStringList result;

    size_t separator_pos =0;
    while(separator_pos < str.length()) {
        int p = str.find(sep, separator_pos);
        if (p == -1)
            p = str.length();

        QString elem = str.mid(separator_pos, p-separator_pos); //exctract element
        if(is_trim) {
            rtrim(elem);
            ltrim(elem);
        }
        separator_pos = p+1;
        result.push_back(elem);
        if (p >= (int)str.length()) break;
    }

    return result;
}

void C_MainWindow::ltrim(QString& str) {
	size_t len = str.length();
	for (size_t i=0; i<len;  i++) {
		if (str[0] == ' ')
			str.remove(0,1);
		else
			break;
	}
}

void C_MainWindow::rtrim(QString& str) {
	int len = static_cast<int>(str.length());
	for (int i = len; i>=0; i--) {
		if (str[(int)str.length()-1] == ' ')
			str.remove(str.length()-1, 1);
		else
			break;
	}
}


void C_MainWindow::saveSettings()
{

	QSettings settings_;

	settings_.setPath("umputun.com", "esvn", QSettings::User);

	settings_.beginGroup("/esvn");
	settings_.writeEntry("/working_directory", workspace_.FullPath());

	QString str;
	QTextOStream out1(&str);
	out1 << *spl_dir_list;
	settings_.writeEntry("/geometry/spl_dir_list", str);
	QTextOStream out2(&str);
	out2 << *spl_list_out;

	settings_.writeEntry("/geometry/spl_list_out", str);
	settings_.writeEntry("/geometry/fields/1", list_files->columnWidth(0));
	settings_.writeEntry("/geometry/fields/2", list_files->columnWidth(1));
	settings_.writeEntry("/geometry/fields/3", list_files->columnWidth(2));
	settings_.writeEntry("/geometry/fields/4", list_files->columnWidth(3));
	settings_.writeEntry("/geometry/fields/5", list_files->columnWidth(4));
	settings_.writeEntry("/geometry/fields/6", list_files->columnWidth(5));
	settings_.writeEntry("/geometry/fields/7", list_files->columnWidth(6));

	QRect sz = QWidget::geometry();
	settings_.writeEntry("/geometry/main_height", sz.height());
	settings_.writeEntry("/geometry/main_width", sz.width());
	settings_.writeEntry("/geometry/main_top", sz.top());
	settings_.writeEntry("/geometry/main_left", sz.left());

	settings_.writeEntry("/progs/svn",  subversion_set_);
	settings_.writeEntry("/progs/edit", editor_set_);
	settings_.writeEntry("/progs/diff", diff_set_);
	settings_.writeEntry("/progs/diff3", dlg_options_->le_diff_3way->text());
	settings_.writeEntry("/progs/browser", dlg_options_->le_browser->text());

	settings_.writeEntry("/state/ext_diff", dlg_options_->ch_extdiff->isChecked());

	settings_.writeEntry("/state/dblclk_cat",  dlg_options_->rb_cat->isChecked());
	settings_.writeEntry("/state/dblclk_edit", dlg_options_->rb_extedit->isChecked());
	settings_.writeEntry("/state/dblclk_open", dlg_options_->rb_open->isChecked());

	settings_.writeEntry("/state/hide_unknown", hide_unknown_);
	settings_.writeEntry("/state/show_ch_paths", dlg_options_->ch_showchp->isChecked());
	settings_.writeEntry("/state/show_dir", dlg_options_->ch_showdir->isChecked());

	settings_.writeEntry("/state/hide_commit", dlg_options_->ch_hide_commit->isChecked());

	settings_.writeEntry("/state/disable_symlinks", dlg_options_->ch_disablesl->isChecked());
	settings_.writeEntry("/state/disable_svn_prop", dlg_options_->ch_disable_sprop->isChecked());
	settings_.writeEntry("/state/disable_commit_tag", dlg_options_->ch_disable_cotag->isChecked());

	settings_.writeEntry("/state/hide_log_filters", dlg_options_->ch_hide_filters->isChecked());

	settings_.writeEntry("/auth/user",  dlg_options_->le_user->text());
	settings_.writeEntry("/auth/passw", dlg_options_->le_passw->text());

	settings_.writeEntry("/refresh/online",   dlg_options_->rb_online->isChecked());
	settings_.writeEntry("/refresh/manual",   dlg_options_->rb_manual->isChecked());
	settings_.writeEntry("/refresh/interval", dlg_options_->sb_refresh->value() );

	settings_.writeEntry("/log/regular",   dlg_options_->rb_log_reg->isChecked());
	settings_.writeEntry("/log/verbouse",  dlg_options_->rb_log_verb->isChecked());


	QTextOStream out3(&str);
	out3 << *(dlg_repos_->spl_dir_list);
	settings_.writeEntry("/repos_geometry/spl_dir_list", str);
	settings_.writeEntry("/repos_geometry/fields/1", dlg_repos_->lv_files->columnWidth(0));
	settings_.writeEntry("/repos_geometry/fields/2", dlg_repos_->lv_files->columnWidth(1));
	settings_.writeEntry("/repos_geometry/fields/3", dlg_repos_->lv_files->columnWidth(2));
	settings_.writeEntry("/repos_geometry/fields/4", dlg_repos_->lv_files->columnWidth(3));
	settings_.writeEntry("/repos_geometry/fields/5", dlg_repos_->lv_files->columnWidth(4));


	QStringList ws_names;
	map<QString, QStringList>::iterator iter_ws;
	for (iter_ws = ws_.begin(); iter_ws != ws_.end(); iter_ws++)
		ws_names.push_back(iter_ws->first);

	settings_.writeEntry("/workspace/names", ws_names);

	for (size_t d=0; d<ws_names.size(); d++)
		if (ws_[ws_names[d]].size())
			settings_.writeEntry("/workspace/" + ws_names[d], ws_[ws_names[d]] );


	settings_.writeEntry("/openmap/items", open_map_);

	settings_.writeEntry("/recently/size", int(recent_files_.size()));
	for (size_t r = 0; r < recent_files_.size(); ++r) {
		char tmp[128];
		sprintf(tmp, "/recently/%d", (int) r);
		settings_.writeEntry(tmp, recent_files_[r]);
	}



	QStringList log_flt_msgs;
	for (int m=0; m<dlg_log_->cb_msg->count() && m < 32; m++)
		log_flt_msgs.push_back(dlg_log_->cb_msg->text(m));
	settings_.writeEntry("/filters/log/msg", log_flt_msgs);

	QStringList log_flt_auth;
	for (int a=0; a<dlg_log_->cb_author->count() && a < 32; a++)
		log_flt_msgs.push_back(dlg_log_->cb_author->text(a));
	settings_.writeEntry("/filters/log/auth", log_flt_msgs);


	//fonts
	settings_.writeEntry("/fonts/default",  dlg_options_->le_default->text());
	settings_.writeEntry("/fonts/view",     dlg_options_->le_view->text());
	settings_.writeEntry("/fonts/output",   dlg_options_->le_output->text());
	settings_.writeEntry("/fonts/list",     dlg_options_->le_list->text());
	settings_.writeEntry("/fonts/diff",     dlg_options_->le_diff_font->text());
	settings_.writeEntry("/fonts/cmd",      dlg_options_->le_cmd->text());


	//toolbars
	settings_.writeEntry("/state/tb_file/show",   viewToolbarsFileAction->isOn() );
	settings_.writeEntry("/state/tb_view/show",   viewToolbarsViewAction->isOn() );
	settings_.writeEntry("/state/tb_modify/show", viewToolbarsModifyAction->isOn() );
	settings_.writeEntry("/state/tb_query/show",  viewToolbarsQueryAction->isOn()) ;


	settings_.endGroup();
}

void C_MainWindow::loadSettings()
{

	QSettings settings_;
	settings_.setPath("umputun.com", "esvn", QSettings::User);
	settings_.beginGroup("/esvn");

	QRect win_size;
	win_size.setTop(settings_.readNumEntry("/geometry/main_top", 10));
	win_size.setLeft(settings_.readNumEntry("/geometry/main_left", 10));
	win_size.setWidth(settings_.readNumEntry("/geometry/main_width", 700));
	win_size.setHeight(settings_.readNumEntry("/geometry/main_height", 500));
	setGeometry(win_size);


	QString str1=settings_.readEntry("/geometry/spl_dir_list");
	QTextIStream in1(&str1);
	in1 >> *spl_dir_list;

	QString str2=settings_.readEntry("/geometry/spl_list_out");
	QTextIStream in2(&str2);
	in2 >> *spl_list_out;

	//updateGeometry();

	list_files->setColumnWidth(0, settings_.readNumEntry("/geometry/fields/1",100));
	list_files->setColumnWidth(1, settings_.readNumEntry("/geometry/fields/2",100));
	list_files->setColumnWidth(2, settings_.readNumEntry("/geometry/fields/3",100));
	list_files->setColumnWidth(3, settings_.readNumEntry("/geometry/fields/4",100));
	list_files->setColumnWidth(4, settings_.readNumEntry("/geometry/fields/5",100));
	list_files->setColumnWidth(5, settings_.readNumEntry("/geometry/fields/6",100));
	list_files->setColumnWidth(6, settings_.readNumEntry("/geometry/fields/7",100));


	QString str3=settings_.readEntry("/repos_geometry/spl_dir_list");
	QTextIStream in3(&str3);
	in3 >> *(dlg_repos_->spl_dir_list);

	//cout << "fff" << settings_.readNumEntry("/repos_geometry/fields/1", 80) << endl;
	dlg_repos_->lv_files->setColumnWidth(0, settings_.readNumEntry("/repos_geometry/fields/1", 80));
	dlg_repos_->lv_files->setColumnWidth(1, settings_.readNumEntry("/repos_geometry/fields/2", 40));
	dlg_repos_->lv_files->setColumnWidth(2, settings_.readNumEntry("/repos_geometry/fields/3", 40));
	dlg_repos_->lv_files->setColumnWidth(3, settings_.readNumEntry("/repos_geometry/fields/4", 40));
	dlg_repos_->lv_files->setColumnWidth(4, settings_.readNumEntry("/repos_geometry/fields/5", 40));


	QStringList ws_names = settings_.readListEntry("/workspace/names");
	for (size_t d=0; d<ws_names.size(); d++) {
		QStringList curr_ws = settings_.readListEntry("/workspace/" + ws_names[d]);
		if (curr_ws.size())
			ws_[ws_names[d]] = curr_ws;
	}

	QStringList open_map = settings_.readListEntry("/openmap/items");
	for (size_t m=0; m<open_map.size(); m++) {
		QStringList elems = QStringList::split("|", open_map[m]);
		if (elems.size() < 2)
			continue;
		QListViewItem *item = new QListViewItem(dlg_options_->lv_map, 0);
		item->setText(0, elems[0]);
		item->setText(1, elems[1]);
	}
	getOpenMap();


	QStringList log_flt_msgs = settings_.readListEntry("/filters/log/msg");
	for (size_t m=0; m<log_flt_msgs.size(); m++)
		dlg_log_->cb_msg->insertItem(log_flt_msgs[m]);
	dlg_log_->cb_msg->setCurrentText("");

	QStringList log_flt_auth = settings_.readListEntry("/filters/log/auth");
	for (size_t a=0; a<log_flt_msgs.size(); a++)
		dlg_log_->cb_author->insertItem(log_flt_auth[a]);
	dlg_log_->cb_author->setCurrentText("");


	subversion_set_ = settings_.readEntry("/progs/svn", "svn");
	svn_commands_->setSvn(subversion_set_);

	editor_set_ = settings_.readEntry("/progs/edit","kwrite");
	diff_set_ = settings_.readEntry("/progs/diff", "kdiff3" );
	dlg_options_->le_diff_3way->setText(settings_.readEntry("/progs/diff3", "kdiff3" ));
	dlg_options_->le_browser->setText(settings_.readEntry("/progs/browser", "mozilla" ));

	bool is_ext_dif = settings_.readBoolEntry("/state/ext_diff", false);
	dlg_options_->ch_extdiff->setChecked(is_ext_dif);
	dlg_diff_setup_->ch_extdiff->setChecked(is_ext_dif);

	dlg_options_->ch_extdiff->setChecked(settings_.readBoolEntry("/state/ext_diff", false));

	dlg_options_->rb_cat->setChecked(settings_.readBoolEntry("/state/dblclk_cat", false));
	dlg_options_->rb_extedit->setChecked(settings_.readBoolEntry("/state/dblclk_edit", true));
	dlg_options_->rb_open->setChecked(settings_.readBoolEntry("/state/dblclk_open", false));
	if (!dlg_options_->rb_cat->isChecked() && !dlg_options_->rb_extedit->isChecked() && !dlg_options_->rb_open->isChecked() )
		dlg_options_->rb_extedit->setChecked(true); //compatible with prev version


	hide_unknown_ = settings_.readBoolEntry("/state/hide_unknown", false);
    viewHide_unknownAction->setOn(hide_unknown_);

    //dlg_options_->ch_hide->setChecked(hide_unknown_);

	dlg_log_->gr_filters->setHidden(settings_.readBoolEntry("/state/hide_log_filters", false));


	dlg_log_->useExtDiff(dlg_options_->ch_extdiff->isChecked() );

	dlg_options_->le_user->setText(settings_.readEntry("/auth/user", ""));
	dlg_options_->le_passw->setText(settings_.readEntry("/auth/passw",""));

	dlg_options_->rb_online->setChecked(settings_.readBoolEntry("/refresh/online", false));
	dlg_options_->rb_manual->setChecked(settings_.readBoolEntry("/refresh/manual", true));

	dlg_options_->rb_log_reg->setChecked(settings_.readBoolEntry("/log/regular", true));
	dlg_options_->rb_log_verb->setChecked(settings_.readBoolEntry("/log/verbouse", false));


	dlg_options_->sb_refresh->setValue(settings_.readNumEntry("/refresh/interval", 30));

	dlg_options_->le_svn->setText(subversion_set_);
	dlg_options_->le_editor->setText(editor_set_);
	dlg_options_->le_diff->setText(diff_set_);

	dlg_options_->ch_showchp->setChecked(settings_.readBoolEntry("/state/show_ch_paths", false));
	dlg_options_->ch_showdir->setChecked(settings_.readBoolEntry("/state/show_dir", false));

	dlg_options_->ch_disablesl->setChecked(settings_.readBoolEntry("/state/disable_symlinks", true));
	dlg_options_->ch_disable_sprop->setChecked(settings_.readBoolEntry("/state/disable_svn_prop", false));
	dlg_options_->ch_disable_cotag->setChecked(settings_.readBoolEntry("/state/disable_commit_tag", false));

	dlg_options_->ch_hide_commit->setChecked(settings_.readBoolEntry("/state/hide_commit", false));

	int rs_sz = settings_.readNumEntry("/recently/size", 0);
	for (int r = 0; r < rs_sz; ++r) {
		char tmp[128];
		sprintf(tmp, "/recently/%d", rs_sz-r-1);
		QString path = settings_.readEntry(tmp, "noname");
		recent_files_.remove(path);
		recent_files_.push_front(path);
		updateRecentFileItems();
	}


	//fonts
	dlg_options_->le_default->setText(settings_.readEntry("/fonts/default",""));
	dlg_options_->le_view->setText(settings_.readEntry("/fonts/view",""));
	dlg_options_->le_output->setText(settings_.readEntry("/fonts/output",""));
	dlg_options_->le_list->setText(settings_.readEntry("/fonts/list",""));
	dlg_options_->le_diff_font->setText(settings_.readEntry("/fonts/diff",""));
	dlg_options_->le_cmd->setText(settings_.readEntry("/fonts/cmd",""));


	//toolbars
	viewToolbarsFileAction->setOn(settings_.readBoolEntry("/state/tb_file/show", false));
	viewToolbarsViewAction->setOn(settings_.readBoolEntry("/state/tb_view/show", false));
	viewToolbarsModifyAction->setOn(settings_.readBoolEntry("/state/tb_modify/show", false));
	viewToolbarsQueryAction->setOn(settings_.readBoolEntry("/state/tb_query/show", false));

	showHideToolBars();

	//dlg_busy busy(this);
	//busy.lb_command->setText("Loading workspace " + settings_.readEntry("/working_directory", ""));
	//busy.show();


	QString ws_name = settings_.readEntry("/state/active_workspace", "");
	if (ws_name.isEmpty()) {
		QString working_directory = settings_.readEntry("/working_directory", "");

		if (!working_directory.isEmpty()) {
			C_DirPath wsPath = QDir(working_directory);
			loadWorkspace(wsPath);
		}
	}
	else {
		dir_refresh_disable_ = false;
		loadWS(ws_name);
	}


	//busy.hide();

	user_ = dlg_options_->le_user->text();
	passwd_ = dlg_options_->le_passw->text();

	dlg_prop_->setParams(svn_commands_, dlg_options_->ch_disable_sprop->isChecked());
	dlg_workspace_->wsData(&ws_);

	setFonts();
	settings_.endGroup();

}


void C_MainWindow::setFonts() {
	QFont font;

	if (dlg_options_->le_default->text() != "") {
		font.fromString(dlg_options_->le_default->text());
		setFont(font);
	}

	if (dlg_options_->le_view->text() != "") {
		font.fromString(dlg_options_->le_view->text());
		dlg_view_->tb_view->setFont(font);
	}

	if (dlg_options_->le_output->text() != "") {
		font.fromString(dlg_options_->le_output->text());
		textBrowser->setFont(font);
	}

	if (dlg_options_->le_list->text() != "") {
		font.fromString(dlg_options_->le_list->text());
		list_files->setFont(font);
		list_dirs->setFont(font);
	}

	if (dlg_options_->le_diff_font->text() != "") {
		font.fromString(dlg_options_->le_diff_font->text());
		dlg_diff_->tx_diff->setFont(font);
	}

	if (dlg_options_->le_cmd->text() != "") {
		font.fromString(dlg_options_->le_cmd->text());
		cmd_LineEdit->setFont(font);
	}
}


QString C_MainWindow::getUrlOnline(const C_Path& localpath) {

	QStringList info;
    PathSelection selection;
    selection.push_back(localpath);
	int res = svn_commands_->info(selection, info);

	QString url = "";
	if (res == 0) {
		//find URL QString
		for (size_t i=0; i<info.size(); i++) {
			QString str = info[i];
			if (str.find("URL:") == 0 && str.length() > 4 ) {
				url = str.mid(5, 1024);
				break;
			}
		}
	}
	return url;
}


bool C_MainWindow::fillSvnStatus(C_SvnStatus* res_svn_status)
{

	if (dir_refresh_disable_) {
		sendToOutput("get status - refresh disabled");
		return false;
	}
	sendToOutput("get status");

	C_SvnCommands svn_command(textBrowser, this);
	svn_command.setUserPassw(user_, passwd_);
	svn_command.setSvn(subversion_set_);

	C_SvnStatus svn_status(&svn_command);

	QStringList list_info;
    const C_Path* sel_path = dynamic_cast<const C_Path*>(list_dirs->selectedItem());

    if (!sel_path || sel_path->FullPath().isEmpty()) {
        sendToOutput("get status - no current directory");
        return false;
    }
	//return res_svn_status->status("");

	if (!svn_status.status(sel_path->FullPath()) ) //true = use curr dir
		return false;

	res_svn_status->setData(&svn_status);


	return true;
}

int C_MainWindow::getSelectedFiles(vector<QString> &selected_files)
{
	selected_files.clear();

    PathSelection selection;
    getSelectedFiles(selection);

    for (PathSelection::const_iterator it = selection.begin(); it != selection.end(); ++it)
    {
        const C_Path& path = *it;
        selected_files.push_back(path.Name());
    }

	return selected_files.size();
}

int getSelection(PathSelection& selection, QListView& view)
{
	selection.clear();

    QListViewItemIterator iter(&view, QListViewItemIterator::Selected|QListViewItemIterator::Visible);

    while ( iter.current() ) {
        //QString fname = EsvnLVI::getFullPath(iter.current());
        //cout << "sel: " << fname <<  endl;
        const C_Path* path = dynamic_cast<const C_Path*>(iter.current());
        if (path) {
            //cout << "sel dir: " << path->Path() << " name: " << path->Name() <<  endl;
            selection.push_back(*path);
        } else {
            cout << "Warning: no conversion from list view item to path object available" << endl;
        }
        ++iter;
    }

	return selection.size();
}

int C_MainWindow::getSelectedPaths(PathSelection& selection)
{
	selection.clear();

    // Get current view
    QListView* active_view = NULL;

#if 1
    active_view = is_selected_dir_?list_dirs:list_files;
#else
    if (list_files->currentItem()) {
        active_view = list_files;
    } else
    if (list_dirs->currentItem()) {
        active_view = list_dirs;
    }
#endif

    if (!active_view) {
        // No selection
        std::cout << "getSelectedPaths: no current view" << std::endl;
        return 0;
    }

    return getSelection(selection, *active_view);
}

int C_MainWindow::getSelectedFiles(PathSelection& selection)
{
    return getSelection(selection, *list_files);
}

void C_MainWindow::getFilesAndSize(const C_Path& path, vector<QString>& files_and_size)
{
	files_and_size.clear();

	if (path.FullPath().isEmpty()) {
		sendToOutput("get files info: invalid path");
		return;
	} else {
		sendToOutput("get files info: " + path.FullPath());
	}

	QDir files = QDir(path.FullPath());
	for (size_t i=0; i<files.count(); i++) {
		if (files[i] != "." && files[i] != "..") {
            C_RelativePath curr_entry(path, files[i]);
            QFileInfo fi( curr_entry.FullPath() );
			if( fi.exists() ) {
				QString f_s = curr_entry.FullPath() + QString("_") + fi.lastModified().toString();
				files_and_size.push_back(f_s);
			}
		}
	}
}

void C_MainWindow::showHideToolBars()
{

	if (viewToolbarsFileAction->isOn() ) 	tlb_file->show();
	else									tlb_file->hide();

	if (viewToolbarsViewAction->isOn() ) 	tlb_view->show();
	else									tlb_view->hide();

	if (viewToolbarsModifyAction->isOn() ) 	tlb_modify->show();
	else									tlb_modify->hide();

	if (viewToolbarsQueryAction->isOn() ) 	tlb_query->show();
	else									tlb_query->hide();
}


void C_MainWindow::getOpenMap()
{
	if (dlg_options_->lv_map->childCount() == 0)
		return;

	open_map_.clear();

	QListViewItem *item = dlg_options_->lv_map->firstChild();
	do {
		QString str = item->text(0) + "|" + item->text(1);
		open_map_.push_back(str);
		item = item->itemBelow();
	} while (item);

}

void C_MainWindow::setSvnDir(const QString& dir)
{
	svn_dir_ = dir;
	dirs_list_->setSvnDir(svn_dir_);
	current_file_list_.setSvnDir(svn_dir_);
    dlg_commit_->setSvnDir(svn_dir_);
}

void C_MainWindow::sendToOutput(const QString& str)
{
	if (C_Options::instance()->dlgOptions()->rb_log_verb->isChecked() )
		textBrowser->append( QString("<font color=grey>") + str + QString("</font>") );
}

void C_MainWindow::viewSwitchFlatView_toggled(bool show_flat_view)
{
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    show_flat_view_ = show_flat_view;
    refreshList();
    QApplication::restoreOverrideCursor();
}

void C_MainWindow::viewSwitchFlatViewChanged_toggled(bool show_changed)
{
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    show_changed_items_only_ = show_changed;
    refreshList();
    QApplication::restoreOverrideCursor();
}

namespace EsvnLVI
{
QListViewItem* getFirstSelectedItem(QListView *view)
{
    if (!view) {
        return NULL;
    }

    QListViewItemIterator iter(view, QListViewItemIterator::Selected|QListViewItemIterator::Visible);

    return iter.current();
}

} // end of namespace EsvnLVI

