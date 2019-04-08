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

#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include "dlg_log.h"
#include "svn_commands.h"
#include "dlg_view.h"
#include "dlg_diff_output.h"

#include "esvn_utils.h"

#include <qlistview.h>
#include <qpushbutton.h> 
#include <qcombobox.h> 
#include <qcheckbox.h> 
#include <qpainter.h> 

class C_ColoredListViewItem : public QListViewItem
{
public:
	C_ColoredListViewItem(QListView *parent, QListViewItem * after)
	: QListViewItem(parent, after)
	{
		color_selection_ = 0;
	}
    
	C_ColoredListViewItem(QListViewItem *parent, QListViewItem * after)
	: QListViewItem(parent, after)
	{
		color_selection_ = 0;
	}    

	virtual void paintCell(QPainter *painter, const QColorGroup &cg, int column, int width, int alignment) {
		if (color_selection_ != 1 && color_selection_ != 2) {
			// Paint with default color
			QListViewItem::paintCell(painter, cg, column, width, alignment);
			return;
		}

		painter->save();

		QColorGroup grp(cg);
		switch (color_selection_) {
		case 1:
			grp.setColor(QColorGroup::Base, QColor(192,255,192));
			grp.setColor(QColorGroup::Highlight, QColor(0,255,0));
			break;
		case 2:
			grp.setColor(QColorGroup::Base, QColor(255,192,192));
			grp.setColor(QColorGroup::Highlight, QColor(255,0,0));
			break;
		default:
			break;
		}
        QListViewItem::paintCell(painter, grp, column, width, alignment);

		painter->restore();
	}

	void setColor(int selection)
	{
		color_selection_ = selection;
	}

private:
	int color_selection_;
};

class C_LogWindow : public dlg_log {

public:
	C_LogWindow(QWidget* parent = 0, const char* name = 0) 
	: dlg_log(parent, name) {
		msg_box->setContentsPos (0,  0);
		is_wizard_ = false;
		itemRev[0] = itemRev[1] = NULL;
	}

	void setFileDir(const C_Path& pathname) {
		file_dir_ = pathname;
	}

	void setParams(C_SvnCommands* svn_command, dlg_view *view, dlg_diff *diff, bool is_wizard = false) {
		svn_command_ = svn_command;
		dlg_view_ = view;
		dlg_diff_ = diff;
		use_ext_diff_ =  false;
		this->is_wizard_ = is_wizard;
		if (is_wizard) {
			bt_select->show();
			bt_diff->hide();
			bt_update->hide();
		}
		else {
			bt_select->hide();
			bt_diff->show();
			bt_update->show();
		}
	}
	void useExtDiff(bool use_ext_diff) {
		use_ext_diff_ = use_ext_diff;
	}

	virtual void bt_view_clicked() {
		QListViewItem *item = list_log->selectedItem();

		if (!item)
			return;

		QStringList view_txt;
		//cout << "v:"<<item->text(0)<<"|"<<endl;
		if (item && svn_command_->cat(file_dir_, item->text(0), view_txt) == 0) {
			dlg_view_->tb_view->clear();
			for (size_t i=0; i<view_txt.size(); i++)
				dlg_view_->tb_view->append(view_txt[i]);

			dlg_view_->tb_view->verticalScrollBar()->setValue(0);

			if (is_wizard_) 
				dlg_view_->exec();
			else
				dlg_view_->show();
		}
	}

	virtual void bt_update_clicked() {
		QListViewItem *item = list_log->selectedItem();

		if (!item) return;

        PathSelection selection;
        selection.push_back(file_dir_);

		svn_command_->update(selection, item->text(0));
	}

