#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>
#include <QLinkedList>

class DrawArea : public QLabel
{
    Q_OBJECT

public:
    explicit DrawArea(QWidget* parent = 0, QImage* image = new QImage(), int currentFrame = 0);
    ~DrawArea();
    int sizeX;
    int sizeY;
    int numberOfPixels;
    int tool;
    int pixelSize;
    int pixelNumber;
    QImage* image;
    QImage* frame;
    QColor color;
    QPixmap* pixmap;

signals:
    void updateCurrentFrameDisplay();
    void updateModelWithNewFrame(QImage*);

public slots:
    void updateFrameWidth(int size);
    void updateToolNumber(int number);
    void updateCanvasToNewImage(QImage* image);
    void updateCurrentColor(QColor);
    void undo();
    void redo();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void drawPixel(QMouseEvent* event);
    void detectCanvasChange(QImage);
    QLinkedList<QImage *> undoList;
    QLinkedList<QImage *> redoList;

};

#endif // DRAWAREA_H
