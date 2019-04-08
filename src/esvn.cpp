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
  

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <qapplication.h>
#include <qstring.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	//get --svndir parameter
	QString svn_dir = ".svn";
	for (int i=1; i<argc; i++) {
		QString arg = argv[i];
		int p = arg.find("--svndir=");
		if (p != -1) {
			svn_dir = arg.right(arg.length()-p-9);
			cout << "Special svn directory used: " << svn_dir.ascii() << endl;
		}
	}

	QApplication app(argc, argv);
	C_MainWindow main_form;
	app.setMainWidget(&main_form);

	QObject::connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
	main_form.setSvnDir(svn_dir);
	main_form.show();
	main_form.loadSettings();
	int ret = app.exec();
	main_form.saveSettings();

	return ret;
}

