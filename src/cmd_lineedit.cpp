/***************************************************************************
*   Copyright (C) 2004 by Mark Hillary                                    *
*   mark@no33.co.uk                                                       *
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

#include <qevent.h>

#include "cmd_lineedit.h"

Cmd_LineEdit::Cmd_LineEdit(QWidget * parent, const char * name) 
	: QLineEdit(parent, name) 
	, pos(0)
{
	this->setFrame(false);
	connect(this, SIGNAL( returnPressed() ), this, SLOT( saveCommand() ) );
}

void Cmd_LineEdit::keyPressEvent( QKeyEvent * e )
{
	if( ( e->key() == Qt::Key_Up ) && ( pos > 0) )
		setText(cmd_hist[--pos]);
	else if( ( e->key() == Qt::Key_Down ) &&  ( pos + 1 < cmd_hist.size() ) )
		setText(cmd_hist[++pos]);
	else
		QLineEdit::keyPressEvent(e);
}

void Cmd_LineEdit::saveCommand()
{
	/* Add Command to History */
	QString cmd = text();
	if(cmd_hist.size() >= 10)
		cmd_hist.erase(cmd_hist.begin(), cmd_hist.begin()+1);
	cmd_hist.push_back(cmd);
	pos = cmd_hist.size();
}


