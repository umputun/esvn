/***************************************************************************
 *   Copyright (C) 2004 by Eugene Bort, Damien Caliste                     *
 *   esvn@umputun.com                                                      *
 *   dcaliste@free.fr                                                      *
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

#include "entries_parser.h"

#include <qdom.h>


bool C_SvnFile::parse()
{
	if (xml_entry_ == "")
		return false;

	//fill file information
	QFileInfo fi( file_name_ );
	is_exist_ = fi.exists();
	if( is_exist_ ) {
		modif_dtm_ = fi.lastModified();
		size_ = fi.size();
		is_symlink_ = fi.isSymLink();
		return true;
	}
    return false;
}

void C_SvnEntries::refresh(const C_Path& curr_dir, bool proc_subdirs)
{
    fillEntries(data_, svn_dir_, curr_dir, proc_subdirs);
}

bool C_SvnEntries::tryEntriesText(Name2SvnFileMap &data, QFile &entries_file, QString &error_message, int &error_line, int &error_column, const QString& svn_dir, const C_Path& dir, bool proc_subdirs)
{
  // Try to read the given file as new text format (version > 6).
  if ( entries_file.open( IO_ReadOnly ) ) {
    QTextStream first, stream;
        QString line;
	bool ok;
	
	C_SvnFile svn_file;
	QString base_revision, base_tag, base_branch;

#define GO_TO_NEXT_ENTRY do {line = stream.readLine();} while (!line.isNull() && !line.startsWith("\x0c"))
#define MAYBE_DONE line = stream.readLine(); if (line.startsWith("\x0c")) goto done

	// First pass.
	first.setEncoding(QTextStream::UnicodeUTF8);
	first.setDevice(&entries_file);

	// Check the format.
	line = first.readLine();
	if (line.toInt(&ok) < 7) {
	  error_line = 1;
	  error_column = 1;
	  error_message = "incompatible file version.";
	  entries_file.close();
	  return false;
	}

	data.clear();

	// Check first name is void and format is dir.
	line = first.readLine();
	line = first.readLine();
	if (line != "dir") {
	  error_line = 3;
	  error_column = 1;
	  error_message = "parse error, wrong field format.";
	  entries_file.close();
	  return false;
	}
	
	// Get the revision and the the url.
	base_revision = first.readLine();
	line = first.readLine();
	//process tag/branch info
	if (!line.isEmpty())
	  getTagBranch(line, base_tag, base_branch);

        entries_file.close();
	
	// Second pass, real storage.
	entries_file.open( IO_ReadOnly );

	// First pass.
	stream.setEncoding(QTextStream::UnicodeUTF8);
	stream.setDevice(&entries_file);

	// Ignore the format line
	line = stream.readLine();

	// Read .
        while ( !stream.atEnd() ) {
	  svn_file.name_ = stream.readLine();
	  svn_file.kind_ = stream.readLine();
	  svn_file.deleted_ = "";
	  svn_file.revision_ = "";
	  svn_file.text_dtm_ = svn2dtm("");

	  MAYBE_DONE;
	  svn_file.revision_ = line; // copy revision field.
	  MAYBE_DONE;
	  svn_file.url_ = line; // copy url field.
	  //process tag/branch info
	  if (!svn_file.url_.isEmpty()) {
	    // element switched to other location
	    getTagBranch(svn_file.url_, svn_file.tag_, svn_file.branch_);
	  } else {
	    // element at default location
	    svn_file.branch_ = base_branch;
	    svn_file.tag_ = base_tag;
	  }
	  MAYBE_DONE; // ignore repos field.
	  MAYBE_DONE;
	  svn_file.schedule_ = line; // copy schedule field.
	  MAYBE_DONE;
	  svn_file.text_dtm_ = svn2dtm(line); //read text-time field.
	  MAYBE_DONE; // ignore checksum field.
	  MAYBE_DONE;
	  svn_file.commited_dtm_ = svn2dtm(line); //read commited-date field.
	  MAYBE_DONE;
	  svn_file.commited_revision_ = line; // copy committed-rev field.
	  MAYBE_DONE;
	  svn_file.last_author_ = line; // copy last-author field.
	  MAYBE_DONE; // ignore has-props field.
	  MAYBE_DONE; // ignore has-prop-mods field.
	  MAYBE_DONE; // ignore cachable-props field.
	  MAYBE_DONE;
	  svn_file.conflict_old_ = line; // copy conflict-old field.
	  MAYBE_DONE;
	  svn_file.conflict_new_ = line; // copy conflict-new field.
	  MAYBE_DONE;
	  svn_file.conflict_wrk_ = line; // copy conflict-wrk field.
	  MAYBE_DONE; // ignore prop-reject-file field;
	  MAYBE_DONE; // ignore copied field.
	  MAYBE_DONE; // ignore copyfrom-url field.
	  MAYBE_DONE; // ignore copyfrom-rev field.
	  MAYBE_DONE;
	  svn_file.deleted_ = line; // copy deleted field.
	  MAYBE_DONE; // ignore absent field.
	  MAYBE_DONE; // ignore incomplete field.
	  MAYBE_DONE;
	  svn_file.uuid_ = line; // copy uuid field.
	  MAYBE_DONE; // ignore lock-token field.
	  MAYBE_DONE; // ignore lock-owner field.
	  MAYBE_DONE; // ignore lock-comment field.
	  MAYBE_DONE; // ignore lock-creation-date field.
	  MAYBE_DONE; // ignore changelist field.
	  GO_TO_NEXT_ENTRY;

	done:
	  if (proc_subdirs && svn_file.kind_ == "dir" && !svn_file.name_.isEmpty()) {
	    Name2SvnFileMap sub_data;
	    C_RelativePath subDir(dir, svn_file.name_);
	    fillEntries(sub_data, svn_dir, subDir, false);
	    if (sub_data.find("") != sub_data.end()) {
	      QString tmp = svn_file.name_;
	      svn_file = sub_data[""];
	      svn_file.name_ = tmp;

	      //test for modif. files in directory
	      Name2SvnFileMap::iterator iter_sub;
	      bool was_modif = false;

	      for(iter_sub = sub_data.begin(); iter_sub != sub_data.end(); ++iter_sub) {
		C_SvnFile cur_f = iter_sub->second;
		if (cur_f.kind_ == "file"  && cur_f.is_svn_controled_ && !cur_f.is_symlink_ && !cur_f.name_.isEmpty() && cur_f.modif_dtm_ != cur_f.text_dtm_) {
		  was_modif = true;
		  cout << cur_f.modif_dtm_.toString() << cur_f.text_dtm_.toString();
		  break;
		}
	      }

	      svn_file.is_modif_ = was_modif;

	      if (!was_modif) {
		//cout << "up-2-date:" << svn_file.name_ << endl;
		C_RelativePath curr_entry(dir, svn_file.name_);
		QFileInfo fi( curr_entry.FullPath() );
		if( fi.exists() ) {
		  // Note that fi may refer to a directory even if svn_file.name_ is empty
		  // in that case data from subdirectory will update already stored item
		  svn_file.text_dtm_ = fi.lastModified();
		  svn_file.is_symlink_ = fi.isSymLink();
		}
	      }

	    }
	  }

	  // Do not load times for item referring to current directory
	  if (!svn_file.name_.isEmpty()) {
	    QString f_name = svn_file.name_;
	    if (!dir.FullPath().isEmpty())
	      f_name = dir.FullPath() +"/" + f_name;

	    QFileInfo fi(f_name );
	    if( fi.exists() ) {
	      svn_file.modif_dtm_ = fi.lastModified();
	      svn_file.size_ = fi.size();
	      svn_file.is_symlink_ = fi.isSymLink();
	    }
	    else {
	      //cout << "not here: " <<  f_name << endl;
	    }
	  }

	  if (!svn_file.deleted_.isEmpty())
	    svn_file.is_svn_controled_ = (svn_file.deleted_.lower() == "false");
	  else{	
	    svn_file.is_svn_controled_ = true; 		

	    if (svn_file.revision_.isEmpty()) {
	      svn_file.revision_ = base_revision;
	    }
	  }

	  data[svn_file.name_] = svn_file;
        }
        entries_file.close();
	return true;
    }
  else
    {
      error_line = 0;
      error_column = 0;
      error_message = "can't open file.";
      return false;
    }
}

bool C_SvnEntries::tryEntriesXML(Name2SvnFileMap &data, QFile &entries_file, QString &error_message, int &error_line, int &error_column, const QString& svn_dir, const C_Path& dir, bool proc_subdirs)
{
  // Try to read the given file in an XML format.
  QDomDocument dom;
  if( !dom.setContent(&entries_file, &error_message, &error_line, &error_column ) ) {
    entries_file.close();
    return false;
  }

  C_SvnFile svn_file;
  QString base_revision, base_tag, base_branch;

  data.clear();

  QDomElement el;
  QDomNode node;

  // Pass1: load default data from base entry (the one with empty name)

  node = dom.documentElement().firstChild();
  while ( !node.isNull() ) {
    if( node.isElement() ) {
      el = node.toElement();
      if( el.tagName() == "entry" && el.hasAttribute( "name" )) {

	QString element_name = el.attribute( "name" );

	if (element_name.isEmpty()) {
	  // found directory entry with default data
	  base_revision = el.attribute( "revision" );
	  QString base_url = el.attribute( "url");

	  //process tag/branch info
	  if (!base_url.isEmpty())
	    getTagBranch(base_url, base_tag, base_branch);

	  // there is only one base element in entries file
	  break;
	}
      }
    }
    node = node.nextSibling();
  }

  // Pass2: load individual element data and use loaded defaults
  node = dom.documentElement().firstChild();

  while ( !node.isNull() ) {
    if( node.isElement() ) {
      el = node.toElement();
      //qDebug( "reading element: %s: %s", el.tagName().latin1(), (const char*)el.attribute( "name" ).local8Bit() );
      if( el.tagName() == "entry" ) {
	svn_file.name_ = el.attribute( "name" );
	svn_file.kind_ = el.attribute( "kind");

	svn_file.commited_revision_ = el.attribute( "committed-rev");

	svn_file.url_ = el.attribute( "url");

	//process tag/branch info
	if (!svn_file.url_.isEmpty()) {
	  // element switched to other location
	  getTagBranch(svn_file.url_, svn_file.tag_, svn_file.branch_);
	} else {
	  // element at default location
	  svn_file.branch_ = base_branch;
	  svn_file.tag_ = base_tag;
	}

	svn_file.last_author_ = el.attribute( "last-author");
	svn_file.uuid_ = el.attribute( "uuid");
	svn_file.schedule_ = el.attribute( "schedule");
	svn_file.deleted_ = el.attribute( "deleted");


	svn_file.conflict_wrk_ = el.attribute("conflict-wrk");
	svn_file.conflict_old_ = el.attribute("conflict-old");
	svn_file.conflict_new_ = el.attribute("conflict-new");

	svn_file.commited_dtm_ = svn2dtm(el.attribute( "committed-date"));
	svn_file.text_dtm_ = svn2dtm(el.attribute("text-time"));

	if (proc_subdirs && svn_file.kind_ == "dir" && !svn_file.name_.isEmpty()) {
	  Name2SvnFileMap sub_data;
	  C_RelativePath subDir(dir, svn_file.name_);
	  fillEntries(sub_data, svn_dir, subDir, false);
	  if (sub_data.find("") != sub_data.end()) {
	    QString tmp = svn_file.name_;
	    svn_file = sub_data[""];
	    svn_file.name_ = tmp;

	    //test for modif. files in directory
	    Name2SvnFileMap::iterator iter_sub;
	    bool was_modif = false;

	    for(iter_sub = sub_data.begin(); iter_sub != sub_data.end(); ++iter_sub) {
	      C_SvnFile cur_f = iter_sub->second;
	      if (cur_f.kind_ == "file"  && cur_f.is_svn_controled_ && !cur_f.is_symlink_ && !cur_f.name_.isEmpty() && cur_f.modif_dtm_ != cur_f.text_dtm_) {
		was_modif = true;
		break;
	      }
	    }

	    svn_file.is_modif_ = was_modif;

	    if (!was_modif) {
	      //cout << "up-2-date:" << svn_file.name_ << endl;
	      C_RelativePath curr_entry(dir, svn_file.name_);
	      QFileInfo fi( curr_entry.FullPath() );
	      if( fi.exists() ) {
		// Note that fi may refer to a directory even if svn_file.name_ is empty
		// in that case data from subdirectory will update already stored item
		svn_file.text_dtm_ = fi.lastModified();
		svn_file.is_symlink_ = fi.isSymLink();
	      }
	    }

	  }
	}

	// Do not load times for item referring to current directory
	if (!svn_file.name_.isEmpty()) {
	  QString f_name = svn_file.name_;
	  if (!dir.FullPath().isEmpty())
	    f_name = dir.FullPath() +"/" + f_name;

	  QFileInfo fi(f_name );
	  if( fi.exists() ) {
	    svn_file.modif_dtm_ = fi.lastModified();
	    svn_file.size_ = fi.size();
	    svn_file.is_symlink_ = fi.isSymLink();
	  }
	  else {
	    //cout << "not here: " <<  f_name << endl;
	  }
	}

					
	if (!svn_file.deleted_.isEmpty())
	  svn_file.is_svn_controled_ = (svn_file.deleted_.lower() == "false");
	else{	
	  svn_file.is_svn_controled_ = true; 		

	  svn_file.revision_ = el.attribute( "revision" );
	  if (svn_file.revision_.isEmpty()) {
	    svn_file.revision_ = base_revision;
	  }

	}

	data[svn_file.name_] = svn_file;
      }
    }
    node = node.nextSibling();
  }
  return true;
}

void C_SvnEntries::fillEntries(Name2SvnFileMap &data, const QString& svn_dir, const C_Path& dir, bool proc_subdirs)
{
	// Just a sanity check, caller should always provide full path to existing directory
	if (dir.FullPath().isEmpty() || !QFileInfo(dir.FullPath()).exists()) {
		return;
	}

	// Fill entries as follows:
	// 1. Read svn entries file, process existing items and add to data map
	// 2. Read current directory contents, remove versioned items that are
	//    already in map and add the rest as unversioned items

	QString entr_file_name = svn_dir+"/entries";
	entr_file_name = dir.FullPath() + "/" + entr_file_name;

	QFileInfo fi(entr_file_name);

	if (fi.exists()) {
	  QFile entries_file(entr_file_name);
	  QString error_messsage;
	  int error_line, error_column;

	  if (!tryEntriesXML(data, entries_file, error_messsage, error_line, error_column, svn_dir, dir, proc_subdirs)) {
	    if (!tryEntriesText(data, entries_file, error_messsage, error_line, error_column, svn_dir, dir, proc_subdirs)) {
	      qWarning( "%s %s/entries:%d:%d:%s", (const char*)dir.Name().local8Bit(), (const char*)svn_dir.local8Bit(),
			error_line, error_column, (const char*)error_messsage.local8Bit() );
	      return;
	    }
	  }
	}


	//process rest of files (nonversioned)

	QDir files =  QDir(dir.FullPath());

	if (proc_subdirs) 
		files.setFilter(QDir::Files | QDir::Hidden | QDir::Dirs);
	else
		files.setFilter(QDir::Files | QDir::Hidden);

	for (size_t i=0; i<files.count(); i++) {
		QString curr = files[i];
		if (data.find(curr) == data.end()) {

			if (curr == "." || curr == ".." || curr == svn_dir) 
				continue;

			C_SvnFile svn_file;
			svn_file.is_svn_controled_ = false;
			svn_file.file_name_ = curr;
			svn_file.name_ = curr;

			C_RelativePath curr_entry(dir, svn_file.name_);
			QFileInfo fi(curr_entry.FullPath());
			if( fi.exists() ) {
				svn_file.modif_dtm_ = fi.lastModified();
				svn_file.size_ = fi.size();
				if (fi.isDir() )
					svn_file.kind_ = "dir";
				else
					svn_file.kind_= "file";
				svn_file.is_symlink_ = fi.isSymLink();
			}
			data[svn_file.name_] = svn_file;
		}
	}

}




//2004-07-23T18:46:48.124692Z
inline time_t full_minutes_rounded(time_t val)
{
	return ((val + 60/2) / 60)*60;
}

time_t ISO8601toTime_t(const QString& isoDateTime)
{
	struct tm m;

	memset(&m, 0, sizeof(m));

	m.tm_year = isoDateTime.mid(0,4).toInt() - 1900;
	m.tm_mon  = isoDateTime.mid(5,2).toInt() - 1;
	m.tm_mday = isoDateTime.mid(8,2).toInt();

	m.tm_hour = isoDateTime.mid(11,2).toInt();
	m.tm_min  = isoDateTime.mid(14,2).toInt();
	m.tm_sec  = 0;

	int gm_sec  = isoDateTime.mid(17,2).toInt();

	time_t utc_time;

#if defined(FreeBSD)

	// BSD has inverse of gmtime()
	utc_time = timegm(&m);

#else

	// all others do mktime() and adjust timezone
	// mktime assumes broken-down time in local timezone,
	// to get UTC we need to correct result by timezone offset

	utc_time = mktime(&m);

#if (_WINDOWS || ESVN_WIN )
	utc_time -= _timezone;
#else
	utc_time -= timezone;
#endif

#endif

	// round to remove leap seconds in "right" timezones
	utc_time = full_minutes_rounded(utc_time) + gm_sec;

	return utc_time;
}

QDateTime C_SvnEntries::svn2dtm(const QString& value)
{
    QDateTime ret;
	if (value.length() < 20) {
		//qWarning( "wrong time value: %s", value.local8Bit() );
		return ret;
	}

	ret.setTime_t(ISO8601toTime_t(value));
	return ret;
}

void C_SvnEntries::getTagBranch(const QString& url, QString& tag, QString& branch)
{

	tag = "";
	branch = "";

	int pos = url.find("/tags/");
	if (pos != -1) {
		int fn_pos = url.find("/", pos+6);
		if (fn_pos == -1) 
			fn_pos = url.length();
		tag = url.mid(pos+6, fn_pos - (pos+6));
		//cout <<"tag:"<<tag << endl;;
	}
	else {
		pos = url.find("/branches/");
		if (pos != -1) {
			int fn_pos = url.find("/", pos+10);
			if (fn_pos == -1) 
				fn_pos = url.length();
			branch = url.mid(pos+10, fn_pos - (pos+10));
		}
	}
}

bool C_SvnEntries::cmp(C_SvnEntries sec)
{
	bool cmp_res = true;

	//cout << "cmp: " << getData().size() << " " << sec.getData().size() << endl;

	if (getData().size() == sec.getData().size()) {
		Name2SvnFileMap::iterator iter_svn;
		for (iter_svn = getData().begin(); iter_svn != getData().end();  iter_svn++) {

			if (sec.getData().find(iter_svn->first) == sec.getData().end()) {
				cmp_res = false;
				//cout << "diff names" << endl;
				break;
			}

			C_SvnFile svn_1 = sec.getData()[iter_svn->first];
			C_SvnFile svn_2 = getData()[iter_svn->first];

			if (svn_1.name_ != svn_2.name_ || svn_1.modif_dtm_ != svn_2.modif_dtm_) {
				cmp_res = false;
				//cout << "diff data" << endl;
				break;
			}
		}
	}
	else {
		cmp_res = false;
		//cout << "diff size " << getData().size() << " " <<sec.getData().size() << endl;
	}

	return cmp_res;
}

C_SvnFile::E_Status C_SvnFile::getStatus()
{
	C_SvnFile::E_Status result = UpToDate;

	if (!is_svn_controled_)
		return C_SvnFile::NonSvn;

	if (schedule_ == "add")
		return C_SvnFile::Add;

	if (schedule_ == "delete")
		return C_SvnFile::Delete;

	if (conflict_wrk_ != "")
		return C_SvnFile::Conflict;
		
	if ( (kind_ == QString("dir") && is_modif_) || (kind_ == QString("file") &&  modif_dtm_ != text_dtm_) )
		return C_SvnFile::Modify;

	return result;
}

