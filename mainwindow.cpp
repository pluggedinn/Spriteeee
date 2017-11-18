#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "framelist.h"
#include "gif.h"
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QColorDialog>
#include <QColor>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numFrames = 0;
    createEmptyFrame();

    nextFrame = new QTimer();
    connect(nextFrame, SIGNAL(timeout()), this, SLOT(previewAnimation()));

    previewAnimation();
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
    QIcon currentIcon(currentFrame->copy(0,0,512,512));
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
        ui->brushToolButton->setChecked(true);
        ui->flipToolButton->setChecked(false);
        ui->eraseButton->setChecked(false);

    }
    // flip
    else if (button == 2)
    {
        ui->brushToolButton->setChecked(false);
        ui->flipToolButton->setChecked(true);
        ui->eraseButton->setChecked(false);

    }
    // erase
    else if (button == 3) {
        ui->brushToolButton->setChecked(false);
        ui->flipToolButton->setChecked(false);
        ui->eraseButton->setChecked(true);
    }

    ui->brushToolButton->update();
    ui->flipToolButton->update();
    ui->eraseButton->update();

    emit toolClicked(button);
}


void MainWindow::updateSelectedFrameDisplay()
{
    QPixmap pxmap;
    pxmap.convertFromImage(*sprite.frames.at(sprite.currentFrame));
    QIcon currentIcon(pxmap.copy(0,0,512,512));
    selectedFrameItem->setIcon(currentIcon);

}


void MainWindow::updateSelectedFrameWithNewImage(QImage* img)
{
    sprite.frames.replace(sprite.currentFrame, img);
    updateSelectedFrameDisplay();
}

void MainWindow::export_to_gif() {
    int counter = 0;
    QProcess proc;
    QString process = "convert";
    QStringList parameter_list;
    QString delay = QString::number(100.0/ (double) 10);
    parameter_list << "-dispose" << "background" << "-delay" << delay;

    QFileDialog *fileDialog = new QFileDialog;
//    fileDialog->setDefaultSuffix("gif");

    QString filename = fileDialog->getSaveFileName(this, tr("Save File"), "");

    for(QImage *img : sprite.frames) {
        QString msg = QString ("temp%1").arg(counter);
        img->save(msg, "PNG");
        parameter_list << msg;
        counter++;
    }
    QFileInfo convertFileInfo("/usr/bin/convert");

    parameter_list << filename+".gif";

    proc.start(process, parameter_list);
    if (!(proc.waitForFinished()))
        qDebug() << "Conversion failed:" << proc.errorString();
    else
        qDebug() << "Conversion output:" << proc.readAll();
}

void MainWindow::copyPreviousFrame() {
    QImage *previousImage = new QImage(sprite.frames.at(sprite.currentFrame - 1)->copy(0,0,448,448));
    updateSelectedFrameWithNewImage(previousImage);
}

void MainWindow::previewAnimation()
{
    qDebug("preview updated");
    if (previewFrame < sprite.frames.size() - 1)
        previewFrame++;
    else
        previewFrame = 0;
    QImage* previewImage = sprite.frames[previewFrame];
    QImage preview;
    preview = (previewImage->scaled(128,128));
    ui->previewFrame->setAlignment(Qt::AlignCenter);
    ui->previewFrame->setPixmap(QPixmap::fromImage(preview));
    nextFrame->start(1000);
}

void MainWindow::restartPreview()
{
    qDebug("and");
    previewAnimation();
}

void MainWindow::save()
{
    QString output = sprite.getSaveoutput();
    QFileDialog *fileDialog = new QFileDialog;
    fileDialog->setDefaultSuffix("ssp");

    QString filename = fileDialog->getSaveFileName(this, tr("Save File"), "");

    QFile f(filename);
    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &f );
        stream << sprite.width << " " << sprite.width << "\n";
        stream << sprite.frames.size() << "\n";
        stream << output << "\n";
        f.close();;
    }
}

/**
 * @brief MainWindow::open
 * Opens a .ssp file and converts it into the frames that comprise a project
 */
