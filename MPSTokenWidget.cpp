/*
 * This file is part of MemphisNow.
 *
 * MemphisNow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MemphisNow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MemphisNow.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MPSTokenWidget.hpp"
#include <QPainter>
#include <QRectF>
#include <QFontMetrics>
#include "MPSToken.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDebug>
#include "MPSMainWindow.hpp"

int MPSTokenWidget::KExtraWidth = 20;
int MPSTokenWidget::KExtraHeight = 5;

MPSTokenWidget::MPSTokenWidget (
        MPSToken* token,
        MPSEngine& engine,
        int inter_token_horiz_space,
        int inter_token_vert_space) :
    m_engine (engine),
    m_token (token),
    m_selected (false),
    m_horiz_inter_space (inter_token_horiz_space),
    m_vert_inter_space (inter_token_vert_space),
    m_is_first_subtoken (false),
    m_is_last_subtoken (false)
{
    m_color.setRgb(2, 157, 116);
    m_sel_color.setRgb(238, 238, 0);
    m_group_color.setRgb(100, 149, 237);
    m_discard_color.setRgb(100, 96, 96);
    m_font_size = 12;

    m_font.setFamily("Arial");
    m_font.setPointSize (m_font_size);
    m_font.setBold(true);

    QFontMetrics fm (m_font);
    QString text = QString::fromStdWString(m_token->text());
    setGeometry(x(), y(), fm.horizontalAdvance(text) + KExtraWidth, fm.height() + KExtraHeight);
}

void MPSTokenWidget::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem*,
                           QWidget*)
{
    bool is_group = !m_token->sub_tokens_empty();

    m_font.setItalic(is_group);
    painter->setFont(m_font);
    QFontMetrics fm = painter->fontMetrics();
    QString text = QString::fromStdWString(m_token->text());

    QPoint center = QPoint((rect().width() - fm.horizontalAdvance(text)) / 2,
                            (rect().height() + fm.height()) / 2 - 5);

    QBrush brush;
    if (!m_token->is_discard()) {
        if (is_group) {
            brush.setColor(m_selected ? m_sel_color : m_group_color);
            brush.setStyle(Qt::Dense4Pattern);
        } else {
            brush.setColor(m_selected ? m_sel_color : m_color);
            brush.setStyle(Qt::SolidPattern);
        }
    } else {
        brush.setStyle(Qt::DiagCrossPattern);
        brush.setColor(m_selected ? m_sel_color : m_discard_color);        
    }
    painter->fillRect(rect(), brush);

    QPen pen;
    pen.setWidth(1);

    pen.setColor((!m_token->is_discard()) ? Qt::white : m_discard_color);
    painter->setPen(pen);
    painter->drawRect(rect());

    pen.setColor(
        m_token->is_discard() ?
            (m_selected ? m_sel_color : m_discard_color) :
            (m_selected ? Qt::black : Qt::white)
    );
    painter->setPen(pen);
    painter->drawText(center, text);
}

void MPSTokenWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);

    //  deselect all others
    int idx = 0;
    for ( ; idx < scene()->items().count(); ++idx) {
        MPSTokenWidget* item = qgraphicsitem_cast<MPSTokenWidget*> (scene()->items().at(idx));
        if (item != 0)
            item->set_selected (false);
    }

    //  toggle enabled status on right click
    if (event->button() == Qt::RightButton) {
        m_token->set_discard(!m_token->is_discard());
    }

    m_selected = true;

    if (!m_engine.is_token_current_root(m_token)) {
        //  force update
        m_engine.update(m_token);
    }
    //  do select, which will update the viewers as well
    m_engine.select_subtoken(m_token, event->button() == Qt::RightButton);
}
