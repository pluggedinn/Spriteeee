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
    void displayFrameWidthQuestion();
    void updateToolButton(int button);
    void save();
    void load();

signals:
    void setSizeFrame(int);
    void toolClicked(int);
    void frameSelected(QImage*);
    void selectedColor(QColor);
    void undoButtonClicked();
    void redoButtonClicked();
    void updateFrameSize(int);

private slots:
    void on_brushToolButton_clicked();
    void on_flipToolButton_clicked();
    void on_colorsButton_pressed();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_mirrorButton_clicked();
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void on_eraseButton_clicked();

public slots:
    void updateSelectedFrameDisplay();

private:
    Ui::MainWindow *ui;
    void createEmptyFrame();

    void updateSelectedFrameWithNewImage(QImage* img);

    QListWidgetItem* selectedFrameItem;
    QColor color;
    QPixmap *currentFrame;
    bool isSaved;
    int numFrames;
};

#endif // MAINWINDOW_H
