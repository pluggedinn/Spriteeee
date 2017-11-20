#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>
#include <QLinkedList>

/**
 * DrawArea Model header file.
 * @brief The DrawArea class
 */
class DrawArea : public QLabel
{
    Q_OBJECT

public:
    explicit DrawArea(QWidget* parent = 0, QImage* image = new QImage());
    ~DrawArea();

signals:
    void updateCurrentFrame();
    void loadNewFrame(QImage*);

public slots:
    void updateFrameSize(int size);
    void updateCurrentTool(int number);
    void changePaintFrame(QImage* image);
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
