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

#include "dlg_url.h"
#include "../src/logwindow.h"

void dlg_diff_setup::rb_revision_toggled( bool value)
{
	rb_date->setChecked(!value);
}


void dlg_diff_setup::rb_date_toggled( bool value)
{
	rb_revision->setChecked(!value);
}



void dlg_diff_setup::le_revision_textChanged( const QString & )
{
	rb_date->setChecked(false);
	rb_revision->setChecked(true);
}


void dlg_diff_setup::le_date_textChanged( const QString & )
{
	rb_date->setChecked(true);
	rb_revision->setChecked(false);

}


void dlg_diff_setup::bt_uw1_clicked()
{
 dlg_url url;
 url.le_url_->setText(lb_url->text());
 if (url.exec() == QDialog::Accepted)
  le_url1->setText(url.le_url_->text());
}




void dlg_diff_setup::bt_uw2_clicked()
{
 dlg_url url;
 url.le_url_->setText(lb_url->text());
 if (url.exec() == QDialog::Accepted)
  le_url2->setText(url.le_url_->text());

}


void dlg_diff_setup::bt_rev_wizard_clicked()
{
}


void dlg_diff_setup::bt_rev_prev_clicked()
{
 le_revision->setText("PREV");
}


void dlg_diff_setup::bt_rev_head_clicked()
{
 le_revision->setText("HEAD");
}


void dlg_diff_setup::bt_rev_base_clicked()
{
 le_revision->setText("BASE");
}
