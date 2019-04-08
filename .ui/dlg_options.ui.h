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

#include <qfontdialog.h> 
#include <qlineedit.h>
#include <qlistview.h>
#include <qfiledialog.h>

#include "dlg_map_edit.h"

void dlg_options::bt_default_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_default->setText(font.toString());
}


void dlg_options::bt_view_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_view->setText(font.toString());
}




void dlg_options::bt_output_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_output->setText(font.toString());
}


void dlg_options::bt_list_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_list->setText(font.toString());
}


void dlg_options::bt_diff_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_diff_font->setText(font.toString());
}


void dlg_options::bt_cmd_clicked()
{
 bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( "Helvetica [Cronyx]", 10 ), this );
 if (ok) 
  le_cmd->setText(font.toString());
}


void dlg_options::bt_add_clicked()
{
 dlg_map_edit dlg_map_edit_;
 if (dlg_map_edit_.exec() == QDialog::Accepted) {
  if (dlg_map_edit_.le_ext->text().isEmpty() || dlg_map_edit_.le_act->text().isEmpty() )
 return;
  QListViewItem *item = new QListViewItem(lv_map, 0);
  item->setText(0, dlg_map_edit_.le_ext->text());  
  item->setText(1, dlg_map_edit_.le_act->text());
 }
}



void dlg_options::bt_edit_clicked()
{
  QListViewItem* item = lv_map->selectedItem();
   if (item) {
   dlg_map_edit dlg_map_edit_;
  dlg_map_edit_.le_ext->setText(item->text(0));
    dlg_map_edit_.le_act->setText(item->text(1));
  if (dlg_map_edit_.exec() == QDialog::Accepted) {
     if (dlg_map_edit_.le_ext->text().isEmpty() || dlg_map_edit_.le_act->text().isEmpty() )
    return;
   QListViewItem *item = new QListViewItem(lv_map, 0);
     item->setText(0, dlg_map_edit_.le_ext->text());  
     item->setText(1, dlg_map_edit_.le_act->text());
   bt_delete_clicked();
   }
 }
}


void dlg_options::bt_delete_clicked()
{
 QListViewItem* item = lv_map->selectedItem();
 if (item) {
  lv_map->takeItem(item); 
 }
}
