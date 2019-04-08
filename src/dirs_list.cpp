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
  
#include "dirs_list.h"
#include "esvn_utils.h"

bool C_DirsList::load(const C_Path& path, QListView* list_dirs, C_DirViewItem* root, bool open, bool disable_symlinks, bool append) 
{

//  if (!path.FullPath().isEmpty()) {
//      QFileInfo fi(path.FullPath());
//          if(!fi.exists() ) 
//              return false;
//  }

    //disable colors
    svn_status_ = NULL;

    if (!root)  {//wihtout parent it means top-level directory
        if (!append) 
            list_dirs->clear();

		root = new C_DirViewItem(list_dirs);

		//root_item->setPixmap(0, QPixmap::fromMimeSource( "folder_home_2.png" ) );
		root->setPixmap(0, QPixmap::fromMimeSource("folder_green_open.png")); //colorscm.png") );

		root->SetName(path.Name());
		root->SetPath(path.Path());

		load(path, list_dirs, root);
		//root->setOpen(TRUE);
		return true;
	}

    C_SvnEntries svn_entries(svn_dir_);
    //cout << "path:" << path << endl;
    svn_entries.setDir(path, true); //force "show dirs"


    //performs open/close and change folder icon
	if (root->childCount() >0 && root->firstChild()->text(EsvnLVI::ITEM_WORKING_DIRECTORY) != "empty item") {
		C_SvnFile svn_file_info = svn_entries.get(root->FullPath());
		root->setPixmap(0,getIcon(root->FullPath(), root->isOpen(), &svn_file_info) );
		return true;
	}


    if (root->childCount() == 1 && root->firstChild()->text(EsvnLVI::ITEM_WORKING_DIRECTORY) == "empty item")
        root->takeItem(root->firstChild());

    QDir workspace_dir = QDir(root->FullPath());
    workspace_dir.setFilter(QDir::Dirs | QDir::Hidden | (disable_symlinks ? QDir::NoSymLinks : 0) );

    if (!workspace_dir.exists() ) {
        qWarning( ("Cannot find " + path.FullPath()).local8Bit());
        return false;
    }

    for (size_t i=0; i<workspace_dir.count(); i++) {

        QString curr = workspace_dir[i];
        if ( curr == "." || curr == ".." || curr == svn_dir_)
            continue;

        C_SvnFile svn_file_info = svn_entries.get(curr);

        C_DirViewItem *item;
        C_DirPath full_path;

        // note full path is set in each case later
        full_path.SetName(curr);

		full_path.SetPath(root->FullPath());

		item = new C_DirViewItem(root);

		root->setOpen(open);
		root->setPixmap(0, getIcon(root->FullPath(), open, &svn_file_info) );


		if (hasSubDirs(full_path.FullPath(), disable_symlinks)) {
			C_DirViewItem *sub_item = new C_DirViewItem(item);
			sub_item->SetPath("empty item");
		}

        item->SetPath(full_path.Path());
        item->SetName(full_path.Name());

        item->setPixmap(0, getIcon(item->FullPath(), false, &svn_file_info) );
    }


    return true;
}

QPixmap C_DirsList::getIcon(const QString& dir_name, bool is_open,  C_SvnFile *svn_file_info)
{

    //cout << "dir:" << dir_name << endl; 
    QPixmap result = QPixmap::fromMimeSource("folder_green.png");;

    if (isSvnControlled(dir_name)) {
        if (is_open) 
            result = QPixmap::fromMimeSource("folder_green_open.png");
        else         
            result = QPixmap::fromMimeSource("folder_green.png");
    }
    else {
        if (is_open) 
            result = QPixmap::fromMimeSource("folder_grey_open.png");
        else         
            result = QPixmap::fromMimeSource("folder_grey.png");
    }

    if (!svn_status_ || !svn_file_info)     //if revision's testing disabled
        return result;

    //revision list defined (fully colorized)
    if (isSvnControlled(dir_name)) {

        if (svn_file_info->modif_dtm_ == svn_file_info->text_dtm_) {
            QString full_name = svn_file_info->name_;
            if (!dir_name.isEmpty()) 
                full_name = dir_name ;//+ "/" + svn_file_info->name_;
            //cout << "full name: " << full_name << " " << svn_file_info->name_ << endl;
                
            if (svn_status_->getData()->find(full_name) != svn_status_->getData()->end()) {
                if (svn_status_->getData()->find(full_name)->second.last_ch_rev_ != svn_file_info->commited_revision_) {
                    result = QPixmap::fromMimeSource("folder_exec_3.png");
                    // cout << "out-of-date : " << rev_list_->find(full_name)->second  << " " << svn_file_info->commited_revision_ << endl;
                    return result;
                }
            }
        }
        else {
            result = QPixmap::fromMimeSource( "folder_red.png"); //modified
            //cout << "modified" << endl;
        }

        if (svn_file_info->schedule_ == "add") 
            result = QPixmap::fromMimeSource("folder_orange.png");

        if (svn_file_info->schedule_ == "delete") 
            result = QPixmap::fromMimeSource( "folder_red_3.png");
    }
    //else
        //cout << "non-svn" << endl;


    return result;
}



