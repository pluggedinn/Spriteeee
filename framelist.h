#ifndef FRAMELIST_H
#define FRAMELIST_H

#include <QObject>

class FrameList : public QObject
{
public:
    explicit FrameList(QObject *parent = 0);

    QList<QImage*> frames;
    int currentFrame;
    int spriteSize;
    void addFrame();
    QString getSaveoutput();

signals:

public slots:

private:
    Q_OBJECT

};

#endif // FRAMELIST_H
