#ifndef MPSTOKENWIDGET_HPP
#define MPSTOKENWIDGET_HPP

#include <QtWidgets/QGraphicsWidget>
#include <QtGui/QFont>
#include <QtCore/QVector>
#include "MPSEngine.hpp"

class QGraphicsSceneMouseEvent;
class MPSToken;
class MPSMainWindow;
class MPSTokenWidget;

//  vector
typedef QVector<MPSTokenWidget*> MPSTokenWidgetVector;

class MPSTokenWidget : public QGraphicsWidget
{
public:
    enum { Type = UserType + 1 };

    // Enable the use of qgraphicsitem_cast with this item.
    int type() const { return Type; }

public:
    MPSTokenWidget (
            MPSToken* token,
            MPSEngine& engine,
            int inter_token_horiz_space,
            int inter_token_vert_space);

    MPSToken* token () const { return m_token; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *,QWidget *);

    void set_selected (bool sel) { m_selected = sel; }
    bool is_selected () const { return m_selected; }

    void set_color (QColor color) { m_color = color; }
    QColor get_color () const { return m_color; }

    void set_selected_color (QColor color) { m_sel_color = color; }
    QColor get_selected_color () const { return m_sel_color; }

    void set_is_first_subtoken (bool flag) { m_is_first_subtoken = flag; }
    bool is_first_subtoken () const { return m_is_first_subtoken; }

    void set_is_last_subtoken (bool flag) { m_is_last_subtoken = flag; }
    bool is_last_subtoken () const { return m_is_last_subtoken; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    MPSEngine&      m_engine;
    MPSToken*       m_token;      //  token we're drawing
    QFont           m_font;

    bool            m_selected;
    int             m_horiz_inter_space;
    int             m_vert_inter_space;
    bool            m_is_first_subtoken;
    bool            m_is_last_subtoken;

    //  settings
    QColor          m_color;        //  regular color when unselected and not group
    QColor          m_group_color;  //  color when unselected and group
    QColor          m_sel_color;    //  color when selected    
    QColor          m_discard_color;
    int             m_font_size;

    static int      KExtraWidth;
    static int      KExtraHeight;
};

#endif // MPSTOKENWIDGET_HPP
