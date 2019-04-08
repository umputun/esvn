
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

#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include "dlg_diff_setup.h"
#include "svn_commands.h"
#include "dlg_view.h"
#include "logwindow.h"

class C_DiffWindow : public dlg_diff_setup {

public:
    C_DiffWindow(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0) 
    : dlg_diff_setup(parent, name, modal, fl), svn_command_(NULL), dlg_view_(NULL) {

    }

    void setParams(C_SvnCommands* svn_command, dlg_view *view) {
        svn_command_ = svn_command;
        dlg_view_ = view;
    }
    void setFileDir(const C_Path& path) {
        file_dir_ = path;
    }

    void bt_rev_wizard_clicked() {
        C_LogWindow dlg_log_(this);
        dlg_log_.setParams(svn_command_,dlg_view_, 0, true);
        dlg_log_.setFileDir(file_dir_);
        dlg_log_.load(false);

        if (dlg_log_.exec()== QDialog::Accepted) 
            le_revision->setText(dlg_log_.getSelectedRevision());
    }


private:
    C_SvnCommands* svn_command_;
    dlg_view *dlg_view_;
    C_PathCopy file_dir_;
};

#endif
