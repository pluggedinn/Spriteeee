#include "mainwindow.h"
#include "drawarea.h"
#include <QApplication>
#include <QFrame>

/**
 * Authors: Josh Lipio, Riccardo Sonsini
 *
 * Main function used to connected the various parts of the program.
 * The controller of the application
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Attaching the paintFrame
    QFrame *paintFrame = w.findChild<QFrame*>("paintFrame");
    DrawArea drawArea(paintFrame, w.frames[0]);
    drawArea.setGeometry(0,0,512,512);

    // Connect from the view with the model
    QObject::connect(&w, SIGNAL(setSizeFrame(int)), &drawArea, SLOT(updateFrameSize(int)));
    QObject::connect(&w, SIGNAL(toolClicked(int)), &drawArea, SLOT(updateCurrentTool(int)));
    QObject::connect(&w, SIGNAL(selectedColor(QColor)), &drawArea, SLOT(updateCurrentColor(QColor)));
    QObject::connect(&w, SIGNAL(undoButtonClicked()), &drawArea, SLOT(undo()));
    QObject::connect(&w, SIGNAL(redoButtonClicked()), &drawArea, SLOT(redo()));
    QObject::connect(&w, SIGNAL(clearFrameClicked()), &drawArea, SLOT(clearDrawArea()));
    QObject::connect(&w, SIGNAL(frameSelected(QImage*)), &drawArea, SLOT(changePaintFrame(QImage*)));
    QObject::connect(&w, SIGNAL(updateFrameSize(int)), &drawArea, SLOT(updateFrameSize(int)));
    QObject::connect(&w, SIGNAL(brushSliderMoved(int)), &drawArea, SLOT(updateBrushSize(int)));
    QObject::connect(&w, SIGNAL(invertButtonClicked()), &drawArea, SLOT(invertColors()));
    QObject::connect(&w, SIGNAL(flipButtonClicked()), &drawArea, SLOT(flipImage()));

    // Connect the model to the view
    QObject::connect(&drawArea, SIGNAL(updateCurrentFrame()), &w, SLOT(updateSelectedCurrentFrame()));
    QObject::connect(&drawArea, SIGNAL(loadNewFrame(QImage*)), &w, SLOT(updateLoadedNewFrame(QImage*)));

    drawArea.show();
    w.show();

    return a.exec();
}
