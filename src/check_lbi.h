/***************************************************************************
*   Copyright (C) 2004 by Alexander Darovsky                              *
*   adarovsky@users.sf.net                                                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   Based on QListBoxPixmap source from TrollTech                         *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
***************************************************************************/
#include <qlistbox.h>

class QListBoxCheck : public QListBoxItem
{
public:
    QListBoxCheck( QListBox* listbox, const QPixmap &, bool on = false );
    QListBoxCheck( const QPixmap &, bool on = false );
    QListBoxCheck( QListBox* listbox, const QPixmap & pix, QListBoxItem *after, bool on = false );
    QListBoxCheck( QListBox* listbox, const QPixmap &, const QString&, bool on = false );
    QListBoxCheck( const QPixmap &, const QString&, bool on = false );
    QListBoxCheck( QListBox* listbox, const QPixmap & pix, const QString&, QListBoxItem *after, bool on = false );
    QListBoxCheck( QListBox* listbox, const QString &, bool on = false );
    QListBoxCheck( const QString &, bool on = false );
    QListBoxCheck( QListBox* listbox, const QString & pix, QListBoxItem *after, bool on = false );
   ~QListBoxCheck();

    const QPixmap *pixmap() const { return &pm; }

    void setOn( bool );
    bool isOn() const { return on; }

    bool isPointOnCheck( const QPoint& p );

    int	 height( const QListBox * ) const;
    int	 width( const QListBox * )  const;

    int rtti() const;
    static int RTTI;

protected:
    void paint( QPainter * );

private:
    QPixmap pm;
    bool    on;
private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QListBoxCheck( const QListBoxCheck & );
    QListBoxCheck &operator=( const QListBoxCheck & );
#endif
};
