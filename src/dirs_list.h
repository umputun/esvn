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

#ifndef DIRS_LIST_H
#define DIRS_LIST_H

#include <qobject.h>
#include <qlistview.h>
#include <qdir.h>
#include <qtimer.h> 
#include <qpixmap.h> 
#include <qstring.h>
#include <qapplication.h> 
#include <qfileinfo.h>
#include <qeventloop.h>


#include <string>
#include <iostream>

#include "entries_parser.h"
#include "svn_status.h"


using namespace std;

class C_DirViewItem;

class C_DirsList : public QWidget {

	Q_OBJECT

public:
	C_DirsList(const QString& svn_dir) : hideUnknown_(false), svn_status_(NULL), svn_dir_(svn_dir) {}

	void setSvnDir(const QString& svn_dir) {
		svn_dir_ = svn_dir;
	}

	bool load(const C_Path& path, QListView* list_dirs, C_DirViewItem* root, bool open=true, bool disable_symlinks=true, bool append=false);

	void setSvnStatus(C_SvnStatus* svn_status) {
		svn_status_ = svn_status;
	}

	static QString removeLastLevel(const QString& full_file_name) {
		int pos = full_file_name.findRev('/');

		if (pos == -1)
			return full_file_name;
		else
			return full_file_name.left(pos);
	}

	void setHide(bool hide) {
		hideUnknown_ = hide;
	}

public slots:



private:
	bool isSvnControlled(const QString& dir) {
		QFileInfo fi(dir + "/"+svn_dir_+"/entries");
		return (fi.exists());
	}

	bool hasSubDirs(const QString &dir, bool disable_symlinks) {
		QDir curr_dir =  QDir(dir);
		curr_dir.setFilter(QDir::Dirs | QDir::Hidden | (disable_symlinks ? QDir::NoSymLinks : 0) );
		curr_dir.setPath(dir);

		if (!curr_dir.count()) 
			return false;

		for (size_t i=0; i<curr_dir.count(); i++) {
			if (curr_dir[i] != "." && curr_dir[i] != ".." && curr_dir[i] != svn_dir_)
				return true;
		}
		return false;

	}

	QPixmap getIcon(const QString& dir_name, bool is_open = false, C_SvnFile *svn_file_info = NULL);

	bool hideUnknown_;
	C_SvnStatus* svn_status_;
	QString svn_dir_;
};

class C_FilesList {
public:
	C_FilesList(const QString& svn_dir=".svn") : svn_dir_(svn_dir) {
	}

	void setSvnDir(const QString& svn_dir) {
		svn_dir_ = svn_dir;
	}

    // Load filesystem items from specified path into view list_files
    // If flat_view is true, load items recursively
    // and store their path names relative to path.
    bool load(const C_Path& path, QListView* list_files, bool hide_unknown, bool flat_view, bool show_changed_items_only, bool show_dirs, C_SvnStatus* svn_status_);

protected:
    // Load filesystem items from base_path/rel_path_prefix into view list_files.
    // On top level (when called from load() function) rel_path_prefix should be empty string.
    // If flat_view is true then this function recurse into subdirs.
    // When recursion is used, base_path is passed down without modification,
    // and rel_path_prefix vary to specify next subdirectory.
    bool load_real(const QString& rel_path_prefix, const C_Path& base_path, QListView* list_files, bool hide_unknown, bool flat_view, bool show_changed_items_only, bool show_dirs, C_SvnStatus* svn_status_);

private:
	QString svn_dir_;
	C_SvnStatus* svn_status_;
};

class C_DirViewItem : public QListViewItem, public C_DirPath
{
public:
    C_DirViewItem(QListView *parent, QListViewItem *after = NULL);
    C_DirViewItem(QListViewItem *parent, QListViewItem *after = NULL);

    virtual ~C_DirViewItem();

public:
    // Reimplement custom text values
    virtual QString text(int) const;
};

class C_NumSortedFileListItem : public QListViewItem, public C_RelativePath {
public:
	C_NumSortedFileListItem( const C_Path& parent_path, const QString& name, QListView * parent, QListViewItem * after ) :
		QListViewItem(parent, after) , C_RelativePath(parent_path, name) {}

	int compare(QListViewItem *i, int col, bool ascending ) const {
		if (col == 2 || col == 3) {
			if (text(col).toULong()  == i->text(col).toULong() )	
				return 0;
			return (text(col).toULong()  > i->text(col).toULong()) ? 1 : -1; 
		}
		if (col == 0) {
            C_NumSortedFileListItem* item = dynamic_cast<C_NumSortedFileListItem*>(i);
            if (item) {
                if (isDir() && !item->isDir()) 
                    return -1;

                if (!isDir() && item->isDir()) 
                    return 1;
            }
		}

		return QListViewItem::compare(i, col, ascending);
	}

public:
    // Reimplement custom text values
    virtual QString text(int) const;

private:
	bool is_dir_;
};

#endif


