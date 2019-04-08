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


void dlg_repository::bt_browse_clicked()
{
	
}


void dlg_repository::lv_dirs_selectionChanged( QListViewItem *)
{
	
}


void dlg_repository::lv_dirs_expanded( QListViewItem* item)
{
	item->setPixmap(0, QPixmap::fromMimeSource( "folder_blue_open.png" ));
}


void dlg_repository::lv_dirs_collapsed( QListViewItem* item)
{
	item->setPixmap(0, QPixmap::fromMimeSource( "folder_blue.png" ));
}


void dlg_repository::bt_view_clicked()
{
	
}


void dlg_repository::bt_checkout_clicked()
{
	
}
