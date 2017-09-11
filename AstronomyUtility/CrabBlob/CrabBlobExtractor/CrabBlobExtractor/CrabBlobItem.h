#ifndef CRABBLOBITEM_H
#define CRABBLOBITEM_H

#include <QRect>

class CrabBlobItem
{
public:
    CrabBlobItem();
    QRect rect_map_to_rect(QRect InputRect);
    double xc;
    double yc;
    double fc;

};

#endif // CRABBLOBITEM_H
