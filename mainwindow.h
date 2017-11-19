#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPixmap>
#include "framelist.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    FrameList sprite;
    void updateToolButton(int button);
    void save();
    void load();
    void export_to_gif();
    void newProject(int);

signals:
    void setSizeFrame(int);
    void toolClicked(int);
    void frameSelected(QImage*);
    void selectedColor(QColor);
    void undoButtonClicked();
    void redoButtonClicked();
    void clearFrameClicked();
    void addFrameButtonClicked();
    void updateFrameSize(int);
    void brushSliderMoved(int);
    void invertButtonClicked();
    void flipButtonClicked();
    void previewFinished();

private slots:
    void on_brushToolButton_clicked();
    void on_flipToolButton_clicked();
    void on_colorsButton_pressed();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_mirrorButton_clicked();
    void on_clearButton_clicked();
    void on_deleteFrameButton_clicked();
    void on_duplicateFrameButton_clicked();
    void on_addFrameButton_clicked();
    void on_framesListWidget_itemClicked(QListWidgetItem *item);
    void on_eraseButton_clicked();
    void on_brushSizeSlider_valueChanged(int value);
    void on_invertButton_clicked();
    void on_actionSave_As_triggered();
    void on_actionLoad_triggered();
    void on_actionExport_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionBrush_triggered();
    void on_actionMirror_triggered();
    void on_actionErase_triggered();
    void on_actionFlip_triggered();
    void on_actionFlip_2_triggered();
    void on_actionInvert_2_triggered();
    void on_actionClear_triggered();
    void on_action16_Pixel_Frame_triggered();
    void on_action32_Pixel_Frame_triggered();
    void on_action64_Pixel_Frame_triggered();

public slots:
    void updateSelectedFrameDisplay();
    void createEmptyFrame();
    void copyPreviousFrame();
    void updateSelectedFrameWithNewImage(QImage* img);
    void restartPreview();
    void previewAnimation();

private:
    Ui::MainWindow *ui;
    QListWidgetItem* selectedFrameItem;
    QColor color;
    QPixmap *currentFrame;
    QTimer *nextFrame;
    int previewFrame;
    int numFrames;
};

#endif // MAINWINDOW_H