bool C_FilesList::load(const C_Path& path, QListView* list_files, bool hide_unknown, bool flat_view, bool show_changed_items_only, bool show_dirs, C_SvnStatus* svn_status) 
{
    list_files->clear();

    return load_real(QString(""), path, list_files, hide_unknown, flat_view, show_changed_items_only, show_dirs, svn_status);
}

bool C_FilesList::load_real(const QString& rel_path_prefix, const C_Path& base_path, QListView* list_files, bool hide_unknown, bool flat_view, bool show_changed_items_only, bool show_dirs, C_SvnStatus* svn_status) 
{
    C_RelativePath path(base_path,rel_path_prefix);
    path.SetIsDir(true);

    QDir files =  QDir(path.FullPath());

    files.setFilter(QDir::Files | QDir::Hidden | QDir::Dirs );

    if (!files.exists() ) {
        qWarning( ("Cannot find the " + path.FullPath()).local8Bit());
        return false;
    }

    C_SvnEntries curr_svn_entries;
    curr_svn_entries.setSvnDir(svn_dir_);
    curr_svn_entries.setDir(path, show_dirs);

    for (size_t i=0; i<files.count(); i++) {

        QString curr = files[i];
        if (curr == "." || curr == ".." || curr == svn_dir_) 
            continue;

        C_SvnFile svn_file_info = curr_svn_entries.get(curr);

        if (!svn_file_info.is_svn_controled_ && hide_unknown) 
            continue;

        // Remove name from current entries to catch objects missing from this directory later
        curr_svn_entries.erase(curr);


        QString curr_rel_path = C_Path::Concat(rel_path_prefix, curr);

        if (show_dirs || svn_file_info.kind_ != QString("dir")) {
            C_NumSortedFileListItem* item = new C_NumSortedFileListItem(base_path, curr_rel_path, list_files, 0);

            if (svn_file_info.is_svn_controled_) {
                if (svn_file_info.modif_dtm_ == svn_file_info.text_dtm_ ) {
                    C_RelativePath full_name(path, svn_file_info.name_);

                    //if (svn_status && svn_status->getData()->find(full_name) != svn_status->getData()->end() ) {
                    //  cout <<"LIST: " << full_name << " " << svn_status->getData()->find(full_name)->second.status_  << endl;
                    //}

                    if (svn_status && svn_status->getData()->find(full_name.FullPath()) != svn_status->getData()->end()
                         && svn_status->getData()->find(full_name.FullPath())->second.status_ =="out-of-date") {

                        //cout << full_name << " " << svn_status->getData()->find(full_name)->second.last_ch_rev_ << " : " << svn_file_info.commited_revision_ << endl;

                        if (svn_file_info.kind_ == QString("dir") ) {               
                            item->setPixmap(0, QPixmap::fromMimeSource( "folder_exec_3.png")) ;
                            item->SetIsDir(true);
                        } 
                        else
                            item->setPixmap(0, QPixmap::fromMimeSource( "FileNeedsPatch16x16.xpm" ) );

                        item->setText(1, "out-of-date");
                    }
                    else {
                        if (svn_file_info.kind_ == QString("dir") ) {               
                            item->setPixmap(0, QPixmap::fromMimeSource( "folder_green.png")) ;
                            item->SetIsDir(true);
                        } 
                        else 
                            item->setPixmap(0, QPixmap::fromMimeSource( "FileWriteable16x16.xpm" ) );
                        item->setText(1, "up-to-date");
                        item->setVisible(!show_changed_items_only);
                    }
                }
                else {
                    if (svn_file_info.kind_ == QString("dir") ) {               
                        item->setPixmap(0, QPixmap::fromMimeSource( "folder_red.png")) ;
                        item->SetIsDir(true);
                    } 
                    else 
                        item->setPixmap(0, QPixmap::fromMimeSource( "FileModified16x16.xpm" ) );

                    //cout << "modified" << endl;
                    item->setText(1, "modified");
                }

                if (svn_file_info.schedule_ == "add") {
                    if (svn_file_info.kind_ == QString("dir") ) {               
                        item->setPixmap(0, QPixmap::fromMimeSource( "folder_orange.png")) ;
                        item->SetIsDir(true);
                    } 
                    else 
                        item->setPixmap(0, QPixmap::fromMimeSource( "FileAdded16x16.xpm" ) );

                    item->setText(1, "add");
                }


                if (svn_file_info.schedule_ == "delete") {
                    if (svn_file_info.kind_ == QString("dir") ) {               
                        item->setPixmap(0, QPixmap::fromMimeSource( "folder_red_3.png")) ;
                        item->SetIsDir(true);
                    }
                    else
                        item->setPixmap(0, QPixmap::fromMimeSource( "FileRemoved16x16.xpm" ) );

                    item->setText(1, "removed");
                }

                if (svn_file_info.conflict_wrk_ != "") {
                    item->setPixmap(0, QPixmap::fromMimeSource( "FileConflict16x16.xpm" ) );
                    item->setText(1, "conflict");
                }

                if (svn_file_info.is_symlink_) {
                    item->setPixmap(0, QPixmap::fromMimeSource( "FileLocked16x16.xpm" ) );
                    item->setText(1, "symlink");
                }

                item->setText(2, svn_file_info.revision_);
                item->setText(3, svn_file_info.commited_revision_);
                item->setText(4, svn_file_info.commited_dtm_.toString("yy/MM/dd hh:mm:ss"));

                if (svn_file_info.modif_dtm_.isValid() )
                    item->setText(5, svn_file_info.modif_dtm_.toString("yy/MM/dd hh:mm:ss"));

                item->setText(6, svn_file_info.last_author_);

                //cout <<"tb:" <<  svn_file_info.tag_ << " " << svn_file_info.branch_ << endl;
                if (svn_file_info.tag_ != "") {
                    item->setText(7, svn_file_info.tag_);
                    item->setPixmap(7, QPixmap::fromMimeSource("t-16x16.xpm" ) );
                }

                if (svn_file_info.branch_ != "") {
                    item->setText(7, svn_file_info.branch_);
                    item->setPixmap(7, QPixmap::fromMimeSource("b-16x16.xpm" ) );
                }

                if (svn_file_info.branch_ == "" && svn_file_info.tag_ == "")
                    item->setText(7, "trunk");

            }
            else {
                if (svn_file_info.kind_ == QString("dir") ) {
                    item->setPixmap(0, QPixmap::fromMimeSource( "folder_grey.png")) ;
                    item->SetIsDir(true);
                }
                else 
                    item->setPixmap(0, QPixmap::fromMimeSource( "FileUnknown16x16.xpm" ) );

                item->setText(1, "unknown");
            }
        }

        // allow descend to subdirs
        if (flat_view && svn_file_info.kind_ == QString("dir") ) {
            load_real(curr_rel_path, base_path, list_files, hide_unknown, flat_view, show_changed_items_only, show_dirs, svn_status);
        } else {
            // simple fallthrough allows to skip descending to unknown dirs
        }
    }

    //cout << "rest:" << curr_svn_entries.getData().size() << endl;;

    Name2SvnFileMap::iterator iter_entries;
    for (iter_entries = curr_svn_entries.getData().begin(); iter_entries != curr_svn_entries.getData().end(); ++iter_entries) {
        C_SvnFile svn_file_info = iter_entries->second;

        if (!show_dirs && svn_file_info.kind_ != "file") 
            continue;

        if (!svn_file_info.is_svn_controled_ || svn_file_info.name_ == "" || svn_file_info.name_ == "." || svn_file_info.name_ == ".."  || svn_file_info.name_ == svn_dir_  || /*svn_file_info.kind_ != "file" ||*/ svn_file_info.deleted_ == "true") 
            continue;


        QString curr_rel_path = C_Path::Concat(rel_path_prefix, svn_file_info.name_);
        C_NumSortedFileListItem* item = new C_NumSortedFileListItem(base_path, curr_rel_path, list_files, 0);

        if (svn_file_info.schedule_ == "delete") {
            if (svn_file_info.kind_ == QString("dir") ) {               
                item->setPixmap(0, QPixmap::fromMimeSource( "folder_orange.png")) ;
                item->SetIsDir(true);
            }
            else
                item->setPixmap(0, QPixmap::fromMimeSource( "FileRemoved16x16.xpm" ) );

            item->setText(1, "removed");
        }
        else {
            if (svn_file_info.is_svn_controled_) {
                if (svn_file_info.kind_ == QString("dir") ) {               
                    item->setPixmap(0, QPixmap::fromMimeSource( "folder_violet.png")) ;
                    item->SetIsDir(true);
                }
                else
                    item->setPixmap(0, QPixmap::fromMimeSource( "FileNeedsCheckout16x16.xpm" ) );
                item->setText(1, "missing");
            }
        }

        item->setText(2, svn_file_info.revision_);
        item->setText(3, svn_file_info.commited_revision_);

        item->setText(4, svn_file_info.commited_dtm_.toString("yy/MM/dd hh:mm:ss"));

        if (svn_file_info.modif_dtm_.isValid())
            item->setText(5, svn_file_info.modif_dtm_.toString("yy/MM/dd hh:mm:ss"));

        item->setText(6, svn_file_info.last_author_);

        // allow descend to subdirs
        if (flat_view && svn_file_info.kind_ == QString("dir") ) {
            load_real(curr_rel_path, base_path, list_files, hide_unknown, flat_view, show_changed_items_only, show_dirs, svn_status);
        } else {
            // simple fallthrough allows to skip descending to unknown dirs
        }

    }
    return true;
}

