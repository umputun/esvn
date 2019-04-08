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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <qspinbox.h>
#include <qlineedit.h> 
#include <qcheckbox.h>
#include <qsettings.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qvaluelist.h> 
#include <qsplitter.h> 
#include <qradiobutton.h> 



#include "dlg_options.h"


//this class is a sigleton wrapper for dlg_options data
//setDlg called from mainwindow constructor
class C_Options {
public:

	static C_Options* instance() {
		if (opt_instanse_ == NULL )
			opt_instanse_ = new C_Options();
		return opt_instanse_;
	}

public:
	void setDlg(dlg_options *dlg) {
		if (opt_instanse_) 
			dlg_options_ = dlg;
		else
			throw "options: empty instanse";
	}

	//accessors
	dlg_options* dlgOptions() {
		return dlg_options_;
	}

private:
	C_Options() : dlg_options_(NULL){}
	~C_Options() {}

private:
	static C_Options* opt_instanse_;
	dlg_options* dlg_options_;

};



#endif


