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

#include "svn_status.h"

bool C_SvnStatus::status(const QString &dir) 
{

	QStringList status_list;

	int res = svn_command_->status_curr(dir, "", status_list);

	if (res == 0) {
		data_.clear();

		for (size_t i=0; i<status_list.size(); i++) {
			//cout <<  status_list[i] << endl;
			QStringList elems = QStringList::split(" ", status_list[i]);

			if (elems.size() > 2) {
				if (elems[0].isEmpty())
					continue;

				if (elems[0] == "Status") //skip the string "Status against ..."
					continue;

				QString status_str = "normal";
				char st = elems[0].at(0).latin1();					  
				switch (st) {
					case 'M': status_str="modified";	break;
					case 'A': status_str="added";	   	break;
					case 'D': status_str="deleted";	 	break;
					case 'C': status_str="conflict";	break;
					case 'X': status_str="e-related";	break;
					case 'I': status_str="ignored";		break;
					case '!': status_str="missing";		break;
					case '~': status_str="dir?file";	break;
					case '*': status_str="out-of-date"; break;
					case 'L': status_str="locked";	  	break;
					case 'S': status_str="switched";	break;
					default:  status_str=st;			break;
				}

				S_StatusInfo status_info;

				status_info.file_name_	= elems[elems.size() - 1];
				status_info.file_name_ = dir + "/" + status_info.file_name_;

				status_info.file_name_.replace('\\','/');

				if (elems.size() > 3) {
					status_info.author_ 	= elems[elems.size() - 2];
					status_info.last_ch_rev_= atol(elems[elems.size() - 3].ascii());
					status_info.rev_ 		= atol(elems[elems.size() - 4].ascii());
				}
				status_info.status_ 	= status_str;
				data_[status_info.file_name_] = status_info;
				//cout << status_info.file_name_ << " : " << status_info.status_ << " " << status_info.rev_ << endl;
			}
		}
	}
	return (res == 0);
}

bool C_SvnStatus::cmpData(C_SvnStatus* in_svn_stauts) {
	if (data_.size() != in_svn_stauts->getData()->size()) 
		return false;

	map<QString, S_StatusInfo>::iterator iter_data, iter_in;

	for (iter_data = data_.begin();iter_data != data_.end(); iter_data++ ) {
		iter_in = in_svn_stauts->getData()->find(iter_data->second.file_name_);
		if (iter_in == in_svn_stauts->getData()->end())
			return false;

		S_StatusInfo this_info = iter_data->second;
		S_StatusInfo int_info  = iter_in->second;

		if (this_info.last_ch_rev_ != int_info.last_ch_rev_ || this_info.rev_ != int_info.rev_)
			return false;
	}

	return true;
}

