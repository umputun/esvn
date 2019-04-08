
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
#include <qeventloop.h>
#include <qsettings.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#if ( _WINDOWS || ESVN_WIN )
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>

#include "esvn-diff-wrapper.h"

int main (int argc, char *argv[])
{
	QApplication app(argc, argv, false);
//	QApplication app(argc, argv, QApplication::Tty);

	QStringList process_args;

	#ifdef DEBUG
		 for (size_t i=1; i < argc; i++) 
			printf("arg [%d]:%s\n", i, argv[i]);
	#endif

	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
	QSettings	settings;
	settings.setPath("umputun.com", "esvn", QSettings::User);
	settings.beginGroup("/esvn");

	QString esvn_diff_mask = argc >= 8 ?
		settings.readEntry("/progs/diff", "kdiff3" ) :
		settings.readEntry("/progs/diff3", "kdiff3" );
	settings.endGroup();

	if (esvn_diff_mask.isEmpty()) {
		printf("empty diff mask\n");
		return 2;
	}

	#ifdef DEBUG
		printf("mask: %s\n", esvn_diff_mask.ascii());
	#endif 	

	// If diff executable path itself contains spaces, then it should be
	// enclosed in quotation marks, i.e. diff program parameter may look like
	// "diff executable path" flag1 flag2

	QStringList elems;

	// Extract executable path without quotation marks
	int start = esvn_diff_mask.find("\"");
	int end = esvn_diff_mask.find("\"", start + 1);
	if(start != -1 && end != -1){
		elems += esvn_diff_mask.mid(start + 1, end - start - 1);
		esvn_diff_mask.remove(start, end - ( start - 1 ) );
	}

	// Extract remaining program arguments. Spaces in arguments are not supported
	elems += QStringList::split(" " , esvn_diff_mask);

	process_args.push_back(QDir::convertSeparators(elems[0]));

	//find position of '-L'
	int pos = 0;
	for (int p=1; p<argc; p++) {
		QString arg = argv[p];
		if (arg == "-L" && pos == 0) {
			pos = p;
			#ifdef DEBUG
		   	  printf("pos found %s %d\n", arg.ascii(), p);
			#endif
			break;
		}
	}

	//if(argc == 8) pos += 2;
	//printf("-L pos=%d\n", pos);

	QString label1("no label"), label2("no label");
	QString filename1(""), filename2(""), filename3(""), filename4("");

	if (argc == 6) {
		// 3-way diff call from eSvn
		filename1 = argv[2];
		filename2 = argv[3];
		filename3 = argv[4];
		filename4 = argv[5];
	} else if (argc-pos >= 6) {
		// called from svn diff
		label1 = argv[pos+1];
		label2 = argv[pos+3];
		filename1 = argv[pos+4];
		filename2 = argv[pos+5];
	}

	if (elems.size() == 1) {
		process_args.push_back(QDir::convertSeparators(filename1));
		process_args.push_back(QDir::convertSeparators(filename2));
		// Do not add 3rd argument if empty
		if (filename3.length() > 0) {
			process_args.push_back(QDir::convertSeparators(filename3));
		}
	}
	else {
		for (size_t i=1; i<elems.size() ; i++) {
			QString e = elems[i];
			if (e == "f1" || e == "F1"){
				process_args.push_back(filename1);
			}
			else if (e.find("%f1") != -1 || e.find("%F1") != -1 ){
				e.replace("%F1", "%f1");
				e.replace("%f1", QDir::convertSeparators(filename1));
				process_args.push_back(e);

			}else if (e == "f2" || e == "F2") {
				process_args.push_back(filename2); 
			}
			else if (e.find("%f2") != -1 || e.find("%F2") != -1 ){
				e.replace("%F2", "%f2");
				e.replace("%f2", QDir::convertSeparators(filename2));
				process_args.push_back(e);
			}
			else if (e == "f3" || e == "F3") {
				process_args.push_back(filename3); 
			}
			else if (e.find("%f3") != -1 || e.find("%F3") != -1 ){
				e.replace("%F3", "%f3");
				e.replace("%f3", QDir::convertSeparators(filename3));
				process_args.push_back(e);
			}
			else if (e == "f4" || e == "F4") {
				process_args.push_back(filename4); 
			}
			else if (e.find("%f4") != -1 || e.find("%F4") != -1 ){
				e.replace("%F4", "%f4");
				e.replace("%f4", QDir::convertSeparators(filename4));
				process_args.push_back(e);
			}
			else if (e == "a1" || e == "A1"){ 
				process_args.push_back(label1.replace('\t', ' ')); 
			}
			else if (e.find("%a1") != -1 || e.find("%A1") != -1 ){
				e.replace("%A1", "%a1");
				e.replace("%a1", label1.replace('\t', ' '));
				process_args.push_back(e);
			}
			else if (e == "a2" || e == "A2") { 
				process_args.push_back(label2.replace('\t', ' ')); 
			}
			else if (e.find("%a2") != -1 || e.find("%A2") != -1 ){
				e.replace("%A2", "%a2");
				e.replace("%a2", label2.replace('\t', ' '));
				process_args.push_back(e);
			}
			else
			{
				process_args.push_back(e);
			}
		}
	}

#ifdef DEBUG
	printf("exec diff: ");
	for (QStringList::iterator it = process_args.begin(); it != process_args.end(); ++it)
		printf(" %s", (*it).ascii());
	printf("\n");
#endif

#ifdef DEBUG
	QTime timeStart = QTime::currentTime();
#endif

	OutputReader reader;

	int exit_status = reader.run_subprocess(process_args);

#ifdef DEBUG
	QTime timeDone = QTime::currentTime();

	std::cout << "Running time     : " << timeStart.msecsTo(timeDone)  << " ms" << std::endl;
	std::cout << "Read stdout lines: " << reader.count_stdout() << std::endl;
	std::cout << "Read stderr lines: " << reader.count_stderr() << std::endl;
#endif

	return exit_status;
}

int OutputReader::run_subprocess(const QStringList& process_args)
{
	int exit_status;

	m_process.setArguments(process_args);

	if (m_process.start()) {
		if (m_process.isRunning()) {
			exit_status = qApp->exec();
			std::cout << "Event loop returned exit status " << exit_status << std::endl;
		}
		else {
			exit_status = m_process.exitStatus();
			std::cout << "Process is not running, exit status " << exit_status << std::endl;
		}
	}
	else {
		std::cout << "Process cannot be started" << std::endl;
		exit_status = -1;
	}

	return exit_status;
}

void OutputReader::readFromStdout()
{
	while (m_process.canReadLineStdout()) {
		++m_lines_stdout;
		QString src_msg = m_process.readLineStdout();
	}
}

void OutputReader::readFromStderr()
{
	while (m_process.canReadLineStderr()) {
		++m_lines_stderr;
		QString err_line = m_process.readLineStderr();
		QString str = QTextCodec::codecForLocale()->toUnicode(err_line);
	}
}

void OutputReader::processFinished()
{
	qApp->exit(m_process.exitStatus());
}

