#if !defined(__ESVN_DIFF_WRAPPER_H__)
#define __ESVN_DIFF_WRAPPER_H__
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


#include <qprocess.h>
#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#include <iostream>

class OutputReader: public QObject
{
	Q_OBJECT

private:
	QProcess   m_process;
	int        m_lines_stdout;
	int        m_lines_stderr;

	QStringList m_data_stdout, m_data_stderr;

public:
	OutputReader()
	: m_process(this), m_lines_stdout(0), m_lines_stderr(0)
	{
		connect(&m_process, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()));
		connect(&m_process, SIGNAL(readyReadStderr()), this, SLOT(readFromStderr()));
		connect(&m_process, SIGNAL(processExited()),   this, SLOT(processFinished()));
	}
	~OutputReader()
	{
	}

public:
	int count_stdout() const
	{
		return m_lines_stdout;
	}
	int count_stderr() const
	{
		return m_lines_stderr;
	}

public:
	int run_subprocess(const QStringList& process_args);

public slots:
	void readFromStdout();
	void readFromStderr();
	void processFinished();
};

#endif /* __ESVN_DIFF_WRAPPER_H__ */

