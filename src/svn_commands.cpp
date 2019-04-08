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

#include "svn_commands.h"


// Diff command ---------------------------------------------------------------------------------------

int C_SvnCommands::diff(const QString& currwd,
                        const QString& file_dir_name_1, const QString& file_dir_name_2, 
                        const QString& rev, bool use_external_cmd, bool non_recursive, bool notice_ancestry, 
                        const QString& extensions,  QStringList& out_txt) 
{
    proc->clearArguments();
    proc->addArgument(svn_); proc->addArgument("diff");
    if (rev != "") {
        proc->addArgument("-r");
        proc->addArgument(rev);
    }

    if (!file_dir_name_1.isEmpty()) {
		if (!file_dir_name_2.isEmpty()) {
			proc->addArgument("--old");
			proc->addArgument(QDir::convertSeparators(file_dir_name_1));
			proc->addArgument("--new");
			proc->addArgument(QDir::convertSeparators(file_dir_name_2));
		} else {
			// Simple syntax
			proc->addArgument(QDir::convertSeparators(file_dir_name_1));
		}
	}
    


    if (non_recursive) 
        proc->addArgument("--non-recursive");

    if (notice_ancestry) 
        proc->addArgument("--notice-ancestry");


    if (!extensions.isEmpty()) {
        proc->addArgument("--extensions");
        proc->addArgument(extensions);      
    }


    if (use_external_cmd) {
        proc->addArgument("--diff-cmd");
#ifdef Q_OS_WIN
        proc->addArgument(QDir::convertSeparators(qApp->applicationDirPath() + "/esvn-diff-wrapper"));
#else
        proc->addArgument("esvn-diff-wrapper");     
#endif
        return execCommand(currwd, false);
    }
    else    
        return execCommand(currwd, SC_APPEND_USER_PASS, &out_txt);

}

