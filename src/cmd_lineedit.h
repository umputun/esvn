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

#include <vector>

#include <qlineedit.h>
#include <qstring.h>

class QKeyEvent;
class Cmd_LineEdit : public QLineEdit 
{
	Q_OBJECT
public:
    Cmd_LineEdit(QWidget * parent, const char * name = 0 ) ;

protected:
	virtual void keyPressEvent( QKeyEvent * e );

private slots:
	void saveCommand();

private:
	std::vector<QString> cmd_hist;
	size_t pos;
};


