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

#include "reposwindow.h"

#define ROOT_ID "ROOT_REP_BRW "

void C_ReposWindow::bt_browse_clicked()
{
  QListViewItem *item =0;
  QString curr_dir;
  QString root_name;
  QString tmpString;

  lv_files->clear();
  lv_dirs->clear();

  // Clean entered URL : allways remove trailing slash
  tmpString = le_url->text();
  tmpString.stripWhiteSpace();
  if (tmpString.endsWith("/")) {
    tmpString = tmpString.left(tmpString.length() - 1);
  }
  le_url->setText(tmpString);

  //fill map <directory, full list strings>
  repository_.clear();

  //insert root item
  curr_dir = ROOT_ID;
  item = new QListViewItem(lv_dirs, 0);
  root_name = getDirectoryFileName(le_url->text());
  if (root_name.isEmpty()) 
    item->setText(0, getDirectoryFileName("root"));
  else
    item->setText(0, getDirectoryFileName(root_name));

  item->setText(1, curr_dir);
  item->setPixmap(0, QPixmap::fromMimeSource( "folder_blue.png" ) );
  repository_[curr_dir].item = item;
  load(item);
  item->setOpen(true);
}

void C_ReposWindow::lv_dirs_selectionChanged( QListViewItem* item)
{
  if (!repository_[getSubPath(item)].loaded) {
    load(item);
  }
  
	S_DirInfo dir_info =  repository_[getSubPath(item)];
	//cout << "selected:"<< item->text(0) << " " << item->text(1) << " " << dir_info.files.size() << endl;

	lv_files->clear();
	for (size_t i=0; i<dir_info.files.size(); i++) {
		QString file_info = dir_info.files[i];

		C_NumSortedListItem *item = new C_NumSortedListItem(lv_files, 0);
		QStringList elems = QStringList::split(" ", file_info);

		item->setText(0, getDirectoryFileName(elems[6]));
		item->setText(1, elems[0]);
		item->setText(2, elems[3] + " " + elems[4] + " " + elems[5]);
		item->setText(3, elems[2]);
		item->setText(4, elems[1]);
	}
}

QListViewItem* C_ReposWindow::findParentItem(const QString& dir)
{
	int pos = dir.findRev("/");

	if (pos == -1) {
		if (repository_.find(ROOT_ID) != repository_.end()) 
			return (repository_[ROOT_ID].item);
		else
			return NULL;
	}


	QString parent_dir = dir.left(pos);

	if (repository_.find(parent_dir) == repository_.end()) {
		cout << "not found:" << parent_dir.ascii() << endl;
		return 0;
	}

	//cout << "found:" << parent_dir << endl;
	return repository_[parent_dir].item;
}

QString C_ReposWindow::getDirectoryFileName(const QString& path)
{
	int pos = path.findRev("/");

	if (pos == -1) 
		return path;

	return path.right(path.length()-pos-1);
}

QString C_ReposWindow::getFileNameFromPath(const QString& path)
{
	int pos = path.findRev("/");

	if (pos == -1) 
		return path;

	return path.left(pos);
}


void C_ReposWindow::bt_view_clicked()
{
	QString full_path = getCurrentLocationUrl();

	QStringList cat;
    // In this context relative path below holds url as Name() component
    C_DirPath currwd;
    currwd = QDir::current();
	bool res = svn_command_->cat(C_RelativePath(currwd, full_path),"", cat);

	if (res == 0) {
		dlg_view_->tb_view->clear();
		for (size_t i=0; i<cat.size(); i++)
			dlg_view_->tb_view->append(cat[i]);

		dlg_view_->tb_view->verticalScrollBar()->setValue(0);

		dlg_view_->setCaption("View");
		dlg_view_->show();
	}
}

void C_ReposWindow::bt_checkout_clicked()
{
	QString full_path = getCurrentLocationUrl(true);

	dlg_checkout_->le_url->setText(full_path);

	if (dlg_checkout_->exec() == QDialog::Accepted) {
		QString rev = "";
		if (dlg_checkout_->rb_revision->isChecked())
			rev = (dlg_checkout_->le_revision->text());
		else
			rev	= "{" + dlg_checkout_->le_date->text() + "}";

		svn_command_->checkout(QDir::currentDirPath(), dlg_checkout_->le_url->text(),
			dlg_checkout_->le_lpath->text(),
			dlg_checkout_->le_user->text(),
			dlg_checkout_->le_password->text(),
			rev,  dlg_checkout_->ch_nonrecursive->isChecked());

	}
}

QString C_ReposWindow::getCurrentLocationUrl(bool dir_only)
{
	QString full_path;

  full_path = le_url->text();
  
	if (lv_dirs->currentItem() != NULL) {
		full_path += "/" + getSubPath(lv_dirs->currentItem());

  	if (lv_files->currentItem() != NULL && !dir_only)
		  full_path += lv_files->currentItem()->text(0);
  }

	return full_path;
}

/**
 * Load a sublevel under given item.
 */
void C_ReposWindow::load(QListViewItem *item) {
  QString curr_dir;
  QStringList elems;
  QString localUrl;
  QStringList list_info;
  S_DirInfo curr_dir_info;
  int nbFolders = 0;
  QListViewItem *tmpItem;
  QString tmpDir;
  
  curr_dir = getSubPath(item);
  
  if (!repository_[curr_dir].loaded) {
    localUrl = le_url->text() + "/" + getSubPath(item);

    if (svn_command_->list(QDir::currentDirPath(), localUrl, "", list_info, false, false) == 0) {
      for (size_t i = 0 ; i < list_info.size() ; i++) {
        elems = QStringList::split(" ", list_info[i]);
        
        if (elems.size() < 6) {
          cerr << "wrong lists entry:" <<  list_info[i] << endl;
          continue;
        } else if (elems.size() == 6 ) { //is directory
          curr_dir_info.files.clear();
          curr_dir_info.item = 0;

          tmpDir = elems[5];
		  if (tmpDir.endsWith("/")) {
			  tmpDir = tmpDir.left(tmpDir.length()-1); //remove last "/"
		  }
          
          repository_[tmpDir] = curr_dir_info;

          tmpItem = new QListViewItem(item, 0);

          tmpItem->setText(0, getDirectoryFileName(tmpDir));
          tmpItem->setText(1, tmpDir);
          tmpItem->setPixmap(0, QPixmap::fromMimeSource( "folder_blue.png" ) );

          repository_[tmpDir].item = tmpItem;
          nbFolders++;
        } else if (elems.size() == 7) {	//is file
          repository_[curr_dir].files.push_back(list_info[i]);
        }
      }
    }

    repository_[curr_dir].loaded = true;  
  }
}

/**
 * Get the sub path of an item (relative to URL entered by user) by traversing
 * it's parents to the first element in tree.
 */
QString C_ReposWindow::getSubPath(QListViewItem *item, QString res /* = "" */) {
  if (item->parent() != 0) {
    res = getSubPath(item->parent(), item->text(1) + "/" + res);
  }
  
  return res;
}
