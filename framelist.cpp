#include "framelist.h"
#include <QImage>

FrameList::FrameList(QObject *parent) : QObject(parent)
{
    currentFrame = -1;
    width = 32;
}

void FrameList::addFrame() {
    QImage* emptyImage = new QImage(448,448, QImage::Format_ARGB32);
    QColor color(0,0,0,0);
    emptyImage->fill(color);
    frames.append(emptyImage);
}
