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
#include <qfiledialog.h> 
#include <qlistview.h>

void dlg_ws_edit::bt_dir_clicked()
{
 QFileDialog* fd = new QFileDialog( this, "select directory", TRUE );
 fd->setMode(QFileDialog::DirectoryOnly);
 
 if (fd->exec() == QDialog::Accepted) {
  QString path = fd->selectedFile();
  if (path.endsWith("/"))
    path.remove(path.length()-1, 1);
  le_dir->setText(path);
 }
}


void dlg_ws_edit::bt_add_clicked()
{
 if (!le_dir->text().isEmpty()) {
   QListViewItem* item = new QListViewItem(lv_dirs, 0);
    item->setText(0, le_dir->text());
  le_dir->setText("");
 }
}


void dlg_ws_edit::bt_del_clicked()
{
 QListViewItem* item = lv_dirs->selectedItem();
 if (item)
   lv_dirs->takeItem(item);
}
