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

#ifndef MERGE_WINDOW_H
#define MERGE_WINDOW_H

#include "dlg_merge.h"
#include "svn_commands.h"
#include "logwindow.h"
#include "dlg_view.h"

#include "dlg_url.h"

class C_MergeWindow : public dlg_merge {

public:
	C_MergeWindow(QWidget* parent = 0, const char* name = 0) :
		dlg_merge(parent, name) {}

	void setParams(C_SvnCommands* svn_command,  dlg_view *view) {
		svn_command_ = svn_command;
		dlg_view_ = view;
	}

	void setFileDirUrl(const C_Path& pathname, const QString& url) {
		file_dir_ = pathname;
		url_ = url;
	}

	void bt_rev_wizard_clicked() {
		C_LogWindow dlg_log_(this);
		dlg_log_.setParams(svn_command_,dlg_view_, 0, true);
		dlg_log_.setFileDir(file_dir_);
		dlg_log_.load(false);

		if (dlg_log_.exec()== QDialog::Accepted) 
			le_rev1->setText(dlg_log_.getSelectedRevision());
	}
	void bt_rev_wizard_2_clicked() {
		C_LogWindow dlg_log_(this);
		dlg_log_.setParams(svn_command_,dlg_view_, 0, true);
		dlg_log_.setFileDir(file_dir_);
		dlg_log_.load(false);

		if (dlg_log_.exec()== QDialog::Accepted) 
			le_rev2->setText(dlg_log_.getSelectedRevision());
	}

	void bt_uw2_clicked() {
		dlg_url url;
		url.le_url_->setText(url_);
		if (url.exec() == QDialog::Accepted)
			le_file->setText(url.le_url_->text());
	}

	void bt_url1_clicked() {
		dlg_url url;

		if (le_url1->text().isEmpty()) 
			url.le_url_->setText(url_);
		else
			url.le_url_->setText(le_url1->text());

		if (url.exec() == QDialog::Accepted)
			le_url1->setText(url.le_url_->text());
	}

	void bt_url2_clicked() {
		dlg_url url;

		if (le_url2->text().isEmpty()) 
			url.le_url_->setText(url_);
		else
			url.le_url_->setText(le_url2->text());

		if (url.exec() == QDialog::Accepted)
			le_url2->setText(url.le_url_->text());
	}

    // post-process file selection change while browsing
    // as it affects current working directory
    /*
    void bt_dirs_clicked() {
        dlg_merge::bt_dirs_clicked();
        if (le_file->text()) {
            QString selected_path = QDir::cleanDirPath(le_file->text())
            if (file_dir_.FullPath() != selected_path) {
                file_dir_ = C_RelativePath(C_DirPath(selected_path), le_file->text());
            }
        }
    }
    */

private:
	C_SvnCommands* svn_command_;
	dlg_view *dlg_view_;
	QString url_ ;
    C_PathCopy file_dir_;
};

#endif

