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

/**
 * Creates the saveble output
 * @brief FrameList::getSaveoutput
 * @return
 */
QString FrameList::getSaveoutput()
{
    QString output = "";
    int totalWidth = 448 / width;

    for(QImage *img : frames)
    {
        for(int y = 1 ; y <= 448 - totalWidth+1; y = y + totalWidth)
        {
            for(int x = 1 ; x <= 448 - totalWidth+1; x = x + totalWidth)
            {
                QRgb rgbColor = QRgb(img->pixel(x, y));
                if(rgbColor != 0000)
                {
                    QColor color = QColor(rgbColor);
                    output += "" + QString::number(color.red());
                    output += " " + QString::number(color.green());
                    output += " " + QString::number(color.blue());
                    output += " " + QString::number(color.alpha());
                    output += " ";
                } else
                {
                    output += "0 0 0 0 ";
                }
            }
            output += "\n";
        }
    }
    return output;
}
