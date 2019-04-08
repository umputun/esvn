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
#include <qstyle.h>
#include <qpainter.h>
#include <qapplication.h>

#include "check_lbi.h"



/*!
    \class QListBoxCheck qlistbox.h
    \brief The QListBoxCheck class provides list box items with a
    pixmap and optional text.

    \ingroup advanced

    Items of this class are drawn with the pixmap on the left with the
    optional text to the right of the pixmap.

    \sa QListBox, QListBoxItem
*/


/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pixmap.
*/

QListBoxCheck::QListBoxCheck( QListBox* listbox, const QPixmap &pixmap, bool on )
    : QListBoxItem( listbox )
{
    pm = pixmap;
    this->on = on;
}

/*!
    Constructs a new list box item showing the pixmap \a pixmap.
*/

QListBoxCheck::QListBoxCheck( const QPixmap &pixmap, bool on )
    : QListBoxItem()
{
    pm = pixmap;
    this->on = on;
}

/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pixmap. The item gets inserted after the item \a after,
    or at the beginning if \a after is 0.
*/

QListBoxCheck::QListBoxCheck( QListBox* listbox, const QPixmap &pixmap, QListBoxItem *after, bool on )
    : QListBoxItem( listbox, after )
{
    pm = pixmap;
    this->on = on;
}

/*!
    Constructs a new list box item in list box \a listbox showing \a text only
*/

QListBoxCheck::QListBoxCheck( QListBox* listbox, const QString &text, bool on )
    : QListBoxItem( listbox )
{
    this->on = on;
    setText( text );
}

/*!
    Constructs a new list box item showing \a text only
*/

QListBoxCheck::QListBoxCheck( const QString &text, bool on )
    : QListBoxItem()
{
    this->on = on;
    setText( text );
}

/*!
    Constructs a new list box item in list box \a listbox showing \a text
    only. The item gets inserted after the item \a after,
    or at the beginning if \a after is 0.
*/

QListBoxCheck::QListBoxCheck( QListBox* listbox, const QString &text, QListBoxItem *after, bool on )
    : QListBoxItem( listbox, after )
{
    this->on = on;
    setText( text );
}



/*!
    Destroys the item.
*/

QListBoxCheck::~QListBoxCheck()
{
}


/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pix and the text \a text.
*/
QListBoxCheck::QListBoxCheck( QListBox* listbox, const QPixmap &pix, const QString& text, bool on)
    : QListBoxItem( listbox )
{
    pm = pix;
    this->on = on;
    setText( text );
}

/*!
    Constructs a new list box item showing the pixmap \a pix and the
    text to \a text.
*/
QListBoxCheck::QListBoxCheck( const QPixmap & pix, const QString& text, bool on)
    : QListBoxItem()
{
    pm = pix;
    this->on = on;
    setText( text );
}

/*!
    Constructs a new list box item in list box \a listbox showing the
    pixmap \a pix and the string \a text. The item gets inserted after
    the item \a after, or at the beginning if \a after is 0.
*/
QListBoxCheck::QListBoxCheck( QListBox* listbox, const QPixmap & pix, const QString& text,
                              QListBoxItem *after, bool on )
    : QListBoxItem( listbox, after )
{
    pm = pix;
    this->on = on;
    setText( text );
}

/*!
    \fn const QPixmap *QListBoxCheck::pixmap() const

    Returns the pixmap associated with the item.
*/


/*!
    Draws the pixmap using \a painter.
*/

void QListBoxCheck::paint( QPainter *painter )
{
	int itemHeight = height( listBox() );
    int yPos;

    QSize sz = QSize( listBox()->style().pixelMetric( QStyle::PM_IndicatorWidth ),
		      listBox()->style().pixelMetric( QStyle::PM_IndicatorHeight ) );
    QColorGroup c( listBox()->colorGroup() );
    c.setBrush( QColorGroup::Background, c.brush( QColorGroup::Base ) );
    QStyle::SFlags flags = QStyle::Style_Default;
    if(isSelectable() && listBox()->isEnabled())
	flags |= QStyle::Style_Enabled;
    if ( on )
	flags |= QStyle::Style_On;
    else
	flags |= QStyle::Style_Off;

	const QPixmap *pm = pixmap();
	if ( pm && ! pm->isNull() ) {
		yPos = ( itemHeight - pm->height() ) / 2;
		painter->drawPixmap( 3 + sz.width(), yPos, *pm);
	}

	if ( !text().isEmpty() ) {
		QFontMetrics fm = painter->fontMetrics();
		yPos = ( ( itemHeight - fm.height() ) / 2 ) + fm.ascent();
		if (selected()) {
			painter->setPen(listBox()->colorGroup().highlightedText());
		} else {
			painter->setPen(listBox()->colorGroup().text());
		}
		painter->drawText( sz.width() + pm->width() + 5, yPos, text() );
	}

	listBox()->style().drawPrimitive( QStyle::PE_Indicator, painter,
									  QRect( 0, ( itemHeight - sz.height() ) / 2, sz.width(), sz.height() ), c, flags );
}

/*!
    Returns the height of the pixmap in list box \a lb.

    \sa paint(), width()
*/

int QListBoxCheck::height( const QListBox* lb ) const
{
    int h;

    int ch = listBox()->style().pixelMetric( QStyle::PM_IndicatorHeight );

    if ( text().isEmpty() )
	h = QMAX( pm.height(), ch + 2 );
    else
	h = QMAX( QMAX( pm.height(), ch ), lb->fontMetrics().lineSpacing() + 2 );
    return QMAX( h, QApplication::globalStrut().height() );
}

/*!
    Returns the width of the pixmap plus some margin in list box \a lb.

    \sa paint(), height()
*/

int QListBoxCheck::width( const QListBox* lb ) const
{
	int w = listBox()->style().pixelMetric( QStyle::PM_IndicatorWidth );

	if ( text().isEmpty() )
		return QMAX( pm.width() + 6, QApplication::globalStrut().width() ) + w;
	return QMAX( pm.width() + lb->fontMetrics().width( text() ) + 6,
				 QApplication::globalStrut().width() ) + w;
}

int QListBoxCheck::RTTI = 1001;

/*!
    \fn int QListBoxCheck::rtti() const

    \reimp

    Returns 1001.
*/

int QListBoxCheck::rtti() const
{
    return RTTI;
}

void QListBoxCheck::setOn( bool on )
{
    if( this->on != on  ) {
        this->on = on;
        listBox()->triggerUpdate( false );
    }
}

bool QListBoxCheck::isPointOnCheck( const QPoint& p )
{
    int itemHeight = height( listBox() );
    QRect r = listBox()->itemRect( this );
    QSize sz = QSize( listBox()->style().pixelMetric( QStyle::PM_IndicatorWidth ),
		      listBox()->style().pixelMetric( QStyle::PM_IndicatorHeight ) );

    QRect rg( listBox()->viewport()->mapToGlobal(r.topLeft()+QPoint(0,(itemHeight - sz.height()) / 2)), sz );
    return rg.contains( p );
}
