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
#include "dlg_url.h"

void dlg_copy::tb_src_clicked()
{
 if (tb_src->text() == "U") {
  dlg_url url;
  url.le_url_->setText(lb_url->text());
  if (url.exec() == QDialog::Accepted)
   le_source->setText(url.le_url_->text());
 }
 else {
   QFileDialog* fd = new QFileDialog(this, "Select file/directory", TRUE );
   fd->setMode(QFileDialog::ExistingFile);
   if (fd->exec() == QDialog::Accepted) {
    QString path = fd->selectedFile();
	if (path.endsWith("/"))
		path.remove(path.length()-1, 1);
     le_source->setText(path);
    }
 }
}


void dlg_copy::tb_dst_clicked()
{
  if (tb_dst->text() == "U") {
   dlg_url url;
    url.le_url_->setText(lb_url->text());
    if (url.exec() == QDialog::Accepted)
      le_dest->setText(url.le_url_->text());
 }
  else {
    QFileDialog* fd = new QFileDialog(this, "Select file/directory", TRUE );
    fd->setMode(QFileDialog::AnyFile);
    if (fd->exec() == QDialog::Accepted) {
      QString path = fd->selectedFile();
	  if (path.endsWith("/"))
		  path.remove(path.length()-1, 1);
      le_dest->setText(path);
     }
  }
}
