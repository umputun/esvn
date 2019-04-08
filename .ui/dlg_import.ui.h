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

void dlg_import::bt_brow_clicked()
{
	QFileDialog* fd = new QFileDialog( this, "Import from ...", TRUE );
	fd->setMode(QFileDialog::DirectoryOnly);
	if (fd->exec() == QDialog::Accepted) 
		le_path->setText(fd->selectedFile());
}
