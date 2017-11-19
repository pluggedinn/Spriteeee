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
    explicit DrawArea(QWidget* parent = 0, QImage* image = new QImage());
    ~DrawArea();

signals:
    void updateCurrentFrameDisplay();
    void updateModelWithNewFrame(QImage*);

public slots:
    void updateFrameWidth(int size);
    void updateToolNumber(int number);
    void updateCanvasToNewImage(QImage* image);
    void updateCurrentColor(QColor);
    void updateBrushSize(int);
    void undo();
    void redo();
    void clearDrawArea();
    void invertColors();
    void flipImage();

private:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void drawPixel(QMouseEvent* event);
    void clearPixel(int x, int y);
    void detectCanvasChange(QImage);
    int numberOfPixels;
    int pixelNumber;
    int tool;
    int pixelSize;
    QImage* image;
    QColor color;
    QPixmap* pixmap;
    QLinkedList<QImage *> undoList;
    QLinkedList<QImage *> redoList;

};

#endif // DRAWAREA_H
