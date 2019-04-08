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


void dlg_workspace::bt_add_clicked()
{

}


void dlg_workspace::lv_wslist_selectionChanged( QListViewItem *item)
{
 tx_full->clear();
 QStringList dirs_list = QStringList::split("; ", item->text(1) );
 for (size_t i=0; i<dirs_list.size(); i++)
  tx_full->append(dirs_list[i]);
 tx_full->verticalScrollBar()->setValue(0);
}


void dlg_workspace::bt_delete_clicked()
{
}


void dlg_workspace::bt_edit_clicked()
{

}


void dlg_workspace::bt_rename_clicked()
{

}

