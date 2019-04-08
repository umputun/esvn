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


void dlg_merge::rb_url_toggled( bool state)
{
 if (state)
  rb_revision->setChecked(false);
}


void dlg_merge::rb_revision_toggled( bool state )
{
 if (state)
  rb_url->setChecked(false);

}


void dlg_merge::bt_rev_wizard_clicked()
{
}


void dlg_merge::bt_rev_wizard_2_clicked()
{
}


void dlg_merge::bt_rev_head_clicked()
{
 le_rev1->setText("HEAD");
}


void dlg_merge::bt_rev_prev_clicked()
{
 le_rev1->setText("PREV");
}


void dlg_merge::bt_rev_prev_2_clicked()
{
 le_rev2->setText("PREV");
}


void dlg_merge::bt_rev_head_2_clicked()
{
 le_rev2->setText("HEAD");
}


void dlg_merge::bt_uw2_clicked()
{
}


void dlg_merge::bt_dirs_clicked()
{
 QFileDialog* fd = new QFileDialog(this, "Select file/directory", TRUE );
 fd->setMode(QFileDialog::Directory);
 if (fd->exec() == QDialog::Accepted) {
  QString path = fd->selectedFile();
  if (path.endsWith("/"))
    path.remove(path.length()-1, 1);
  le_file->setText(path);
 }
}


void dlg_merge::bt_wcpath_clicked()
{
 QFileDialog* fd = new QFileDialog(this, "Select file/directory", TRUE );
 fd->setMode(QFileDialog::Directory);
 if (fd->exec() == QDialog::Accepted) {
  QString path = fd->selectedFile();
  if (path.endsWith("/"))
    path.remove(path.length()-1, 1);
  le_wcpath->setText(path);
  }
}


void dlg_merge::bt_url1_clicked()
{

}


void dlg_merge::bt_url2_clicked()
{

}
