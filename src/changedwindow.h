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

#ifndef CHANGED_WINDOW_H
#define CHANGED_WINDOW_H

#include <qfileinfo.h>
#include "entries_parser.h"

#include "dlg_changed.h"
#include "svn_commands.h"
#include "dlg_view.h"
#include "dlg_diff_output.h"
#include "logwindow.h"

class C_ChangedWindow : public dlg_changed {
public:
	C_ChangedWindow(QWidget* parent = 0, const char* name = 0) :
		  dlg_changed(parent, name), use_ext_diff_(false) {}

	  void setParams(C_SvnCommands* svn_command,  dlg_view *view, dlg_diff *diff) {
		  svn_command_ = svn_command;
		  dlg_view_ = view;
		  dlg_diff_ = diff;
	  }

	  void setFileDir(const C_DirPath& file_dir) {
          file_dir_ = file_dir;
	  }

      void useExtDiff(bool use_ext_diff) {
          use_ext_diff_ = use_ext_diff;
      }

	  void bt_refresh_clicked() {
		  lv_status->clear();
		  QStringList status_list;
          // for this dialog current working directory is
          // the same as file_dir_ for which changes were quieried
          int res = svn_command_->status(C_RelativePath(file_dir_,"."), true, status_list);
		  if (res == 0) {
			  for (size_t i=0; i<status_list.size(); i++) {
				  QStringList elems = QStringList::split(" ", status_list[i]);
				  if (elems.size() > 2) {
					  if (elems[0].isEmpty())
						  continue;
					  if (elems[0] == "Status")
						  continue;

					  QString status_str = "not detected";
					  char st = elems[0].at(0).latin1();					  
					  switch (st) {
						  case 'M': status_str="modified";	  break;
						  case 'A': status_str="added";	   	  break;
						  case 'D': status_str="deleted";	  break;
						  case 'C': status_str="conflict";	  break;
						  case 'X': status_str="e-related";	  break;
						  case 'I': status_str="ignored";	  break;
						  case '!': status_str="missing";	  break;
						  case '~': status_str="dir?file";	  break;
						  case '*': status_str="out-of-date"; break;
						  case 'L': status_str="locked";	  break;
						  case 'S': status_str="switched";	  break;
						  default:  status_str=st;			  break;
					  }


					  QListViewItem *item = new QListViewItem(lv_status, 0);
                      C_RelativePath element_path(file_dir_, elems[elems.size() -1]);
					  item->setText(0, element_path.Name());
					  QFileInfo fi(element_path.FullPath());
					  if( fi.exists()) {
						  item->setText(2, 
							fi.lastModified().toString("yy/MM/dd hh:mm:ss"));
					  }

					  item->setText(1, status_str);

					  if (fi.exists() && fi.isDir())
						  item->setPixmap(0, QPixmap::fromMimeSource( "FolderClosed16x16.xpm")) ;
					  else
						  if (fi.exists() && fi.isFile())
							  item->setPixmap(0, QPixmap::fromMimeSource( "FileUnchanged16x16.xpm")) ;
						  else
							  item->setPixmap(0, QPixmap::fromMimeSource( "FileStatus16x16.xpm")) ;

				  }
			  }
			  if (status_list.size() > 0) 		
					lv_status->setSelected(lv_status->firstChild(), true);
		  }
	  }
	
	virtual void bt_view_clicked() {
		QListViewItem *item = lv_status->selectedItem();
		if (!item) return;

		QStringList view_txt;
		//cout << "v:"<<item->text(0)<<"|"<<endl;
		if (svn_command_->cat(C_RelativePath(file_dir_, item->text(0)), "HEAD", view_txt) == 0) {
			dlg_view_->tb_view->clear();
			for (size_t i=0; i<view_txt.size(); i++) {

				if (!view_txt[i].isEmpty() ) 
					dlg_view_->tb_view->append(view_txt[i]);
				else
					dlg_view_->tb_view->append("");
			}

			dlg_view_->tb_view->verticalScrollBar()->setValue(0);
			dlg_view_->show();
		}
	}
	
	virtual void bt_update_clicked() {
		//QListViewItem *item = lv_status->selectedItem();
		QListViewItem *item = lv_status->selectedItem();
		if (!item) return;

        C_RelativePath item_path(file_dir_, item->text(0));
        PathSelection selection;
        selection.push_back(item_path);

		svn_command_->update(selection, "");
	}

	virtual void bt_diff_clicked() {
		QListViewItem *item = lv_status->selectedItem();

		if (!item) return;

		QStringList diff_res;
        C_RelativePath item_path(file_dir_, item->text(0));
		int res = svn_command_->diff(item_path.Path(),
                                     item_path.Name(), "", "HEAD", 
                                     use_ext_diff_, false, false, "", diff_res);

		if (res == 0 && !use_ext_diff_) {
			dlg_diff_->tx_diff->clear();
			for (size_t i=0; i<diff_res.size(); i++) {
				QString str = diff_res[i];

				if ( str.length() > 0 && str[0] == '+') {
					dlg_diff_->tx_diff->append("<font color=blue>" + diff_res[i] + "</font>");
					continue;
				}
				if (str.length() > 0 && str[0] == '-') {
					dlg_diff_->tx_diff->append("<font color=red>" + diff_res[i] + "</font>");
					continue;
				}
				dlg_diff_->tx_diff->append(diff_res[i]);

			}
			dlg_diff_->tx_diff->verticalScrollBar()->setValue(0);
			dlg_diff_->show();
		}
	}

	void bt_log_clicked(){
		QListViewItem *item = lv_status->selectedItem();
		if (!item)
			return;

        C_RelativePath full_name(file_dir_, item->text(0));

        C_LogWindow dlg_log_(this);
		dlg_log_.setParams(svn_command_, dlg_view_, dlg_diff_);

        dlg_log_.setFileDir(full_name);
		dlg_log_.load(true);
		dlg_log_.exec();
	}


private:
	C_SvnCommands* svn_command_;
	dlg_view *dlg_view_;
	dlg_diff *dlg_diff_;
	C_DirPath file_dir_;
	bool use_ext_diff_;
};

#endif



