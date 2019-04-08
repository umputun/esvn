/***************************************************************************
 *   Copyright (C) 2004 by Alexander Darovsky                              *
 *   adarovsky@users.sf.net                                                *
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
#ifndef C_COMMITWINDOW_H
#define C_COMMITWINDOW_H
#include "dlg_commit.h"
#include "esvn_utils.h"

#include <vector>

class C_SvnStatus;
class C_CommitWindow : public dlg_commit
{
    Q_OBJECT

public:
    C_CommitWindow( C_SvnStatus* status, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~C_CommitWindow();

public slots:
    void setSvnDir(const QString& svn_dir);
    void setCommitPath( const C_DirPath& path, bool commit_list_enabled = true);
    void setCommitFiles( const std::vector<QString>& files );
    void selectAll();
    void deselectAll();

    std::vector<QString> commitList() const;
private:
    C_SvnStatus* status_;
	QString svn_dir_;

private slots:
    void itemToggled( QListBoxItem* );
    void itemClicked( QListBoxItem*, const QPoint& );
};

#endif // C_COMMITWINDOW_H
