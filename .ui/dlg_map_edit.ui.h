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

void dlg_map_edit::tb_brw_clicked()
{
 QFileDialog* fd = new QFileDialog(this, "Select file (action)", TRUE );
 fd->setMode(QFileDialog::Directory);
 if (fd->exec() == QDialog::Accepted) {
  QString path = fd->selectedFile();
  le_act->setText(path);
 }
}