	virtual void bt_diff_clicked() {

		QStringList diff_res;

		int res = svn_command_->diff(file_dir_.Path(),
                                     file_dir_.Name(), "",  getSelectedRevision(), 
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


	virtual void bt_diff_prev_clicked() {
		QListViewItem *item = list_log->selectedItem();

		if (!item) return;

		QStringList diff_res;
		int res = svn_command_->diff(file_dir_.Path(),
                                     file_dir_.Name(), "", item->text(0), 
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


	void bt_chpath_clicked() {
		QStringList log;
		int res = svn_command_->log(file_dir_, true, ch_stop->isChecked(), log);
		if (res == 0) {
			msg_box->clear();
			for (size_t i=0; i<log.size(); i++) 
				msg_box->append(log[i]);
			msg_box->verticalScrollBar()->setValue(0);
		}

	}

	QString getSelectedRevision() {
		if (is_wizard_) {
			if (list_log->selectedItem())
				return list_log->selectedItem()->text(0);
		}
		else if (itemRev[0]) {
			// Primary selection makes '-r M' parameter
			QString diffRevision = itemRev[0]->text(0);

			// Secondary selection makes '-r M:N' parameter
			// If it is the same selection, revert to single selection
			if (itemRev[1] && itemRev[1] != itemRev[0]) {
				diffRevision += ":" + itemRev[1]->text(0);
			}

			return diffRevision;
		}

		return QString("");
	}

    void bt_apply_clicked() {

		if (!cb_msg->currentText().isEmpty() ) {
			bool is_found = false;
			for (int i=0; i<cb_msg->count(); i++) {
				if (cb_msg->text(i) == cb_msg->currentText()) {
					is_found = true;
					break;
				}
			}
			if (!is_found) 
				cb_msg->insertItem(cb_msg->currentText());
		}

		if (!cb_author->currentText().isEmpty() ) {
			bool is_found = false;
			for (int i=0; i<cb_author->count(); i++) {
				if (cb_author->text(i) == cb_author->currentText()) {
					is_found = true;
					break;
				}
			}

			if (!is_found) 
				cb_author->insertItem(cb_author->currentText());
		}

		load(show_changed_paths_);
	}

	void load(bool show_changed_paths) {
		show_changed_paths_ = show_changed_paths;
		itemRev[0] = itemRev[1] = NULL;
		QStringList log;
		int res = svn_command_->log(file_dir_, show_changed_paths, ch_stop->isChecked(), log);

		if (res == 0) {
			list_log->clear();
			msg_box->clear();

			QListViewItem *item = NULL;

			QString msg;
			QString changed_paths;

			bool in_changes_paths = false;
			bool was_rev = false;
			bool is_not_filterd = true;

			for (QStringList::ConstIterator i=log.begin(); i != log.end(); i++) {

				QString curr_str = *i;

				if ((*i).find("-----") != -1) {

                    if (item) {
                        if (!msg.isEmpty()) 
                            item->setText(6, msg);

                        if (!changed_paths.isEmpty()) 
                            item->setText(6, item->text(6) + "<br><font color=navy>" + changed_paths + "</font>" );
                    }

					//test msg filter
					bool msg_ok = cb_msg->currentText().isEmpty() 
								  || ( msg.find(cb_msg->currentText()) != -1 )
								  || (changed_paths.find(cb_msg->currentText()) != -1 ) ;

					if ((!is_not_filterd || !msg_ok) && item) {
						list_log->takeItem(item);
						//cout << "flt:"<< item->text(0) << endl;;
					}


					msg = ""; changed_paths = "";
					was_rev = false; in_changes_paths = false; is_not_filterd = true;
					continue;
				}

				if ((*i).length() > 1 && (*i).at(0) == 'r' && !was_rev) {
					was_rev = true;
					QStringList elems = QStringList::split('|', *i, true);
					if (elems.size() < 4) {
						cerr << "invalid log entry:" << *i << endl;
						continue;
					}
					
					item = new C_ColoredListViewItem(list_log, 0);

					QString rev = elems[0].mid(1, elems[0].length()-2);
					item->setText(0, rev);
					item->setText(1, elems[2].mid(0, 20) );
					item->setText(2, elems[1]); //author
					is_not_filterd = (cb_author->currentText().isEmpty()) || ( elems[1].find(cb_author->currentText()) != -1);

					
				}
				else {
					if (msg.isEmpty () && (*i).isEmpty() && !in_changes_paths)
						continue; //skip first EOL

					if (in_changes_paths) {
						if ( ! (*i).isEmpty()) 
							changed_paths = changed_paths + *i + "<br>";
						else
							in_changes_paths = false;
					}
					else {
						if ((*i).find("Changed paths:") == 0) 
							in_changes_paths = true;
						else
							msg = msg + *i + "<br>";
					}
				}
			}
				
			list_log->setSorting(1, FALSE);
			list_log->setSelected(list_log->currentItem(), true);
			setCaption(file_dir_.Name());

			// Setup initial selection
			itemRev[0] = list_log->selectedItem();
			updateItemColors();
		}
	}

	virtual void list_log_mouseButtonPressed(int num, QListViewItem *pItem, const QPoint &, int) {

		if (num == 1 || num == 2) {

			QListViewItem *oldItem = itemRev[num - 1];
			if (oldItem == pItem)
				return;

			// Change our selection
			itemRev[num - 1] = pItem;

			// Display old deselected
			updateItemColor(oldItem, 0);

			updateItemColors();
		}
	}

protected:
	void updateItemColors()
	{
		// Display new selection
		updateItemColor(itemRev[0], 1);

		// Do not display secondary selection if it matches primary selection
		if (itemRev[0] == itemRev[1])
			return;

		updateItemColor(itemRev[1], 2);
	}

	void updateItemColor(QListViewItem* pItem, int num )
	{
		// Do nothing in wizard mode (single selection will work)
		if (is_wizard_)
			return;

		if (!pItem)
			return;

		C_ColoredListViewItem *pColoredItem = dynamic_cast<C_ColoredListViewItem*>(pItem);
		if (!pColoredItem)
			return;

		pColoredItem->setColor(num);
		pColoredItem->repaint();
	}

private:
	C_SvnCommands* svn_command_;
	dlg_view *dlg_view_;
	dlg_diff *dlg_diff_;
	C_PathCopy file_dir_;
	bool use_ext_diff_, is_wizard_, show_changed_paths_ ;
	QListViewItem *itemRev[2];
};

#endif

