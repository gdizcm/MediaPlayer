#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "videowidget.h"
#include <QFileDialog>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addToPlaylist(const QStringList &fileNames);

private slots:
    void openFile(QString filename = "");
    void playFile();
    void stopVideo();
    void positionChange(qint64 position);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void wheelEvent(QWheelEvent *event);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *m_player;
    QString m_fileName;
    QMediaPlaylist *m_playlist;
    QMediaPlayer::State m_playerState;
    bool b_moveSlider;
    QLabel *timeStatus;

};

#endif // MAINWINDOW_H
