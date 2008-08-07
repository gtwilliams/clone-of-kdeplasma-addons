/*
 *   Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser/Library General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser/Library General Public License for more details
 *
 *   You should have received a copy of the GNU Lesser/Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QVariant>
#include "Widget.h"
#include "Global.h"
#include <lancelot/lancelot.h>
#include <KDebug>

namespace Lancelot {

class Widget::Private {
public:
    Private()
      : group(NULL),
        hover(false),
        down(false)
    {
    };

    WidgetGroup * group;
    bool hover : 1;
    bool down : 1;
};

Widget::Widget(QGraphicsItem * parent)
  : QGraphicsWidget(parent),
    d(new Private())
{
    Instance::activeInstance()->addWidget(this);
    setGroup(NULL);
    // d->group = Instance::activeInstance()->defaultGroup();
    L_WIDGET_SET_INITIALIZED;
}

Widget::~Widget()
{
    L_WIDGET_UNSET_INITIALIZED;
    kDebug() << "Destroying widget" << (void *)this
             << "with private" << (void *)d
             << "and group" << (void *)d->group;
    L_debug();
    if (d->group != NULL) {
        d->group->instance()->removeWidget(this);
        d->group->removeWidget(this, false);
    }
    delete d;
}

void Widget::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if (!isEnabled()) return;
    d->hover = true;
    QGraphicsWidget::hoverEnterEvent(event);
    emit mouseHoverEnter();
    update();
}

void Widget::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if (!isEnabled()) return;
    d->hover = false;
    QGraphicsWidget::hoverEnterEvent(event);
    emit mouseHoverLeave();
    update();
}

void Widget::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        d->down = true;
        update();
        event->accept();
        emit pressed();
    } else {
        QGraphicsWidget::mousePressEvent(event);
    }
}

void Widget::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (d->down && (event->button() == Qt::LeftButton)) {
        d->down = false;
        kDebug();
        L_debug();
        emit released();
        emit clicked();
    } else {
        QGraphicsWidget::mouseReleaseEvent(event);
    }
}

bool Widget::isHovered() const
{
    return d->hover;
}

QString Widget::groupName() const
{
    if (d->group) {
        return d->group->name();
    }
    return QString();
}

void Widget::setGroupByName(const QString & groupName)
{
    setGroup(instance()->group(groupName));
}

void Widget::setGroup(WidgetGroup * group)
{
    if (group == NULL) {
        group = instance()->defaultGroup();
    }

    if (group == d->group) return;

    if (d->group != NULL) {
        d->group->removeWidget(this, false);
    }

    d->group = group;
    d->group->addWidget(this);
    groupUpdated();
}

WidgetGroup * Widget::group() const
{
    return d->group;
}

Instance * Widget::instance() const
{
    if (!d->group) return Instance::activeInstance();

    return d->group->instance();
}

void Widget::groupUpdated()
{
    update();
}

void Widget::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
        QWidget * widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    paintBackground(painter);
}

void Widget::paintBackground(QPainter * painter) {
    if (!d->group) return;

    QString element;
    if (!isEnabled()) {
        element = "disabled";
    } else if (d->down) {
        element = "down";
    } else if (d->hover) {
        element = "active";
    } else {
        element = QString(); // was "normal" - removing prefix for this state
    }

    paintBackground(painter, element);
}

void Widget::paintBackground(QPainter * painter, const QString & element) {
    if (!d->group) return;

    // Background Painting
    if (Plasma::PanelSvg * svg = d->group->backgroundSvg()) {
        kDebug() << "Background SVG " << svg->isValid();
        kDebug() << "Background SVG " << svg->imagePath();
        kDebug() << "Background prefix " << element;
        svg->setElementPrefix(element);
        kDebug() << "Background prefix " << svg->hasElementPrefix(element);
        svg->resizePanel(size());
        svg->paintPanel(painter); // API change since 4.1

    } else if (const WidgetGroup::ColorScheme * scheme = d->group->backgroundColor()) {
        const QColor * color;
        if (!isEnabled()) {
            color = & (scheme->disabled);
        } else if (d->hover) {
            color = & (scheme->active);
        } else {
            color = & (scheme->normal);
        }
        painter->fillRect(QRectF(QPointF(0, 0), size()), QBrush(*color));
    }
}

void Widget::geometryUpdated()
{

}

void Widget::setGeometry(const QRectF & rect)
{
    kDebug() << L_WIDGET_IS_INITIALIZED;
    L_debug();
    if (L_WIDGET_IS_INITIALIZED) {
        QGraphicsWidget::setGeometry(rect);
        geometryUpdated();
    }
}

void Widget::setGeometry(qreal x, qreal y, qreal w, qreal h)
{
    Widget::setGeometry(QRectF(x, y, w, h));
}

QSizeF Widget::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
{
    QSizeF result;
    switch (which) {
        case Qt::MinimumSize:
            result = QSizeF();
            break;
        case Qt::MaximumSize:
            result = MAX_WIDGET_SIZE;
            break;
        case Qt::PreferredSize:
            if (Plasma::PanelSvg * svg = d->group->backgroundSvg()) {
                result = QSizeF(
                    svg->marginSize(Plasma::LeftMargin) +
                    svg->marginSize(Plasma::RightMargin),
                    svg->marginSize(Plasma::TopMargin) +
                    svg->marginSize(Plasma::BottomMargin)
                    );
            } else {
                result = QSizeF();
            }
            break;
        default:
            result = QSizeF();
            break;
    }
    if (constraint != QSizeF(-1, -1)) {
        result = result.boundedTo(constraint);
    }
    return result;
}

bool Widget::isDown()
{
    return d->down;
}


} // namespace Lancelot

#include "Widget.moc"

