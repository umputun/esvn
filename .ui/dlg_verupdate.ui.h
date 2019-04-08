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
#include <qprocess.h>

void dlg_verupdate::bt_upd_stable_clicked()
{
  QProcess process(this);
  process.addArgument(lb_browser->text());
  process.addArgument(lb_stable_url->text());
  process.start();
  hide();
}


void dlg_verupdate::bt_upd_testing_clicked()
{
  QProcess process(this);
  process.addArgument(lb_browser->text());
  process.addArgument(lb_testing_url->text());
  process.start();
  hide();
}


void dlg_verupdate::pb_all_clicked()
{
  QProcess process(this);
  process.addArgument(lb_browser->text());
  process.addArgument("http://zoneit.free.fr/esvn/versions.html");
  process.start();
  hide();
}
