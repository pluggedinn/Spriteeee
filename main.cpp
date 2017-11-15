#include "mainwindow.h"
#include "drawarea.h"
#include <QApplication>
#include <QFrame>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFrame *paintFramePtr = w.findChild<QFrame*>("paintFrame");
    DrawArea drawArea(paintFramePtr, w.sprite.frames[0]);
    drawArea.setGeometry(0,0,448,448);

    QObject::connect(&w, SIGNAL(setSizeFrame(int)), &drawArea, SLOT(updateFrameWidth(int)));
    QObject::connect(&w, SIGNAL(toolClicked(int)), &drawArea, SLOT(updateToolNumber(int)));
    QObject::connect(&drawArea, SIGNAL(updateCurrentFrameDisplay()), &w, SLOT(updateSelectedFrameDisplay()));
    QObject::connect(&w, SIGNAL(selectedColor(QColor)), &drawArea, SLOT(updateCurrentColor(QColor)));
    QObject::connect(&w, SIGNAL(undoButtonClicked()), &drawArea, SLOT(undo()));
    QObject::connect(&w, SIGNAL(redoButtonClicked()), &drawArea, SLOT(redo()));

    QObject::connect(&w, SIGNAL(newFrameSelected(QImage*)), &drawArea, SLOT(updateCanvasToNewImage(QImage*)));

//    QObject::connect(&w, SIGNAL(frameSelected(QImage*)), &iLabel, SLOT(clearUndoRedoList(QImage*)));
    QObject::connect(&w, SIGNAL(frameSelected(QImage*)), &drawArea, SLOT(updateCanvasToNewImage(QImage*)));

    QObject::connect(&drawArea, SIGNAL(updateModelWithNewFrame(QImage*)), &w, SLOT(updateSelectedFrameWithNewImage(QImage*)));
    QObject::connect(&w, SIGNAL(updateFrameSize(int)), &drawArea, SLOT(updateFrameWidth(int)));

    drawArea.show();
    w.show();
    w.displayFrameWidthQuestion();

    return a.exec();
}
