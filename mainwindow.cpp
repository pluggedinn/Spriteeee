#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "framelist.h"
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QColorDialog>
#include <QColor>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numFrames = 0;
    createEmptyFrame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createEmptyFrame() {
    numFrames++;
    currentFrame = new QPixmap(450,450);
    sprite.addFrame();

    QListWidgetItem* item = new QListWidgetItem(QString::number(numFrames), 0);
    item->setText(QString::number(numFrames));
    ui->framesListWidget->addItem(item);
    ui->framesListWidget->item(numFrames - 1)->setSelected(true);
    selectedFrameItem = item;
    currentFrame->convertFromImage(*sprite.frames.at(numFrames-1));
    QIcon currentIcon(currentFrame->copy(0,0,448,448));
    item->setIcon(currentIcon);
    sprite.currentFrame = numFrames - 1;
    ui->framesListWidget->update();

}

/**
 * @brief MainWindow::displayFrameWidthQuestion
 * Shows a prompt that let you choose between the diffent size of the frames available.
 */
void MainWindow::displayFrameWidthQuestion() {
    QMessageBox question;
    question.setText(trUtf8("New Project"));
    question.setInformativeText("Choose the frame size");
    QAbstractButton *size64 = question.addButton(trUtf8("64"), QMessageBox::YesRole);
    QAbstractButton *size32 = question.addButton(trUtf8("32"), QMessageBox::NoRole);
    QAbstractButton *size16 = question.addButton(trUtf8("16"), QMessageBox::ResetRole);
    question.setIcon(QMessageBox::Question);
    question.exec();

    if (question.clickedButton() == size16) {
        emit setSizeFrame(16);
        sprite.width = 16;
    } else if (question.clickedButton() == size32) {
        emit setSizeFrame(32);
        sprite.width = 32;
    } else {
        emit setSizeFrame(64);
        sprite.width = 64;
    }
}

void MainWindow::updateToolButton(int button)
{
    // brush
    if (button == 1)
    {
        ui->brushToolButton->setCheckable(true);
        ui->flipToolButton->setCheckable(false);
        ui->mirrorButton->setCheckable(false);

    }
    // flip
    else if (button == 2)
    {
        ui->brushToolButton->setCheckable(false);
        ui->flipToolButton->setCheckable(true);
        ui->mirrorButton->setCheckable(false);

    }
    // mirror
    else if (button == 3) {
        ui->brushToolButton->setCheckable(false);
        ui->flipToolButton->setCheckable(false);
        ui->mirrorButton->setCheckable(true);
    }

    ui->brushToolButton->update();
    ui->flipToolButton->update();
    ui->mirrorButton->update();

    emit toolClicked(button);
}

/**
 * Method used to save current Image to Frames Window
 * @brief MainWindow::updateSelectedFrameDisplay
 */
void MainWindow::updateSelectedFrameDisplay()
{
    QPixmap pxmap;
    pxmap.convertFromImage(*sprite.frames.at(sprite.currentFrame));
    QIcon currentIcon(pxmap.copy(0,0,448,448));
    selectedFrameItem->setIcon(currentIcon);
    isSaved = false;

}

void MainWindow::updateSelectedFrameWithNewImage(QImage* img)
{
    sprite.frames.replace(sprite.currentFrame, img);
    updateSelectedFrameDisplay();
    isSaved = false;
}

/**
 * @brief MainWindow::save
 * @param saveAs
 * Saves the file to a .ssp file
 */
void MainWindow::save()
{
    QString output = sprite.getSaveoutput();
    QFileDialog *fileDialog = new QFileDialog;
    fileDialog->setDefaultSuffix("ssp");

    QString filename = fileDialog->getSaveFileName(this, tr("Save File"), "", "Text files (*.ssp)");

    QFile f(filename);
    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &f );
        stream << sprite.width << " " << sprite.width << "\n";
        stream << sprite.frames.size() << "\n";
        stream << output << "\n";
        f.close();
        isSaved = true;
    }
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_saveButton_clicked()
{
    save();
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_brushToolButton_clicked()
{
    updateToolButton(1);
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_flipToolButton_clicked()
{
    updateToolButton(2);
}

void MainWindow::on_mirrorButton_clicked()
{
    updateToolButton(3);
}

void MainWindow::on_colorsButton_pressed()
{
    color = QColorDialog::getColor();
    emit selectedColor(color);
    ui->colorsButton->setChecked(false);
}

void MainWindow::on_undoButton_clicked()
{
    emit undoButtonClicked();
    isSaved = false;

}

void MainWindow::on_redoButton_clicked()
{
    emit redoButtonClicked();
    isSaved = false;

}


