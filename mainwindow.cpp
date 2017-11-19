#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include <QIcon>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon icon;
    QPixmap qpm;

    if(qpm.load(":icons/brush.png"))
    {
        qDebug("hello");
        icon.addPixmap(qpm);
        ui->brushToolButton->setIcon(icon);
    }

    currentFrameNum = -1;
    spriteSize = 32;

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

void MainWindow::addFrame()
{
    QImage* emptyImage = new QImage(512,512, QImage::Format_ARGB32);
    QColor color(0,0,0,0);
    emptyImage->fill(color);
    frames.append(emptyImage);
}

void MainWindow::createEmptyFrame()
{
    numFrames++;
    currentFrame = new QPixmap(450,450);
    addFrame();

    QListWidgetItem* item = new QListWidgetItem(QString::number(numFrames), 0);
    item->setText(QString::number(numFrames));
    ui->framesListWidget->addItem(item);
    ui->framesListWidget->item(numFrames - 1)->setSelected(true);
    selectedFrameItem = item;
    currentFrame->convertFromImage(*frames.at(numFrames-1));
    QIcon currentIcon(currentFrame->copy(0,0,512,512));
    item->setIcon(currentIcon);
    currentFrameNum = numFrames - 1;
    ui->framesListWidget->update();
}


void MainWindow::updateToolButton(int button)
{
    emit toolClicked(button);
}


void MainWindow::updateSelectedFrameDisplay()
{
    QPixmap pxmap;
    pxmap.convertFromImage(*frames.at(currentFrameNum));
    QIcon currentIcon(pxmap.copy(0,0,512,512));
    selectedFrameItem->setIcon(currentIcon);

}


void MainWindow::updateSelectedFrameWithNewImage(QImage* img)
{
    frames.replace(currentFrameNum, img);
    updateSelectedFrameDisplay();
}

void MainWindow::export_to_gif()
{
    int counter = 0;
    QProcess proc;
    QString process = "convert";
    QStringList parameter_list;
    QString delay = QString::number(100.0/ (double) 10);
    parameter_list << "-dispose" << "background" << "-delay" << delay;

    QFileDialog *fileDialog = new QFileDialog;

    QString filename = fileDialog->getSaveFileName(this, tr("Save File"), "");

    for(QImage *img : frames)
    {
        QString msg = QString ("temp%1").arg(counter);
        img->save(msg, "PNG");
        parameter_list << msg;
        counter++;
    }
    QFileInfo convertFileInfo("/usr/bin/convert");

    parameter_list << filename+".gif";

    proc.start(process, parameter_list);
    if (!(proc.waitForFinished()))
    {
        qDebug() << "Conversion failed:" << proc.errorString();
    }
    else
    {
        qDebug() << "Conversion output:" << proc.readAll();
    }
}

void MainWindow::copyPreviousFrame()
{
    QImage *previousImage = new QImage(frames.at(currentFrameNum - 1)->copy(0,0,512,512));
    updateSelectedFrameWithNewImage(previousImage);
}

void MainWindow::previewAnimation()
{
    if (previewFrame < frames.size() - 1)
    {
        previewFrame++;
    }
    else
    {
        previewFrame = 0;
    }
    QImage* previewImage = frames[previewFrame];
    QImage preview;
    preview = (previewImage->scaled(128,128));
    ui->previewFrame->setAlignment(Qt::AlignCenter);
    ui->previewFrame->setPixmap(QPixmap::fromImage(preview));
    nextFrame->start(1000);
}

void MainWindow::restartPreview()
{
    previewAnimation();
}