void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select a file to open", QDir::homePath());
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {

       sprite.frames.clear();

       QTextStream in(&f);
       int framesCount = 0;
       int count = 0;
       while (!in.atEnd() && count < 2)
       {
          QString line = in.readLine();
          if(count == 0)
          {
              sprite.width = line.split(" ")[0].toInt();
              emit updateFrameSize(sprite.width);
          }
          else if (count == 1)
          {
              framesCount = line.toInt();
          }
          count++;
       }

       for (int frame = 0; frame < framesCount; frame++)
       {
           QImage *img = new QImage(512, 512, QImage::Format_ARGB32);
           for (int row = 0; row < sprite.width; row++){
               QString line = in.readLine();
               for (int column = 0; column < sprite.width; column++)
               {
                   QColor color;
                   int red;
                   int green;
                   int blue;
                   int alpha;
                   for (int l = 0; l < 4; l++) {
                       switch(l) {
                       case 0:
                           red = line.split(" ")[column * 4].toInt();
                           break;
                       case 1:
                           green = line.split(" ")[column * 4 + 1].toInt();
                           break;
                       case 2:
                           blue = line.split(" ")[column * 4 + 2].toInt();
                           break;
                       case 3:
                           alpha = line.split(" ")[column * 4 + 3].toInt();
                           break;
                       }
                   }
                   color = QColor(red, green, blue, alpha);
                   int pixelWidth = 512 / sprite.width;
                   int startingXPixel = pixelWidth*column; //the leftmost pixel in the column
                   int endingXPixel = startingXPixel + pixelWidth; //the rightmost pixel in the column
                   int startingYPixel = pixelWidth*row; //the top pixel in the row
                   int endingYPixel = startingYPixel + pixelWidth; //the bottom pixel in the row

                   for(int x = startingXPixel; x < endingXPixel; x++)
                   {
                       for(int y = startingYPixel; y < endingYPixel; y++)
                       {
                           img->setPixel(x,y,color.rgba());
                       }
                   }
               }
           }
           sprite.frames.append(img);
       }

       while(ui->framesListWidget->count() > 0)
       {
           QListWidgetItem* x = ui->framesListWidget->takeItem(0);
           delete x;
       }

       int numFrames = 0;
       for(int i = 0; i < sprite.frames.length() ; i++)
       {
           numFrames++;
           currentFrame = new QPixmap(512, 512);

           QListWidgetItem* item = new QListWidgetItem(QString::number(i+1), 0);
           item->setText(QString::number(i+1));
           ui->framesListWidget->addItem(item);
           currentFrame->convertFromImage(*sprite.frames.at(i));
           QIcon currentIcon(currentFrame->copy(0, 0, 512, 512));
           item->setIcon(currentIcon);
           repaint();
       }

       for(int i = 0; i < numFrames; i++)
       {
           selectedFrameItem = ui->framesListWidget->item(i);
           QPixmap pxmap;
           pxmap.convertFromImage(*sprite.frames.at(i));
           QIcon newIcon(pxmap.copy(0,0,512,512));
           selectedFrameItem->setIcon(newIcon);
       }

       selectedFrameItem =  ui->framesListWidget->item(0);
       selectedFrameItem->setSelected(true);
       emit frameSelected(sprite.frames.at(0));

       ui->framesListWidget->update();
    }
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_loadButton_clicked()
{
    load();
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_saveButton_clicked()
{
    save();
}

void MainWindow::on_exportButton_clicked() {
    export_to_gif();
}

/**
 * @brief MainWindow::on_paintbrushToolButton_clicked
 * Selects the paint brush when clicked
 */
void MainWindow::on_brushToolButton_clicked()
{
    updateToolButton(1);
}

void MainWindow::on_mirrorButton_clicked()
{
    updateToolButton(2);
}


void MainWindow::on_eraseButton_clicked()
{
    updateToolButton(3);
}

void MainWindow::on_flipToolButton_clicked()
{
    emit flipButtonClicked();
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
}

void MainWindow::on_redoButton_clicked()
{
    emit redoButtonClicked();
}

void MainWindow::on_clearButton_clicked()
{
    emit clearFrameClicked();//    QImage mirrored = image->mirrored(true, false);
    //    *image = mirrored;
    //    frame = this->image;
    //    this->pixmap = new QPixmap();
    //    pixmap->convertFromImage(*this->image);
    //    this->setPixmap(QPixmap::fromImage(*image));
    //    emit updateCurrentFrameDisplay();
}

void MainWindow::on_addFrameButton_clicked()
{
    createEmptyFrame();
    emit frameSelected(sprite.frames.at(sprite.currentFrame));
}

void MainWindow::on_duplicateFrameButton_clicked() {
    createEmptyFrame();
    copyPreviousFrame();
    emit frameSelected(sprite.frames.at(sprite.currentFrame));
}

void MainWindow::on_deleteFrameButton_clicked()
{
    if(sprite.frames.count() > 1)
    {
         if (ui->framesListWidget->item(sprite.currentFrame)->text()  != "addFrame")
         {
            sprite.frames.removeAt(sprite.currentFrame);
            for(int i = 0; i < numFrames-1; i++)
            {
                selectedFrameItem = ui->framesListWidget->item(i);
                QPixmap pxmap;
                pxmap.convertFromImage(*sprite.frames.at(i));
                QIcon newIcon(pxmap.copy(0,0,512,512));
                selectedFrameItem->setIcon(newIcon);
            }

            QListWidgetItem *item = ui->framesListWidget->item(sprite.frames.length());
            delete item;
            if(sprite.currentFrame >= sprite.frames.count()){
                sprite.currentFrame--;
            }
            selectedFrameItem =  ui->framesListWidget->item(sprite.currentFrame);
            selectedFrameItem->setSelected(true);
            emit frameSelected(sprite.frames.at(sprite.currentFrame));

            numFrames--;
            repaint();
        }
    }
}


void MainWindow:: on_framesListWidget_itemClicked(QListWidgetItem *item)
{
    updateSelectedFrameDisplay();

    int newFrameNumber = item->text().toInt();
    sprite.currentFrame = newFrameNumber - 1;
    selectedFrameItem = item;

    updateSelectedFrameDisplay();

    emit frameSelected(sprite.frames.at(sprite.currentFrame));
}


void MainWindow::on_brushSizeSlider_valueChanged(int value)
{
    emit brushSliderMoved(value);
}

void MainWindow::on_invertButton_clicked()
{
    emit invertButtonClicked();
}
