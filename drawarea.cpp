#include "drawarea.h"
#include <string>
#include <QLabel>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>

/**
 * Authors: Josh Lipio, Riccardo Sonsini
 *
 * Constructor of the application Model.
 * It sets te default values to the color, number of pixels, tool and the default empty picture.
 * @brief DrawArea::DrawArea
 * @param parent
 * @param image
 */
DrawArea::DrawArea(QWidget* parent, QImage* image) : QLabel(parent)
{
    numberOfPixels = 32;
    pixelNumber = 2;
    pixelSize = 512 / numberOfPixels;
    this->image = new QImage(512, 512, QImage::Format_ARGB32);
    tool = 1;

    undoList.append(this->image);

    changePaintFrame(image);
}

/**
 * Decostructor of the function. It deletes the image and the pixmap.
 * @brief DrawArea::~DrawArea
 */
DrawArea::~DrawArea()
{
    delete(pixmap);
    delete(image);
}

/**
 * QT default function that is triggered when the user presses the mouse.
 * It calls the drawing function based on the current tool.
 * @brief DrawArea::mousePressEvent
 * @param event
 */
void DrawArea::mousePressEvent(QMouseEvent* event)
{
    drawPixel(event);
}

/**
 * QT default function that is called when the user keep pressing the mouse.
 * It draws the pixel based on the current tool.
 * @brief DrawArea::mouseMoveEvent
 * @param event
 */
void DrawArea::mouseMoveEvent(QMouseEvent* event)
{
    drawPixel(event);
}

/**
 * QT default function that is called when the user releases the pressed mouse.
 * Changes all the necessary values in the model and the view.
 * @brief DrawArea::mouseReleaseEvent
 * @param event
 */
void DrawArea::mouseReleaseEvent(QMouseEvent *event)
{
    detectCanvasChange(*image);
}

/**
 * Main drawing function.
 * It draws and changes the value of each pixel based on the position of the mouse from the user.
 * It then updates the current frame.
 * @brief DrawArea::drawPixel
 * @param event
 */
void DrawArea::drawPixel(QMouseEvent* event) {
    int mouseX = event->pos().x();
    int mouseY = event->pos().y();
    int verticalPix = mouseX / pixelSize - pixelNumber / 2;
    int horizontalPix = mouseY / pixelSize - pixelNumber / 2;
    int leftBoundary = pixelSize * verticalPix;
    int rightBoundary = leftBoundary + (pixelNumber * pixelSize);
    int topBoundary = pixelSize * horizontalPix;
    int bottomBoundary = topBoundary + (pixelNumber * pixelSize);

    for(int x = leftBoundary; x < rightBoundary; x++)
    {
        for(int y = topBoundary; y < bottomBoundary; y++)
        {
            if (tool == 1)
            {
                image->setPixel(x,y,color.rgba());
            }
            else if (tool == 2)
            {
                int respectiveX = (pixelSize * (512 / pixelSize)) - x - 1 ;
                image->setPixel(x,y,color.rgba());
                image->setPixel(respectiveX,y,color.rgba());
            }
            else if (tool == 3)
            {
                clearPixel(x,y);
            }
        }
    }
    pixmap->convertFromImage(*image);
    setPixmap(QPixmap::fromImage(*image));
    emit updateCurrentFrame();
}

/**
 * Helper function that clears the given pixel.
 * @brief DrawArea::clearPixel
 * @param x
 * @param y
 */
void DrawArea::clearPixel(int x, int y)
{
    image->setPixel(x,y,0000);
}

/**
 * Function that is called when the user changes the dimension of the paintArea.
 * @brief DrawArea::updateFrameSize
 * @param size
 */
void DrawArea::updateFrameSize(int size)
{
    numberOfPixels = size;
    pixelSize = 512 / numberOfPixels;
}

/**
 * It changes the current tool in the model.
 * @brief DrawArea::updateCurrentTool
 * @param number
 */
void DrawArea::updateCurrentTool(int number)
{
    tool = number;
}

/**
 * It changes the brush size.
 * @brief DrawArea::updateBrushSize
 * @param size
 */
void DrawArea::updateBrushSize(int size)
{
    pixelNumber = size;
}

/**
 * Loads the image and changes the current view and model.
 * @brief DrawArea::changePaintFrame
 * @param image
 */
void DrawArea::changePaintFrame(QImage* newImage)
{
    pixmap = new QPixmap();
    image = newImage;
    pixmap->convertFromImage(*image);
    setPixmap(QPixmap::fromImage(*newImage));
}

/**
 * Detect when the paintFrame is changed and reseat the undo redo chain.
 * @brief DrawArea::detectCanvasChange
 * @param oldImage
 */
void DrawArea::detectCanvasChange(QImage oldImage)
{
    QImage* newImage = new QImage(oldImage);
    undoList.append(newImage);
    redoList.clear();
}

/**
 * It updates the current color.
 * @brief DrawArea::updateCurrentColor
 * @param newColor
 */
void DrawArea::updateCurrentColor(QColor newColor)
{
    color = newColor;
}

/**
 * Undo function that updates the current image to once that
 * doesn't have the last action.
 * @brief DrawArea::undo
 */
void DrawArea::undo()
{
    if(undoList.size() > 1)
    {
        redoList.append(undoList.last());
        undoList.removeLast();
        QImage* updatedImage = new QImage(*undoList.last());
        image = updatedImage;
        pixmap->convertFromImage(*image);
        setPixmap(QPixmap::fromImage(*image));
    }
    emit loadNewFrame(image);
}

/**
 * It searches in the undo list and restores the previously undoED action.
 * @brief DrawArea::redo
 */
void DrawArea::redo()
{
    if(redoList.size() > 0)
    {
        QImage* updatedImage = new QImage(*redoList.last());
        image = updatedImage;;
        pixmap->convertFromImage(*image);
        setPixmap(QPixmap::fromImage(*image));
        undoList.append(redoList.last());
        redoList.removeLast();
    }
    emit loadNewFrame(image);
}

/**
 * Clears the current frame in the paintFrame in the model.
 * @brief DrawArea::clearDrawArea
 */
void DrawArea::clearDrawArea()
{
    clear();
    image->fill(Qt::transparent);
    redoList.clear();
    undoList.clear();
    emit loadNewFrame(image);
    detectCanvasChange(*image);
}

/**
 * Tool that inverts all the color of the current frame.
 * @brief DrawArea::invertColors
 */
void DrawArea::invertColors()
{
    image->invertPixels(QImage::InvertRgb);
    pixmap->convertFromImage(*image);
    setPixmap((QPixmap::fromImage(*image)));
    emit loadNewFrame(image);
}

/**
 * Tool that flips all the pixels from left to right of the current frame.
 * @brief DrawArea::flipImage
 */
void DrawArea::flipImage()
{
    QImage mirrored = image->mirrored(true, false);
    *image = mirrored;
    pixmap->convertFromImage(*image);
    setPixmap(QPixmap::fromImage(*image));
    emit updateCurrentFrame();
}
