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

#ifndef PROP_WINDOW_H
#define PROP_WINDOW_H

#include <map>
#include <qmessagebox.h>

#include "dlg_prop.h"
#include "dlg_propedit.h"

#include "svn_commands.h"
using namespace std;

class C_PropWindow : public dlg_prop {
public:
	C_PropWindow(QWidget* parent = 0, const char* name = 0) 
	: dlg_prop(parent, name), selected_(NULL) {}

	void setParams(C_SvnCommands* svn_command, bool disable_change_svn_prop) {
		svn_command_ = svn_command;
		disable_change_svn_prop_ = disable_change_svn_prop;
	}

	void setFileDirName(const C_Path& filename) {
		file_dir_name_ = filename;
	}

	void bt_add_clicked() {
		dlg_propedit pedit;
		pedit.setCaption("Add Property");
		if (pedit.exec() == QDialog::Accepted) {
			if (disable_change_svn_prop_ && pedit.le_key->text().find("svn:") == 0) {
				QMessageBox::information(this, "Action rejected", "Can not change svn: property"); 
				return;
			}
			svn_command_->propset(file_dir_name_, "", pedit.le_key->text(), pedit.le_value->text());
			refresh();
		}
	}
	void bt_edit_clicked() {
		dlg_propedit pedit;
		pedit.setCaption("Edit Property");

		if (!selected_) 
			return;

		pedit.le_key->setText(selected_->text(0));
		pedit.le_value->setText(tx_full->text());

		if (pedit.exec() == QDialog::Accepted) {

			if (disable_change_svn_prop_ && pedit.le_key->text().find("svn:") == 0) {
				QMessageBox::information(this, "Action rejected", "Can not change svn: property"); 
				return;
			}

			svn_command_->propset(file_dir_name_, "", pedit.le_key->text(), pedit.le_value->text());
			refresh();
		}
	}
	void bt_delete_clicked() {
		if (!selected_) 
			return;

		QMessageBox mb( "Confirmation",
						"You are about to delete the following property:\n\n" + selected_->text(0) + "\n",
						QMessageBox::Question,
						QMessageBox::Yes | QMessageBox::Default,
						QMessageBox::No  | QMessageBox::Escape,
						QMessageBox::NoButton );

		if ( mb.exec() == QMessageBox::Yes ) {
			if (disable_change_svn_prop_ && selected_->text(0).find("svn:") == 0) {
				QMessageBox::information(this, "Action rejected", "Can not change svn: property"); 
				return;
			}
			svn_command_->propdel(file_dir_name_, "", selected_->text(0));
		}

		refresh();
	}

	void lv_prop_selectionChanged(QListViewItem *item) {
		selected_ = item;
		tx_full->clear();
		if (prop_list_.find(item->text(0)) != prop_list_.end()) {
			QStringList value = prop_list_[item->text(0)];
			for (size_t i=0; i<value.size(); i++) 
				tx_full->append(value[i]);
		}

	}

	void bt_reload_clicked() {
		refresh();
	}

public:
	void refresh() {
		lv_prop->clear();
		tx_full->clear();

		QStringList prop_list;
		int res = svn_command_->proplist(file_dir_name_, "", prop_list);
		if (res == 0 && parsePropList(prop_list)) {
			map<QString, QStringList>::iterator iter_props;
			for (iter_props = prop_list_.begin(); iter_props != prop_list_.end(); iter_props++) {
				QListViewItem *item = new QListViewItem(lv_prop, 0);
				item->setText(0, iter_props->first);
				QStringList value = iter_props->second;
				if (value.size() > 0 ) 
					item->setText(1, value[0]);
			}
		}
	}
private:
	bool parsePropList(const QStringList& props) {
		bool is_value = false;

		QString key = "";
		QStringList value;
		prop_list_.clear();

		for (size_t i=0; i<props.size(); i++) {
			QString curr = props[i];

			if (curr.find("Properties on ") != -1) {
				value.clear();
				is_value = false;
				continue;
			}

			if (curr.find(" : ") != -1) {

				if (!key.isEmpty()) 
					prop_list_[key] = value;

				value.clear();

				is_value = true;
				QStringList elems;
				elems = QStringList::split(" : ", curr);
				if (elems.size() < 2) {
					cerr << "wrong properties string: " << curr << " " << elems.size() << endl;
					continue;
				}
				key = elems[0];
				key.remove(0,2);
				value.push_back(elems[1]);
				///cout << "prop pair " << key << " " << value << endl;

				continue;
			}

			if (is_value) 
				value.push_back(curr);
			else
				cerr << "martian properties string: " <<  curr << endl;
		}

		if (!key.isEmpty()) 
			prop_list_[key] = value;

		return true;
	}

private:
	C_SvnCommands* svn_command_;
    C_PathCopy file_dir_name_;
	map<QString, QStringList> prop_list_;
	QListViewItem *selected_;
	bool disable_change_svn_prop_;
};

#endif
