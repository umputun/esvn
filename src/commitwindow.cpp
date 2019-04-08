#include "commitwindow.h"
#include "check_lbi.h"
#include "svn_status.h"
#include "entries_parser.h"

#include <qfileinfo.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <queue>

/* 
 *  Constructs a C_CommitWindow which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
C_CommitWindow::C_CommitWindow( C_SvnStatus* status, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : dlg_commit( parent, name, modal, fl ), status_(status)
{
    connect( files_to_commit, SIGNAL( selected(QListBoxItem*) ), this, SLOT( itemToggled(QListBoxItem*) ) );
    connect( files_to_commit, SIGNAL( clicked(QListBoxItem*,const QPoint&) ), this, SLOT( itemClicked(QListBoxItem*,const QPoint&) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
C_CommitWindow::~C_CommitWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

static int mismatch( const QString& s1, const QString& s2 )
{
    uint i;
    for( i = 0 ; i < QMIN(s1.length(), s2.length()); ++i )
        if( s1.at(i) != s2.at(i) )
            break;
    return i;
}

void C_CommitWindow::setCommitPath( const C_DirPath & path, bool commit_list_enabled)
{
    files_to_commit->clear();

	if (!commit_list_enabled) {
		new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileModified16x16.xpm"), path.FullPath(), true);
		files_to_commit->setEnabled( true );
		bt_select_all->setEnabled( true );
		bt_deselect_all->setEnabled( true );
		buttonOk->setEnabled( true );
		return;
	}

//     qDebug( "void C_CommitWindow::setCommitPath( %s )", path.FullPath().latin1() );
//     C_SvnStatus s( cmd_ );
//#define USE_STATUS
#ifdef USE_STATUS
    if( !status_->status( path ) )
        qFatal("Cannot get status!");
    C_SvnStatus::StatusData* d = status_->getData();

    QString basePath = path.FullPath();//QDir(path).absPath();

    for( C_SvnStatus::StatusData::const_iterator i = d->begin(); i != d->end(); ++i ) {

        QString localPath = i->second.file_name_.mid( mismatch(basePath, i->second.file_name_)+1 );

        if( i->second.status_ == "added" )
            new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileAdded16x16.xpm"), localPath, true);
        else if( i->second.status_ == "modified" )
            new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileModified16x16.xpm"), localPath, true);
        else if( i->second.status_ == "deleted" )
            new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileRemoved16x16.xpm"), localPath, true);
    }
#else
    QString basePath = QDir(path.FullPath()).absPath();

    queue<QDir> Q;
    Q.push( QDir(path.FullPath()) );

    while( !Q.empty() ) {
        QDir d = Q.front(); Q.pop();
        d.setFilter( QDir::Dirs|QDir::Files|QDir::Hidden );
//         qDebug("Dir: %s", d.path().latin1());

        C_SvnEntries entries;
        entries.setDir( C_DirPath(d), false );

        QString localPath = d.absPath().mid( mismatch(basePath, d.absPath())+1 );
        if( !localPath.isEmpty() )
            localPath.append( "/" );

        Name2SvnFileMap& data = entries.getData();
        for( Name2SvnFileMap::iterator i = data.begin(); i != data.end(); ++i ) {
            if( i->first.isEmpty() || !i->second.is_svn_controled_ )
                continue;

            QFileInfo fi( d.path() + QDir::separator() +  i->first );
//             qDebug( "fi.filePath() = %s", fi.filePath().latin1() );
            if( fi.isDir() && !fi.isSymLink()  && i->second.getStatus() != C_SvnFile::Delete )
                Q.push( QDir( fi.filePath() ) );


            C_SvnFile file = i->second;

//             qDebug("%s: check status of %s", d.path().latin1(), i->first.latin1());
            switch( file.getStatus() ) {
            case C_SvnFile::Add:
//                 qDebug( "%s is added", (localPath + it.current()->fileName()).latin1() );
                new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileAdded16x16.xpm"), localPath + i->first, true);
                break;
            case C_SvnFile::Modify:
//                 qDebug( "%s is modified", (localPath + it.current()->fileName()).latin1() );
                new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileModified16x16.xpm"), localPath + i->first, true);
//                 qDebug("modified %s", it.current()->filePath().latin1());
                break;
            case C_SvnFile::Delete:
                new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileRemoved16x16.xpm"), localPath + i->first, true);
                break;
            default:
                break;
            }
        }
        files_to_commit->setEnabled( true );
        bt_select_all->setEnabled( true );
        bt_deselect_all->setEnabled( true );
        buttonOk->setEnabled( true );
    }
#endif

    if( !files_to_commit->count() ) {
        files_to_commit->clear();
        new QListBoxText(files_to_commit, tr("Error getting status for dir %1").arg(path.FullPath()));
		new QListBoxText(files_to_commit, tr("or there are no files/directories to commit"));

        files_to_commit->setEnabled( false );
        bt_select_all->setEnabled( false );
        bt_deselect_all->setEnabled( false );
        buttonOk->setEnabled( false );
    }
}

void C_CommitWindow::setSvnDir(const QString& svn_dir)
{
    svn_dir_ = svn_dir;
}

void C_CommitWindow::setCommitFiles( const vector<QString> & files )
{
    qDebug( "void C_CommitWindow::setCommitFiles()" );
    files_to_commit->clear();
    for( vector<QString>::const_iterator i = files.begin(); i != files.end(); ++i )
        new QListBoxCheck(files_to_commit, QPixmap::fromMimeSource("FileModified16x16.xpm"), *i, true);

    files_to_commit->setEnabled( true );
    bt_select_all->setEnabled( true );
    bt_deselect_all->setEnabled( true );
    buttonOk->setEnabled( true );
}

std::vector<QString> C_CommitWindow::commitList() const
{
    std::vector<QString> ret;
    ret.reserve( files_to_commit->count() );

    QListBoxItem* i = files_to_commit->firstItem();
    while( i ) {
        if( i->rtti() == QListBoxCheck::RTTI && ((QListBoxCheck*)i)->isOn() )
            ret.push_back( i->text() );
        i = i->next();
    }

    return ret;
}




void C_CommitWindow::itemToggled( QListBoxItem* i )
{
    if( i && i->rtti() == QListBoxCheck::RTTI ) {
        QListBoxCheck* c = (QListBoxCheck*)i;

        c->setOn( !c->isOn() );
    }
}

void C_CommitWindow::itemClicked( QListBoxItem * i, const QPoint & p )
{
    if( i && i->rtti() == QListBoxCheck::RTTI ) {
        QListBoxCheck* c = (QListBoxCheck*)i;

        if( c->isPointOnCheck(p) )
            c->setOn( !c->isOn() );
    }
}

void C_CommitWindow::selectAll()
{
    QListBoxItem* i = files_to_commit->firstItem();
    while( i ) {
        if( i->rtti() == QListBoxCheck::RTTI )
            ((QListBoxCheck*)i)->setOn( true );
        i = i->next();
    }
}

void C_CommitWindow::deselectAll()
{
    QListBoxItem* i = files_to_commit->firstItem();
    while( i ) {
        if( i->rtti() == QListBoxCheck::RTTI )
            ((QListBoxCheck*)i)->setOn( false );
        i = i->next();
    }
}
