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
#include <string>
using namespace std;

void dlg_switch::bt_change_clicked()
{
	QString modif_to = "";
	
	if (rb_trunk->isChecked())	
		modif_to = "/trunk";
	
	if (rb_tag->isChecked() && le_tag->text() !="")
		modif_to = "/tags/"+le_tag->text();
	
	if (rb_branch->isChecked() && le_branch->text() !="")
		modif_to = "/branches/"+le_branch->text();
	
	
	QString url = le_url->text();
	int pos = url.find("/trunk/");
	
	if (pos != -1) {
		pos = url.find("/trunk/");
		if (pos != -1) {
			if (url.find("/", pos) != -1)
				modif_to += "/";
			qDebug("%d %s", url.find("/", pos), modif_to.ascii());
			url.replace(pos, 7, modif_to);
		}
	}
	else {
		pos = url.find("/tags/");
		if (pos != -1) {
			int name_pos = url.find("/", pos+6);
			if (name_pos == -1)
				name_pos = url.length();
			else {
				if (url.find("/", name_pos) != -1)
					modif_to += "/";
				qDebug("%d %s %d", url.find("/", name_pos+1), modif_to.ascii(),name_pos);
			}
			
			url.replace(pos, name_pos-pos+1, modif_to);
		}
		else {
			pos = url.find("/branches/");
			if (pos != -1) {
				int name_pos = url.find("/", pos+10);
				if (name_pos == -1)
					name_pos = url.length();
				else {
					if (url.find("/", name_pos) != -1)
						modif_to += "/";
					qDebug("%d %s", url.find("/", name_pos+1), modif_to.ascii());
				}
				url.replace(pos, name_pos-pos+1, modif_to);
			}
			else {
				pos = url.find("/trunk");
				if (pos != -1) 
					url.replace(pos,7, modif_to);
			}
		}
	}
	
	le_url->setText(url);
}


void dlg_switch::le_tag_textChanged( const QString & )
{
	rb_tag->setChecked(true);
}


void dlg_switch::le_branch_textChanged( const QString & )
{
	rb_branch->setChecked(true);
}
