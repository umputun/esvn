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

#ifndef WORKSPACE_WINDOW_H
#define WORKSPACE_WINDOW_H

#include <qlistview.h> 
#include <qheader.h> 

#include <map>
using namespace std;

#include "dlg_workspace.h"
#include "dlg_ws_edit.h"

class C_WsWindow : public dlg_workspace {

public:
	C_WsWindow(QWidget* parent = 0, const char* name = 0) :
		dlg_workspace(parent, name) {}

	void wsData(map<QString, QStringList>* ws_data) {
		ws_data_ = ws_data;
	}

	void fillWsInfo() {
		lv_wslist->clear();
		tx_full->clear();
		map<QString, QStringList>::iterator iter_ws;
		for (iter_ws = ws_data_->begin(); iter_ws != ws_data_->end(); ++iter_ws) {
			QListViewItem *item = new QListViewItem(lv_wslist, 0);
			item->setText(0, iter_ws->first);
			QStringList dirs = iter_ws->second;
			QString long_str;
			for (size_t i=0; i<dirs.size(); i++) 
				long_str = long_str + dirs[i] + "; ";
			item->setText(1, long_str);
		}
		lv_wslist->setSelected(lv_wslist->firstChild(), TRUE);
	}


	void bt_add_clicked() {
		bool ok;
		QString text = QInputDialog::getText("New workspace",
				"Enter workspace name:", QLineEdit::Normal,
				QString::null, &ok, this );

		if ( ok && !text.isEmpty() ) {
			dlg_ws_edit ws_edit;
			ws_edit.setCaption(text);
			ws_edit.lv_dirs->header()->hide();
			if (ws_edit.exec() == QDialog::Accepted) {
				QListViewItem *item = ws_edit.lv_dirs->firstChild();
				if (item) {
					QStringList dirs;			
					while(item) {
						dirs.push_back(item->text(0));
						item = item->itemBelow();
					}
					(*ws_data_)[text] = dirs;
				}
				fillWsInfo();
			}
		}
		
	}

	void bt_edit_clicked() {
		QListViewItem* item = lv_wslist->selectedItem();
		if (item) {
			dlg_ws_edit ws_edit;
			QListViewItem* item = lv_wslist->selectedItem();
			ws_edit.setCaption(item->text(0));
			ws_edit.lv_dirs->header()->hide();
			QStringList dirs_list = QStringList::split("; ", item->text(1) );
			for (size_t i=0; i<dirs_list.size(); i++) {
				QListViewItem* dir_item = new QListViewItem(ws_edit.lv_dirs);
				dir_item->setText(0, dirs_list[i]);
			}

			if (ws_edit.exec() == QDialog::Accepted) {
				QListViewItem *dir_item = ws_edit.lv_dirs->firstChild();
				if (item) {
					QStringList dirs;			
					while(dir_item) {
						dirs.push_back(dir_item->text(0));
						dir_item = dir_item->itemBelow();
					}
					(*ws_data_)[item->text(0)] = dirs;
				}
				fillWsInfo();
			}
		}
	}


	void bt_delete_clicked() {
		QListViewItem* item = lv_wslist->selectedItem();
		if (item) {
			ws_data_->erase(ws_data_->find(item->text(0)));
			fillWsInfo();
		}
	}

	void bt_rename_clicked() {
		QListViewItem* item = lv_wslist->selectedItem();
		if (item) {
			bool ok;
			QString text = QInputDialog::getText("Rename workspace " + item->text(0),
					"Enter workspace name:", QLineEdit::Normal,
					QString::null, &ok, this );

			if ( ok && !text.isEmpty() ) {
				map<QString, QStringList>::iterator iter_ws = ws_data_->find(item->text(0));
				if (iter_ws != ws_data_->end()) {
					ws_data_->insert(map<QString, QStringList>::value_type(text, iter_ws->second));
					ws_data_->erase(iter_ws);
				}
			}

			fillWsInfo();
		}
	}

private:
	map<QString, QStringList>* ws_data_;
};

#endif


