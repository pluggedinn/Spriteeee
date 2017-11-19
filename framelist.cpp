#include "framelist.h"
#include <QImage>

FrameList::FrameList(QObject *parent) : QObject(parent)
{
    currentFrame = -1;
    spriteSize = 32;
}

void FrameList::addFrame() {
    QImage* emptyImage = new QImage(512,512, QImage::Format_ARGB32);
    QColor color(0,0,0,0);
    emptyImage->fill(color);
    frames.append(emptyImage);
}

/**
 * Creates the saveble output
 * @brief FrameList::getSaveoutput
 * @return
 */
QString FrameList::getSaveoutput()
{

}
