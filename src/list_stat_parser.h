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

#ifndef LIST_STAT_PARSER
#define LIST_STAT_PARSER

#include <qlistview.h> 
#include <qstring.h> 
#include <qfileinfo.h>
#include <qapplication.h>
#include <qcursor.h> 

#include <map>
#include <vector>
#include <algorithm>

using namespace std;

#include "svn_commands.h"


#define ROOT_LS_ID "ROOT_LS "

struct S_LSDirInfo {
	QStringList files;
	QListViewItem *item;
};


class C_LSParser {
public:
	enum E_LSTYPE {LSP_LIST, LSP_STATUS} ;

	C_LSParser(E_LSTYPE type, C_SvnCommands* svn_command, QListView* lv_dirs) 
		: type_(type), svn_command_(svn_command), lv_dirs_(lv_dirs) {}

	bool fillDirsInfo(const QString& dir);
	bool fillLocalDirsInfo(const QString& dir);
	

protected:

	map<QString, S_LSDirInfo> dirs_info_;
	E_LSTYPE type_;

private:

	bool getInfo(const QString& dir, vector<QString>& info);

	QListViewItem* findParentItem(const QString& dir);
	QString getDirectoryFileName(const QString& path);
	QString getFileNameFromPath(const QString& path);
//	QString getCurrentLocationUrl();
	bool isDirectory(const QString& name);
	int getLevel(const QString& name); 

private:
	C_SvnCommands* svn_command_;
	QListView* lv_dirs_;
};


struct status_cmp
{                                                                                
	bool operator()(const QString& x, const QString& y) const {
		QStringList elems_x = QStringList::split(" ", x);
		QStringList elems_y = QStringList::split(" ", y);
		return elems_x[elems_x.size()-1] < elems_y[elems_y.size()-1]; 
	}
};

#endif



