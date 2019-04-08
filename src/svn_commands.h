/***************************************************************************
 *   Copyright (C) 2004 by Eugene Bort                                         *
 *   esvn@umputun.com                                                     *
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


#ifndef SVN_COMMANDS_H
#define SVN_COMMANDS_H

#if ( _WINDOWS || ESVN_WIN )
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <qobject.h>
#include <qtextbrowser.h> 
#include <qprocess.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <cstdlib>
#include <iostream>
#include <qdialog.h> 
#include <qlabel.h> 
#include <qtextcodec.h>
#include <qapplication.h> 
#include <qlineedit.h> 
#include <qcursor.h> 
#include <qeventloop.h>

#include "dlg_busy.h"
#include "dlg_auth.h"

#include "options.h"

#include "esvn_utils.h"

#include <vector>
using namespace std;

class C_SvnCommands : public QWidget {

    Q_OBJECT

    public:

    enum {SC_WAIT = 0x0001, SC_APPEND_USER_PASS=0x0002, SC_HIDE=0x0004, 
          SC_DISABLE_CURSOR_CHANGE=0x0008, SC_REFRESH = 0x0010};

    C_SvnCommands(QTextBrowser* out_win, QWidget *parent=NULL) : 
        needs_refresh_(false), out_win_(out_win), show_enable_(true), parent_(parent) {
        dlg_busy_ = new dlg_busy(this);
        dlg_auth_ = new dlg_auth(this);
        proc = new QProcess(this);
        svn_ = "svn";
        is_ready_for_last_command_ = false;

        user_ = ""; passw_ = "";

        connect(proc, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()) );
        connect(proc, SIGNAL(readyReadStderr()), this, SLOT(readFromStderr()) );
        connect(proc, SIGNAL(processExited()), this, SLOT(procFinished()) );
    };

    int update(const PathSelection& selection, const QString& revision = "") {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();

        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("update");

        if (revision != "") {
            proc->addArgument("-r");
            proc->addArgument(revision);
        }

        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int log(const C_Path& file_dir_name, bool is_verbouse, bool stop_on_copy, QStringList& out_txt)
    {
        // Note that svn log command supports multiple arguments
        // but there seems to be no use for this feature. -- garrison
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("log");

        if (is_verbouse)
            proc->addArgument("--verbose");
        if (stop_on_copy)
            proc->addArgument("--stop-on-copy");

        proc->addArgument(file_dir_name.Name());


        return execCommand(file_dir_name.Path(), SC_APPEND_USER_PASS, &out_txt);
    }

    int status(const PathSelection& selection) {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("status");
        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());
        proc->addArgument("--verbose");
        return execCommand(currwd, SC_APPEND_USER_PASS | SC_WAIT);
    }

    int status(const C_Path& file_dir_name, bool show_updates, QStringList& out_txt) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("status");
        proc->addArgument(file_dir_name.Name());
        proc->addArgument("--quiet");
        if (show_updates) 
            proc->addArgument("--show-updates");

        return execCommand(file_dir_name.Path(), SC_APPEND_USER_PASS | SC_WAIT, &out_txt);
    }

    int status_curr(const QString& currwd, const QString& file_dir_name, QStringList& out_txt) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("status");
        proc->addArgument(file_dir_name);
        proc->addArgument("-v");
        proc->addArgument("-N");
        proc->addArgument("--show-updates");

        return execCommand(currwd, SC_APPEND_USER_PASS | SC_WAIT | SC_DISABLE_CURSOR_CHANGE | SC_HIDE, &out_txt);
    }

    int status(const QString& currwd, const QString& file_dir_name, QStringList& out_txt) {
        //cout << "status: " << file_dir_name << endl;
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("status");
        proc->addArgument(file_dir_name);
        proc->addArgument("--verbose");
        return execCommand(currwd, SC_APPEND_USER_PASS | SC_WAIT | SC_DISABLE_CURSOR_CHANGE, &out_txt);
    }

    int info(const PathSelection& selection, QStringList& out_txt) {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();

        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("info");
        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());
        return execCommand(currwd, 0, &out_txt);
    }

    int commit(const QString& currwd, const QString& file_dir_name, QString msg) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("commit");

        if (msg == "")  msg = "nomsg";

        proc->addArgument("-m"); proc->addArgument(msg);
        proc->addArgument(file_dir_name);

        return execCommand(currwd, SC_REFRESH | SC_WAIT | SC_APPEND_USER_PASS);
    }

    int commit(const QString& currwd, const vector<QString>& files_dir, QString msg) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("commit");

        if (msg == "") msg = "nomsg";

        proc->addArgument("-m"); proc->addArgument(msg);

        for (size_t i=0; i<files_dir.size(); i++) 
            proc->addArgument(files_dir[i]);

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int cleanup(const C_Path& file_dir_path) {
        proc->clearArguments();
        proc->addArgument(svn_);
        proc->addArgument("cleanup");
        proc->addArgument(file_dir_path.Name());
        return execCommand(file_dir_path.Path(), SC_WAIT);
    }

    int revert(const PathSelection& selection, bool recursive) {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();

        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("revert");
        if (recursive) 
            proc->addArgument("--recursive");

        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());
        return execCommand(currwd, SC_REFRESH | SC_WAIT);
    }

    int resolve(const C_Path& pathname) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("resolved");
        proc->addArgument(pathname.Name());
        return execCommand(pathname.Path(), SC_REFRESH | SC_WAIT);
    }

    int add(const PathSelection& selection, bool recursive) {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();

        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("add");

        if (!recursive)
            proc->addArgument("-N");

        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());

        return execCommand(currwd, SC_REFRESH | SC_WAIT);
    }


    int del(const PathSelection& selection) {
        if (selection.empty()) {
            return 1;
        }

        QString currwd = selection[0].Path();
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("delete");

        for (size_t i=0; i<selection.size(); i++) 
            proc->addArgument(selection[i].Name());

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int copy(const QString& currwd, const QString& src, const QString &dst, const QString& msg, const QString& rev="") {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("copy");
        proc->addArgument(src);
        proc->addArgument(dst);

        proc->addArgument("-m");
        if (msg == "")
            proc->addArgument("\"copied\"");
        else
            proc->addArgument(msg);

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }


    int move(const QString& currwd, const QString& src, const QString &dst, const QString& msg, const QString& rev="") {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("move");
        proc->addArgument(src);
        proc->addArgument(dst);

        proc->addArgument("-m");
        if (msg == "")
            proc->addArgument("\"moved\"");
        else
            proc->addArgument(msg);

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int switch_cmd(const C_Path& selected, const QString& url, const QString &rev, bool non_recursive) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("switch");
        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
        if (non_recursive)
            proc->addArgument("--non-recursive");

        proc->addArgument(url);
        proc->addArgument(selected.Name());
        return execCommand(selected.Path(), SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int merge(const QString& currwd, const QString& file_dir_name, const QString& first, const QString &second, const QString& wcpath, bool use_rev, bool non_recursive, bool dryrun) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("merge");

        if (non_recursive)
            proc->addArgument("--non-recursive");
        if (dryrun)
            proc->addArgument("--dry-run");

        if (use_rev) {
            proc->addArgument("-r");
            proc->addArgument(first+":"+second);
            proc->addArgument(file_dir_name);
        }
        else {
            proc->addArgument(first);
            proc->addArgument(second);
        }
        if (!wcpath.isEmpty()) 
            proc->addArgument(wcpath);

        return execCommand(currwd, SC_REFRESH | SC_WAIT);
    }

    int diff(const QString& currwd,
             const QString& file_dir_name_1, const QString& file_dir_name_2, 
             const QString& rev, bool use_external_cmd, bool non_recursive, bool notice_ancestry, 
             const QString& extensions,  QStringList& out_txt);


    int cat(const C_Path& selected, const QString& rev, QStringList& out_txt) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("cat");

        proc->addArgument(selected.Name());

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }

        return execCommand(selected.Path(), SC_APPEND_USER_PASS, &out_txt);
    }

    int blame(const C_Path& selected, const QString& rev, QStringList& out_txt) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("blame");
        proc->addArgument(selected.Name());

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
//      proc->addArgument("--non-interactive");

        return execCommand(selected.Path(), SC_APPEND_USER_PASS, &out_txt);
    }


    int checkout(const QString& currwd, const QString& url, const QString& local_path, const QString& user, const QString& passw, const QString &rev, bool non_recursive) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("checkout");


        proc->addArgument(url);
        proc->addArgument(local_path);

        if (user != "") {
            proc->addArgument("--username");
            proc->addArgument(user);
        }

        if (passw != "") {
            proc->addArgument("--password");
            proc->addArgument(passw);
        }

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
        if (non_recursive)
            proc->addArgument("--non-recursive");

        proc->addArgument("--non-interactive");

        return execCommand(currwd, SC_REFRESH | SC_WAIT);
    }

    int export_cmd(const QString& currwd, const QString& url, const QString& local_path, const QString &rev) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("export");

        proc->addArgument(url);
        proc->addArgument(local_path);

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int import(const QString& currwd, const QString& url, const QString& local_path, QString msg,  bool non_recursive) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("import");

        proc->addArgument(local_path);
        proc->addArgument(url);



        if (msg == "")  msg = "nomsg";
        proc->addArgument("-m"); 
        proc->addArgument(msg);

        if (non_recursive)
            proc->addArgument("--non-recursive");

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int mkdir(const QString& currwd, const QString& dir) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("mkdir");
        proc->addArgument(dir);

        return execCommand(currwd, SC_REFRESH | SC_APPEND_USER_PASS | SC_WAIT);
    }

    int proplist(const C_Path& file_dir_name, const QString& rev, QStringList& out_txt) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("proplist");
        proc->addArgument(file_dir_name.Name());

        proc->addArgument("--verbose");

        if (!rev.isEmpty()) {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }


        return execCommand(file_dir_name.Path(), SC_WAIT | SC_APPEND_USER_PASS, &out_txt);
    }

    int propset(const C_Path& file_dir_name, const QString& rev, const QString& key, const QString &value) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("propset");
        proc->addArgument(key); proc->addArgument(value);


        proc->addArgument(file_dir_name.Name());

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }

        return execCommand(file_dir_name.Path(), SC_WAIT | SC_APPEND_USER_PASS);
    }

    int propdel(const C_Path& file_dir_name, const QString& rev, const QString& key) {
        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("propdel");
        proc->addArgument(key);  proc->addArgument(file_dir_name.Name());

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
        return execCommand(file_dir_name.Path(), SC_WAIT | SC_APPEND_USER_PASS);
    }

    int list(const QString& currwd, const QString& url, const QString& rev, QStringList& out_txt, bool is_hide=false, bool recursive = true, bool dont_change_cursor = false) {

        //cout << "list:" << (url.isEmpty() ? "current" : url.ascii()) << " curs:" << dont_change_cursor << endl;

        proc->clearArguments();
        proc->addArgument(svn_); proc->addArgument("list");
        proc->addArgument(url);

        if (recursive)
            proc->addArgument("--recursive");

        proc->addArgument("--verbose");

        if (rev != "") {
            proc->addArgument("-r");
            proc->addArgument(rev);
        }
        return execCommand(currwd, SC_WAIT | SC_APPEND_USER_PASS | (dont_change_cursor ? SC_DISABLE_CURSOR_CHANGE : 0) | (is_hide ? SC_HIDE : 0), &out_txt);
    }

    int command(const QString& currwd, QStringList cmd) {
        proc->clearArguments();

        for (size_t i=0; i<cmd.size(); i++)
            proc->addArgument(cmd[i]);

        return execCommand(currwd, 0);
    }


    void setSvn(QString svn) {
        svn_ = svn;
    }

public slots:
    void readFromStdout() {
        while (proc->canReadLineStdout()) {
            QString src_msg = proc->readLineStdout();
            QString str = QTextCodec::codecForLocale()->toUnicode(src_msg);

            if(!str.isNull()){
                if (show_enable_)
                    out_win_->append(QString("<font color=black>") + str + QString("</font>") );
                out_text_.push_back(str);
            }
        }
    }

    void readFromStderr() {     
        while (proc->canReadLineStderr()) {
            QString str = QTextCodec::codecForLocale()->toUnicode(proc->readLineStderr());

            //if (!is_hide_all_)
            out_win_->append(QString("<font color=red>") + str + QString("</font>") );

            if (!disable_cursor_change_) 
                QApplication::restoreOverrideCursor();
/*
            //auth 
            if (str.find("Username:") != -1) {
                proc->writeToStdin(dlg_auth_->le_user->text() + "\n");
                out_win_->append(dlg_auth_->le_user->text());
            }

            if (str.find("Password for ") != -1) {
                proc->writeToStdin(dlg_auth_->le_passw->text() + "\n");
                out_win_->append(dlg_auth_->le_user->text());
            }

            if (str.find("Authentication realm:") != -1) {
                cout << "auth:" << str << endl;
                proc->writeToStdin("");
                //sleep(2); 
                dlg_auth_->exec();
                qApp->processEvents();
            }
*/
        }
        
    }

    void procFinished() {
        //cout << "proc finished" << endl;

        parent_->blockSignals(false);   

        if (!disable_cursor_change_) 
            QApplication::restoreOverrideCursor();

        dlg_busy_->close();

        if (!is_hide_all_) {
            int ret_code = proc->exitStatus();
            char ret_code_buff[64];
            if (ret_code == 0)
                sprintf(ret_code_buff, "<font color=darkgreen>successfully (%d)</font>", ret_code);
            else
                sprintf(ret_code_buff, "<font color=darkred>error (%d)</font>", ret_code);

            out_win_->append(ret_code_buff);
        }

        if (is_refreshable_command_)
            needs_refresh_ = true;

        is_ready_for_last_command_ = true;        
    }

