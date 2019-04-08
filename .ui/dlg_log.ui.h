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

void dlg_log::list_log_selectionChanged( QListViewItem * item)
{
 msg_box->clear();
 msg_box->append(item->text(6));
 msg_box->setContentsPos(0, 0);
}

void dlg_log::bt_view_clicked()
{

 
}


void dlg_log::bt_update_clicked()
{

}


void dlg_log::bt_diff_clicked()
{

}


void dlg_log::bt_chpath_clicked()
{

}




void dlg_log::bt_clear_msg_clicked()
{
 cb_msg->setCurrentText("");
}


void dlg_log::bt_clear_author_clicked()
{
 cb_author->setCurrentText("");
}


void dlg_log::bt_apply_clicked()
{
 
}


void dlg_log::list_log_mouseButtonPressed( int, QListViewItem *, const QPoint &, int )
{

}
