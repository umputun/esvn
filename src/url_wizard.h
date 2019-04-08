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

#ifndef URL_WIZARD_H
#define URL_WIZARD_H

#include "dlg_url.h"

class C_UrlWizard : public dlg_url {
public:
	C_UrlWizard(QWidget* parent = 0, const char* name = 0) :
		dlg_url(parent, name) {}

	void url(const QString& in_url) {
		le_url_->setText(in_url);
	}

	QString url() {
		return le_url_->text();
	}

};
#endif


