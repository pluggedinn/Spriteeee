#include "drawarea.h"
#include <string>
#include <QLabel>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>

DrawArea::DrawArea(QWidget* parent, QImage* image, int currentFrame) : QLabel(parent)
{
    sizeX = 448;
    sizeY = 448;
    numberOfPixels = 32;
    pixelNumber = 2;
    pixelSize = sizeX / numberOfPixels;
    pixmap = NULL;
    frame = image;
    this->image = new QImage(sizeX, sizeY, QImage::Format_ARGB32);
    color = QColor(0,0,0);
    tool = 1;

    QImage* emptyImage = new QImage(sizeX, sizeY, QImage::Format_ARGB32);
    undoList.append(emptyImage);

    updateCanvasToNewImage(frame);
}

DrawArea::~DrawArea()
{
    delete(pixmap);
    delete(image);
}

void DrawArea::mousePressEvent(QMouseEvent* event) {

    if (tool == 1) {            // brush
        drawPixel(event);
    } else if (tool == 2) {     // flip
        QImage mirrored = image->mirrored(true, false);
        *image = mirrored;
        frame = this->image;
        this->pixmap = new QPixmap();
        pixmap->convertFromImage(*this->image);
        this->setPixmap(QPixmap::fromImage(*image));
        emit updateCurrentFrameDisplay();
    } else if (tool == 3) {      // mirror
        drawPixel(event);
    } else if (tool == 4) {      // erase
        drawPixel(event);
    }
}

void DrawArea::mouseMoveEvent(QMouseEvent* event) {
    drawPixel(event);

}
void DrawArea::mouseReleaseEvent(QMouseEvent *event) {
    detectCanvasChange(*image);
}


void DrawArea::drawPixel(QMouseEvent* event) {
    int posX = event->pos().x();
    int posY = event->pos().y();
    int column = posX / pixelSize;
    int row = posY / pixelSize;
    int leftMostXPixel = pixelSize*column; //the leftmost pixel in the column
    int rightMostXPixel = leftMostXPixel + (pixelNumber * pixelSize); //the rightmost pixel in the column
    int leftMostYPixel = pixelSize*row; //the top pixel in the row
    int rightMostYPixel = leftMostYPixel + (pixelNumber * pixelSize); //the bottom pixel in the row

    for(int x = leftMostXPixel; x < rightMostXPixel; x++)
    {
        for(int y = leftMostYPixel; y < rightMostYPixel; y++)
        {
            if (tool == 1) {
                image->setPixel(x,y,color.rgba());
            } else if (tool == 3) {
                int respectiveX = (pixelSize * (448 / pixelSize)) - x - 1 ; //mirror x location
                image->setPixel(x,y,color.rgba());
                image->setPixel(respectiveX,y,color.rgba());
            } else if (tool == 4) {
                clearPixel(x,y);
            }
        }
    }

    frame = this->image;
    this->pixmap = new QPixmap();
    this->pixmap->convertFromImage(*this->image);
    this->setPixmap(QPixmap::fromImage(*image));
    emit updateCurrentFrameDisplay();
}

void DrawArea::clearPixel(int x, int y) {
    image->setPixel(x,y,0000);
}

void DrawArea::updateFrameWidth(int size)
{
    numberOfPixels = size;
    pixelSize = sizeX / numberOfPixels;
}

void DrawArea::updateToolNumber(int number)
{
    tool = number;
}


void DrawArea::updateBrushSize(int size)
{
    pixelNumber = size;
}

/**
 * Updates the current canvas window to the new selected frame Image.
 * @brief ImageLabel::updateCanvasToNewImage
 * @param image
 */
void DrawArea::updateCanvasToNewImage(QImage* image)
{
    pixmap = new QPixmap();
    this->image = image;
    pixmap->convertFromImage(*this->image);
    setPixmap(QPixmap::fromImage(*image));
}

void DrawArea::detectCanvasChange(QImage oldImage)
{
    QImage* newImage = new QImage(oldImage);

    if(undoList.size() >= 20)
    {
        undoList.removeFirst();
    }
    undoList.append(newImage);
    redoList.clear();
}

void DrawArea::updateCurrentColor(QColor newColor) {
    color = newColor;
}

void DrawArea::undo()
{
    if(undoList.size() > 1)
    {
        redoList.append(undoList.last());
        undoList.removeLast();
        QImage* updatedImage = new QImage(*undoList.last());
        image = updatedImage;
        this->pixmap = new QPixmap();
        pixmap->convertFromImage(*this->image);
        this->setPixmap(QPixmap::fromImage(*image));
    }
    else if(undoList.size() == 1)
    {
        redoList.append(undoList.last());
        QImage* updatedImage = new QImage(*undoList.last());
        image = updatedImage;
        this->pixmap = new QPixmap();
        pixmap->convertFromImage(*this->image);
        this->setPixmap(QPixmap::fromImage(*image));
        undoList.removeLast();
    }
    emit updateModelWithNewFrame(image);
}

void DrawArea::redo()
{
    if(redoList.size() > 0)
    {
        QImage* updatedImage = new QImage(*redoList.last());
        image = updatedImage;
        this->pixmap = new QPixmap();
        pixmap->convertFromImage(*this->image);
        this->setPixmap(QPixmap::fromImage(*image));
        undoList.append(redoList.last());
        redoList.removeLast();
    }
    emit updateModelWithNewFrame(image);
}

void DrawArea::clearDrawArea() {
    clear();
    image->fill(Qt::transparent);
    frame = this->image;
    redoList.clear();
    undoList.clear();
    emit updateModelWithNewFrame(image);
    detectCanvasChange(*image);
}