C_DirViewItem::C_DirViewItem(QListView *parent, QListViewItem *after)
: QListViewItem(parent, after)
{
}

C_DirViewItem::C_DirViewItem(QListViewItem *parent, QListViewItem *after)
: QListViewItem(parent, after)
{
}

C_DirViewItem::~C_DirViewItem()
{
}

QString C_DirViewItem::text(int column) const
{
    switch (column) {
    case EsvnLVI::ITEM_NAME:
    case EsvnLVI::ITEM_RELATIVE_PATH:
        return Name();
        break;
    case EsvnLVI::ITEM_WORKING_DIRECTORY:
        return Path();
        break;
    case EsvnLVI::ITEM_FULL_PATH:
        return FullPath();
        break;
    default:
        break;
    }

    return QListViewItem::text(column);
}

QString C_NumSortedFileListItem::text(int column) const
{
    switch (column) {
    case EsvnLVI::ITEM_NAME:
    case EsvnLVI::ITEM_RELATIVE_PATH:
        return Name();
        break;
    case EsvnLVI::ITEM_WORKING_DIRECTORY:
        return Path();
        break;
    case EsvnLVI::ITEM_FULL_PATH:
        return FullPath();
        break;
    default:
        break;
    }

    return QListViewItem::text(column);
}

C_Path::C_Path()
{
}

