#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H
#include <QVideoWidget>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();
};

#endif // VIDEOWIDGET_H
//DirectShowPlayerService::doRender: Unresolved error code 0x80040266 ()