void MainWindow::save()
{
    int totalWidth = 512 / spriteSize;
    int relativeWidth = 512 - totalWidth+1;

    QString data = "";
    for(QImage *img : frames)
    {
        for(int y = 1 ; y <= relativeWidth; y = y + totalWidth)
        {
            for(int x = 1 ; x <= relativeWidth; x = x + totalWidth)
            {
                QRgb rgbColor = QRgb(img->pixel(x, y));
                if(rgbColor != 0000) {
                    QColor color = QColor(rgbColor);
                    data += "" + QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " " + QString::number(color.alpha()) + " ";
                }
                else
                {
                    data += "0 0 0 0 ";
                }
            }
            data += "\n";
        }
    }

    QFileDialog *dialog = new QFileDialog;
    dialog->setDefaultSuffix("ssp");
    QString filename = dialog->getSaveFileName(this, tr("Save as"), "");

    QFile f(filename);
    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &f );
        stream << spriteSize << " " << spriteSize << "\n" << frames.size() << "\n" << data << "\n";
        f.close();;
    }
}


void MainWindow::load()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select a file to open", QDir::homePath());
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {
       frames.clear();
       while(ui->framesListWidget->count() > 0)
       {
           QListWidgetItem* x = ui->framesListWidget->takeItem(0);
           delete x;
       }

       QTextStream stream(&f);
       int framesCount = 0;
       int count = 0;
       while (!stream.atEnd() && count < 2)
       {
          QString line = stream.readLine();
          if(count == 0)
          {
              spriteSize = line.split(" ")[0].toInt();
          }
          else if (count == 1)
          {
              framesCount = line.toInt();
          }
          count++;
       }
       emit updateFrameSize(spriteSize);

       for (int frame = 0; frame < framesCount; frame++)
       {
           QImage *img = new QImage(512, 512, QImage::Format_ARGB32);
           for (int row = 0; row < spriteSize; row++)
           {
               QString line = stream.readLine();
               for (int column = 0; column < spriteSize; column++)
               {
                   QColor color;
                   int red;
                   int green;
                   int blue;
                   int alpha;
                   for (int i = 0; i < 4; i++)
                   {
                       if (i == 0)
                       {
                           red = line.split(" ")[column * 4].toInt();
                       }
                       else if(i == 1)
                       {
                           green = line.split(" ")[column * 4 + 1].toInt();
                       }
                       else if (i == 2)
                       {
                           blue = line.split(" ")[column * 4 + 2].toInt();
                       }
                       else if (i == 3)
                       {
                           alpha = line.split(" ")[column * 4 + 3].toInt();
                       }
                   }

                   color = QColor(red, green, blue, alpha);
                   int size = 512 / spriteSize;
                   int leftmostX = size*column; //the leftmost pixel in the column
                   int rightmostX = leftmostX + size; //the rightmost pixel in the column
                   int leftmostY = size*row; //the top pixel in the row
                   int rightmostY = leftmostY + size; //the bottom pixel in the row

                   for(int x = leftmostX; x < rightmostX; x++)
                   {
                       for(int y = leftmostY; y < rightmostY; y++)
                       {
                           img->setPixel(x,y,color.rgba());
                       }
                   }
               }
           }
           frames.append(img);
       }

       int numFrames = 0;
       for(int i = 0; i < frames.length() ; i++)
       {
           numFrames++;
           currentFrame = new QPixmap(512, 512);

           QListWidgetItem* item = new QListWidgetItem(QString::number(i+1), 0);
           item->setText(QString::number(i+1));
           ui->framesListWidget->addItem(item);
           currentFrame->convertFromImage(*frames.at(i));
           QIcon currentIcon(currentFrame->copy(0, 0, 512, 512));
           item->setIcon(currentIcon);
           repaint();
       }

       for(int i = 0; i < numFrames; i++)
       {
           selectedFrameItem = ui->framesListWidget->item(i);
           QPixmap pxmap;
           pxmap.convertFromImage(*frames.at(i));
           QIcon newIcon(pxmap.copy(0,0,512,512));
           selectedFrameItem->setIcon(newIcon);
           ui->framesListWidget->update();
       }

       selectedFrameItem =  ui->framesListWidget->item(0);
       selectedFrameItem->setSelected(true);
       emit frameSelected(frames.at(0));
    }
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
    ui->selectedColorButton->setStyleSheet("border-radius: 7px; background-color: " + color.name() + ";");
    emit selectedColor(color);
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
    emit clearFrameClicked();
}

