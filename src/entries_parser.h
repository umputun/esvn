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

#ifndef ENTRIES_PARSER_H
#define ENTRIES_PARSER_H

#include <sys/types.h>
#include <time.h>
#include <qstring.h>
#include <qdir.h>

#include <map>
#include <iostream>

#include "esvn_utils.h"

using namespace std;

struct C_SvnFile {

	enum E_Status {NonSvn, UpToDate, Add, Modify, Delete, Conflict };
	
	C_SvnFile() : xml_entry_(""), is_svn_controled_(false), is_symlink_(false), is_modif_(false) {}

	C_SvnFile(const QString& xml_entry, const QString& file_name, const QString &path) 
		: xml_entry_(xml_entry), file_name_(file_name), path_(path){}

	QString xml_entry_;
	QString file_name_, path_;
	QString name_, url_;
	QString revision_, commited_revision_;
	QString kind_;
	QString uuid_;
	QString last_author_;
	QString schedule_;
	QString deleted_;

	QString tag_, branch_;
	QString conflict_wrk_, conflict_old_, conflict_new_;
	QDateTime commited_dtm_, modif_dtm_, text_dtm_;

	unsigned long size_;
	bool is_svn_controled_;
	bool is_exist_;
	bool is_symlink_;
	bool is_modif_;

	void clean() {
		xml_entry_ = "";
		file_name_ = path_ = name_ = kind_ = schedule_ = deleted_ = "";
		revision_ = commited_revision_ = "";
		last_author_ = uuid_ = "";

		conflict_wrk_= conflict_old_ = conflict_new_ = "";
		last_author_ = uuid_ = tag_ = branch_ = "";

		commited_dtm_ = modif_dtm_ = QDateTime();
		size_ = 0;
		is_modif_ = 0;
	}

	E_Status getStatus();

private:
	bool parse();

};

typedef std::map<QString, C_SvnFile> Name2SvnFileMap;

class C_SvnEntries {
private:
    C_SvnEntries(const C_SvnEntries&) {
    }
    C_SvnEntries& operator=(const C_SvnEntries&){
        return *this;
    }

public:
	C_SvnEntries(const QString& svn_dir=".svn") : svn_dir_(svn_dir) {}

	void setSvnDir(const QString& svn_dir) {svn_dir_ = svn_dir;}

	C_SvnFile get(QString file_name) {
        Name2SvnFileMap::iterator it = data_.find(file_name);
		if (it == data_.end()) {
			C_SvnFile result;
			result.is_svn_controled_ = false;
			return result;
		}
		return it->second;
	}

	void erase(QString file_name) {
        Name2SvnFileMap::iterator it = data_.find(file_name);
		if (it != data_.end()) 
			data_.erase(it);
	}

    bool cmp(C_SvnEntries sec);

	bool setDir(const C_Path &dir, bool proc_subdirs) {
		if (QFileInfo(dir.FullPath()).exists()) {
			//cout << "C_SvnEntries path: " << dir.FullPath() << endl;
			refresh(dir, proc_subdirs);
			return true;
		}
		else
			return false;
	}

	//void refresh(bool proc_subdirs);
    void refresh(const C_Path& curr_dir, bool proc_subdirs);

    static bool tryEntriesXML (Name2SvnFileMap &data, QFile &entries_file, QString &error_message, int &error_line, int &error_column, const QString& svn_dir, const C_Path& dir, bool proc_subdirs = false);
    static bool tryEntriesText(Name2SvnFileMap &data, QFile &entries_file, QString &error_message, int &error_line, int &error_column, const QString& svn_dir, const C_Path& dir, bool proc_subdirs = false);
	static void fillEntries(Name2SvnFileMap &data, const QString& svn_dir, const C_Path& dir, bool proc_subdirs=false);
	static void getTagBranch(const QString& url, QString& tag, QString& branch);

	void clear() {
		data_.clear();
	}

	Name2SvnFileMap& getData() {
		return data_;
	}


private:
	static QDateTime svn2dtm(const QString& value);
	Name2SvnFileMap data_;
	bool is_svn_controled_;
	QString svn_dir_;
};

#endif