C_Path::~C_Path()
{
}

C_DirPath::C_DirPath()
{
}

C_DirPath::~C_DirPath()
{
}

void C_DirPath::SetName(const QString& name)
{
    name_ = name;
}

QString C_DirPath::Name() const
{
    return name_;
}

void C_DirPath::SetPath(const QString& pathname)
{
    pathname_ = pathname;
}

QString C_DirPath::Path() const
{
    return pathname_;
}

C_DirPath& C_DirPath::operator =(const QDir& path)
{
    QDir baseDir = path;
    if (baseDir.cdUp()) {
        SetPath(QDir::cleanDirPath(baseDir.absPath()));
        SetName(path.dirName());
        //std::cout << "Split path " << path.absPath() << std::endl;
        //std::cout << "      into " << Path() << " : " << Name() << std::endl;
    } else {
        std::cout << "Unable to cdUp() from path " << path.absPath() << std::endl;
    }

    return *this;
}

bool C_DirPath::isDir() const
{
    return true;
}

C_RelativePath::C_RelativePath(const C_Path& parent, const QString& name)
: parent_(parent), name_(name), is_dir_(false)
{
}

C_RelativePath::~C_RelativePath()
{
}

void C_RelativePath::SetIsDir(bool is_dir)
{
    is_dir_ = is_dir;
}

QString C_RelativePath::Name() const
{
    return name_;
}

QString C_RelativePath::Path() const
{
    return parent_.FullPath();
}

bool C_RelativePath::isDir() const
{
    return is_dir_;
}

// Standalone path object implementation

C_PathCopy::C_PathCopy()
{
}

C_PathCopy::C_PathCopy(const C_Path& from)
{
    *this = from;
}

C_PathCopy::~C_PathCopy()
{
}

C_PathCopy& C_PathCopy::operator=(const C_Path& from)
{
    name_ = from.Name();
    pathname_ = from.Path();
    is_dir_ = from.isDir();

    return *this;
}

QString C_PathCopy::Name() const
{
    return name_;
}

QString C_PathCopy::Path() const
{
    return pathname_;
}

bool C_PathCopy::isDir() const
{
    return is_dir_;
}