void MainWindow::on_addFrameButton_clicked()
{
    createEmptyFrame();
    emit frameSelected(frames.at(currentFrameNum));
}

void MainWindow::on_duplicateFrameButton_clicked()
{
    createEmptyFrame();
    copyPreviousFrame();
    emit frameSelected(frames.at(currentFrameNum));
}

void MainWindow::on_deleteFrameButton_clicked()
{
    if(frames.count() > 1)
    {
         if (ui->framesListWidget->item(currentFrameNum)->text()  != "addFrame")
         {
            frames.removeAt(currentFrameNum);
            for(int i = 0; i < numFrames-1; i++)
            {
                selectedFrameItem = ui->framesListWidget->item(i);
                QPixmap pxmap;
                pxmap.convertFromImage(*frames.at(i));
                QIcon newIcon(pxmap.copy(0,0,512,512));
                selectedFrameItem->setIcon(newIcon);
            }

            QListWidgetItem *item = ui->framesListWidget->item(frames.length());
            delete item;
            if(currentFrameNum >= frames.count()){
                currentFrameNum--;
            }
            selectedFrameItem =  ui->framesListWidget->item(currentFrameNum);
            selectedFrameItem->setSelected(true);
            emit frameSelected(frames.at(currentFrameNum));

            numFrames--;
            repaint();
        }
    }
}


void MainWindow:: on_framesListWidget_itemClicked(QListWidgetItem *item)
{
    int newFrameNumber = item->text().toInt();
    currentFrameNum = newFrameNumber - 1;
    selectedFrameItem = item;
    updateSelectedFrameDisplay();

    emit frameSelected(frames.at(currentFrameNum));
}


void MainWindow::on_brushSizeSlider_valueChanged(int value)
{
    emit brushSliderMoved(value);
}

void MainWindow::on_invertButton_clicked()
{
    emit invertButtonClicked();
}

void MainWindow::on_actionSave_As_triggered()
{
    save();
}

void MainWindow::on_actionLoad_triggered()
{
    load();
}

void MainWindow::on_actionExport_triggered()
{
    export_to_gif();
}

void MainWindow::on_actionUndo_triggered()
{
    emit undoButtonClicked();
}

void MainWindow::on_actionRedo_triggered()
{
    emit redoButtonClicked();
}

void MainWindow::on_actionBrush_triggered()
{
    updateToolButton(1);
}

void MainWindow::on_actionMirror_triggered()
{
    updateToolButton(2);
}

void MainWindow::on_actionErase_triggered()
{
    updateToolButton(3);
}

void MainWindow::on_actionFlip_triggered()
{
    emit flipButtonClicked();
}

void MainWindow::on_actionFlip_2_triggered()
{
    emit flipButtonClicked();
}

void MainWindow::on_actionInvert_2_triggered()
{
    emit invertButtonClicked();
}

void MainWindow::on_actionClear_triggered()
{
    emit clearFrameClicked();
}

void MainWindow::on_action16_Pixel_Frame_triggered()
{
    newProject(16);
}

void MainWindow::on_action32_Pixel_Frame_triggered()
{
    newProject(32);
}

void MainWindow::on_action64_Pixel_Frame_triggered()
{
    newProject(64);
}

void MainWindow::newProject(int size)
{
    numFrames = 0;
    frames.clear();
    ui->framesListWidget->clear();
    createEmptyFrame();
    currentFrameNum = 0;
    spriteSize = size;
    emit setSizeFrame(size);
    emit frameSelected(frames.at(currentFrameNum));
}

void MainWindow::on_actionColors_triggered()
{
    color = QColorDialog::getColor();
    emit selectedColor(color);
}