public:
    bool needs_refresh_;

    void setUserPassw(const QString& user,const QString& passw) {
        user_ = user;
        passw_ = passw;
    }

    bool isRunning() {
        return proc->isRunning();
    }

    QString getLastCommand() {
        if (is_ready_for_last_command_) {
            QStringList args = proc->arguments();
            if (args.empty())
                return "";
            else {
                is_ready_for_last_command_ = false;
                return args[1];
            }
        }
        else
            return "";
    }

private:
//  int execCommand(bool is_refreshable_command, QStringList* out_list=NULL, 
//                  bool wait=false, bool append_user_passw = true, 
//                  bool hide=false, bool disable_cursor_change = false ) {

// de
//  enum {SC_WAIT = 0x0001, SC_APPEND_USER_PASS=0x0002, SC_HIDE=0x0004, 
          //SC_DISABLE_CURSOR_CHANGE=0x0008, SC_REFRESH = 0x0010};

     int execCommand(const QString& currwd, unsigned int flags, QStringList* out_list=NULL) {

        disable_cursor_change_ = (flags & SC_DISABLE_CURSOR_CHANGE);

        if (!disable_cursor_change_) 
            QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

        is_refreshable_command_ = (flags & SC_REFRESH);

        if (flags & SC_APPEND_USER_PASS) {
            if (!user_.isEmpty()) {
                proc->addArgument("--username");
                proc->addArgument(user_);
            }
            if (!passw_.isEmpty()) {
                proc->addArgument("--password");
                proc->addArgument(passw_);
            }
            proc->addArgument("--non-interactive");
        }

        is_hide_all_ = (flags & SC_HIDE) && !isVerbouse();

        if (!is_hide_all_ ) {
            QString svn_cmd = showCommand();
            dlg_busy_->lb_command->setText(svn_cmd);
            //dlg_busy_->show();
        }

        show_enable_ = (!out_list && !is_hide_all_ && !isVerbouse());
        

        out_text_.clear();
        qApp->processEvents();

        // set correct working directory for spawned process,
        // as most of provided file paths are relative to this directory
        if (currwd.isEmpty()) {
            abort();
        }

        proc->setWorkingDirectory(currwd);

        if (!proc->start()) {
            out_text_.append("can't fork the process");

            if (!disable_cursor_change_) 
                QApplication::restoreOverrideCursor();

            return 1;
        }

        parent_->blockSignals(true);
        if (out_list || (flags & SC_WAIT) ) {
//          if (parent_) parent_->setDisabled(true);

            while (proc->isRunning()) {
                //cout <<"running " << proc->processIdentifier() << endl; //sleep(1);
                qApp->processEvents(QEventLoop::ExcludeUserInput);

#if ( _WINDOWS || ESVN_WIN )
                Sleep(1000);
#else
                usleep(1000);
#endif
            }

            if (!disable_cursor_change_) 
                QApplication::restoreOverrideCursor();

//          if (parent_) parent_->setDisabled(false);
        }

        if (out_list)
            *out_list = out_text_;

        show_enable_ = true;

        int ret_code = proc->exitStatus();
        return ret_code;
    }

    QString showCommand() {

        QStringList args = proc->arguments();
        QStringList::Iterator it = args.begin();
        QString out_str = "<font color=navy>";
        QString curr_str, prev_arg;
        int arg_num =0;
        while ( it != args.end() ) {
            if (arg_num && prev_arg == QString("--password") )
                curr_str = "******";
            else
                curr_str = (*it);
                
            if (arg_num++ == 1)
                out_str = out_str + curr_str + "</font> ";
            else
                out_str = out_str + curr_str + " ";
            ++it;
            
            prev_arg = curr_str;
        }
        out_win_->append(out_str);
        return out_str;
    }

    bool isVerbouse() {
        //cout << "verb:" << C_Options::instance()->dlgOptions()->rb_log_verb->isChecked() << endl;
        return C_Options::instance()->dlgOptions()->rb_log_verb->isChecked();
    }

private:
    QProcess *proc;

    dlg_busy *dlg_busy_;
    QTextBrowser* out_win_;
    QStringList out_text_;
    bool show_enable_, is_hide_all_;
    bool is_refreshable_command_, is_ready_for_last_command_;

    QString svn_;
    dlg_auth *dlg_auth_;
    QString user_, passw_;
    QWidget *parent_;

    QString diff_cmd_;

    bool disable_cursor_change_;
};

#endif

