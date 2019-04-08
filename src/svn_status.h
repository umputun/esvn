/***************************************************************************
 *   Copyright (C) 2004 by umputun                                         *
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

#ifndef SVN_STATUS_H
#define SVN_STATUS_H

#include <map>
using namespace std;

#include "svn_commands.h"

struct S_StatusInfo {
	QString file_name_;
	QString author_;
	long rev_;
	long last_ch_rev_;
	QString status_;
};

class C_SvnStatus {
public:
        typedef map<QString, S_StatusInfo> StatusData;

	C_SvnStatus(C_SvnCommands* svn_command) : svn_command_(svn_command) {
	}

	bool status(const QString& dir);

	StatusData* getData() {
		return &data_;
	}

	void setData(C_SvnStatus* in_svn_stauts) {
		data_ = *(in_svn_stauts->getData());
	}

	bool cmpData(C_SvnStatus* in_svn_stauts);

private:
	C_SvnCommands* svn_command_;
	map<QString, S_StatusInfo> data_;
};

#endif
