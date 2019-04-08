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

#include "list_stat_parser.h"



bool C_LSParser::fillDirsInfo(const QString& dir)
{
	vector<QString> info;


	if (getInfo(dir, info)) {

		lv_dirs_->clear();
		dirs_info_.clear();

		QString curr_dir;
		S_LSDirInfo curr_dir_info;

		QListViewItem *parent_item =0;
		QListViewItem *item =0;

		//insert root item
		

		item = new QListViewItem(lv_dirs_, 0);

		curr_dir = dir;

		QString root_name = getDirectoryFileName(dir);

		//cout << "r:" << root_name << endl;

		if (root_name.isEmpty()) 
			item->setText(0, getDirectoryFileName("/"));
		else
			item->setText(0, getDirectoryFileName(root_name));

		//int root_level = getLevel(curr_dir);

		item->setText(1, curr_dir);
		item->setPixmap(0, QPixmap::fromMimeSource("colorscm.png") );
		item->setOpen(true);

		dirs_info_[curr_dir].item = item;


		for (size_t i=0; i<info.size(); i++) {
			//cout << info[i] << endl;
			QStringList elems = QStringList::split(" ", info[i]);

			if (elems.size() < 2) {
				cerr << "wrong lists entry:" <<  info[i] << endl;
				continue;
			}

			if (elems[elems.size()-1]  == dir) 
				continue;

			if (isDirectory(elems[elems.size()-1])) {

				curr_dir_info.files.clear();
				curr_dir_info.item = 0;

				curr_dir = elems[elems.size()-1];
				//cout << "d:" << curr_dir << endl;

				dirs_info_[curr_dir] = curr_dir_info;
				parent_item = findParentItem(curr_dir);

				//cout << "p:"<< parent_item->text(0) << endl;

				if (parent_item == 0 ) 
					item = new QListViewItem(lv_dirs_, 0);
				else
					item = new QListViewItem(parent_item, 0);

				item->setText(0, getDirectoryFileName(curr_dir));
				item->setText(1, curr_dir);
				item->setText(2, info[i]);

				item->setPixmap(0, QPixmap::fromMimeSource("folder_green.png") );

				//item->setOpen(getLevel(curr_dir) - root_level == 1);

				dirs_info_[curr_dir].item = item;
			}
			else {//is a file

				continue;

				curr_dir  = getFileNameFromPath(elems[elems.size()-1]);
				cout << "f:" << elems[elems.size()-1] << " " << curr_dir << endl;
				if (elems[elems.size()-1].find("/") == -1) //root's file
					dirs_info_[ROOT_LS_ID].files.push_back(info[i]);
				else
					dirs_info_[curr_dir].files.push_back(info[i]);
			}
		}
	}
	else {
		cout << "snv loading skipped"<< endl;
		return false;
	}

	return true;
}

bool C_LSParser::fillLocalDirsInfo(const QString& dir)
{
	QDir workspace_dir(dir);
	workspace_dir.setFilter(QDir::Dirs | QDir::Hidden);	


	return true;
}

QListViewItem* C_LSParser::findParentItem(const QString& dir)
{
	int pos = dir.findRev("/");

	if (pos == -1) {
		if (dirs_info_.find(ROOT_LS_ID) != dirs_info_.end()) 
			return (dirs_info_[ROOT_LS_ID].item);
		else
			return NULL;
	}

	QString parent_dir = dir.left(pos);

	if (dirs_info_.find(parent_dir) == dirs_info_.end()) {
		cout << "not found:" << parent_dir << endl;
		return 0;
	}

	//cout << "found:" << parent_dir << endl;
	return dirs_info_[parent_dir].item;
}


QString C_LSParser::getDirectoryFileName(const QString& path)
{
	int pos = path.findRev("/");

	if (pos == -1) 
		return path;

	return path.right(path.length()-pos-1);
}

QString C_LSParser::getFileNameFromPath(const QString& path)
{
	int pos = path.findRev("/");

	if (pos == -1) 
		return path;

	return path.left(pos);
}

bool C_LSParser::getInfo(const QString& dir, vector<QString>& info)
{
	if (type_ == LSP_STATUS) {
		QStringList tmp_info;
        // C_LSParser seems to be unused so temporarily status is called with app cwd
		if (svn_command_->status(QDir::currentDirPath(), dir, tmp_info) == 0) {
			for (size_t i=0 ;i<tmp_info.size();  info.push_back(tmp_info[i++]));
			sort(info.begin(), info.end(), status_cmp() );
			return true;
		}
		else {
			cerr << "not the svn's controlled directory:" << dir.local8Bit();
			return false;
		}
	}

	if (type_ == LSP_LIST)
		return false; 

	return false;
}

bool C_LSParser::isDirectory(const QString& name)
{
	QFileInfo fi(name);
	//is_exist_ = fi.exists();

	return (fi.isDir());
}

int C_LSParser::getLevel(const QString& name) 
{
	return QStringList::split("/", name).size(); 
}
