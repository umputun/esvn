/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void dlg_commit::init()
{
	cb_commit_hist->setDuplicatesEnabled(false);
	loadCommitMessages();

 if(cb_commit_hist->count() > 0)
  msgEdit->setText(cb_commit_hist->text(0));
}

void dlg_commit::bt_clear_clicked()
{
 msgEdit->clear();
}


void dlg_commit::loadCommitMessages()
{
 QSettings settings;
 settings.setPath("umputun", "esvn", QSettings::User);
 settings.beginGroup("/esvn");
 cb_commit_hist->insertStringList(settings.readListEntry("/messages"));
 settings.endGroup();
}


void dlg_commit::saveCommitMessages()
{
 /* build a list of unquie strings */
 QStringList msgs;

 int count = cb_commit_hist->count();
 if(count > 14){ 
  cb_commit_hist->removeItem(count - 1);
  count = 14;
 }

 while(count--)
  msgs.push_front(cb_commit_hist->text(count));

 QString str = msgEdit->text();
 if(msgs.findIndex(str) == -1 && !str.isEmpty()){
  cb_commit_hist->insertItem(str, 0);
  msgs.push_front(str);
 }

 QSettings settings;
 settings.beginGroup("/esvn");
 settings.setPath("umputun", "esvn", QSettings::User);
 settings.writeEntry("/messages", msgs);
 settings.endGroup();
}


void dlg_commit::setCommitPath( const QString & path )
{
}


void dlg_commit::selectAll()
{

}


void dlg_commit::deselectAll()
{

}
