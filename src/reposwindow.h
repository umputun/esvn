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

#ifndef REPOSITORY_WINDOW_H
#define REPOSITORY_WINDOW_H


#include <qlistview.h> 
#include <qheader.h> 
#include <qlineedit.h> 
#include <qcheckbox.h>
#include <qradiobutton.h>


#include <map>
using namespace std;

#include "dlg_repository.h"
#include "svn_commands.h"
#include "dlg_checkout.h"
#include "dlg_view.h"


struct S_DirInfo {
	QStringList files;
	QListViewItem *item;
  bool loaded;

  S_DirInfo() : item(0), loaded(false)  { }
};

class C_ReposWindow : public dlg_repository {

public:
	C_ReposWindow(QWidget* parent = 0, const char* name = 0) 
	: dlg_repository(parent, name), svn_command_(NULL), dlg_view_(NULL), dlg_checkout_(NULL){
		lv_dirs->header()->hide();

		lv_files->setColumnAlignment(1, Qt::AlignRight);
		lv_files->setColumnAlignment(2, Qt::AlignRight);
		lv_files->setColumnAlignment(3, Qt::AlignRight);
		lv_files->setColumnAlignment(4, Qt::AlignHCenter);

		lv_files->setColumnWidthMode(0, QListView::Manual);
		lv_files->setColumnWidthMode(1, QListView::Manual);
		lv_files->setColumnWidthMode(2, QListView::Manual);
		lv_files->setColumnWidthMode(3, QListView::Manual);
		lv_files->setColumnWidthMode(4, QListView::Manual);

	}

	void setParams(C_SvnCommands* svn_command, dlg_view *view, dlg_checkout *chout) {
		svn_command_ = svn_command;
		dlg_view_ = view;
		dlg_checkout_ = chout;
	}

    void setReposURL(const QString& url) {
        le_url->setText(url);
    }

public:
	void bt_browse_clicked();
	void lv_dirs_selectionChanged( QListViewItem* item);
	void bt_view_clicked();
	void bt_checkout_clicked();

private:
	QListViewItem* findParentItem(const QString& dir);
	QString getDirectoryFileName(const QString& path);
	QString getFileNameFromPath(const QString& path);
	QString getCurrentLocationUrl(bool dir_only=false);
  void load(QListViewItem *item);
  QString getSubPath(QListViewItem *item, QString res = "");
  
private:

	C_SvnCommands* svn_command_;
	dlg_view *dlg_view_;
	dlg_checkout *dlg_checkout_;

	map<QString, S_DirInfo> repository_;
};

class C_NumSortedListItem : public QListViewItem {
public:
	C_NumSortedListItem( QListView * parent, QListViewItem * after ) :
		QListViewItem(parent, after) {}

	int compare(QListViewItem *i, int col, bool ascending ) const {
		if (col == 1 || col == 3) {
			if (text(col).toULong()  == i->text(col).toULong() )	
				return 0;

			//if (ascending)	
				return (text(col).toULong()  > i->text(col).toULong()) ? 1 : -1; 
			//else    		
				//return (text(col).toULong()  < i->text(col).toULong()) ? 1 : -1; 
		}
		return QListViewItem::compare(i, col, ascending);
	}
};

#endif
