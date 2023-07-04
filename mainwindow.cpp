#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QMimeData>
#include <QTime>
#include <QWheelEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("://cd"));
    setAcceptDrops(true);

    m_player = new QMediaPlayer;
    m_playlist = new QMediaPlaylist;
    m_player->setPlaylist(m_playlist);

    m_player->setVideoOutput(ui->widget);

    b_moveSlider = false;
    //绑定信号槽
    connect(ui->bt_Open, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->bt_Play, SIGNAL(clicked()), this, SLOT(playFile()));
    connect(ui->bt_Stop, SIGNAL(clicked()), this, SLOT(stopVideo()));

    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [=] (QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::MediaStatus::EndOfMedia)
        {
            playFile();
        }
    });
    timeStatus = new QLabel(this);
    timeStatus->setFrameStyle(QFrame::Box|QFrame::Sunken);
    ui->statusBar->addPermanentWidget(timeStatus);
    ui->progressBar->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToPlaylist(const QStringList &fileNames)
{
    foreach (QString const &name, fileNames)
    {
        QFileInfo fileInfo(name);
        if (fileInfo.exists())
        {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == "m3u")
            {
                m_playlist->load(url);
            }
            else
            {
                m_playlist->addMedia(url);
            }
        }
    }
}

void MainWindow::openFile(QString filename)
{
    QStringList fileNames;
    if (filename.length() > 0)
    {
        fileNames.append(filename);
    }
    else {
        fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"));
    }
    if (fileNames.length() <= 0)
    {
        return;
    }

    int index = m_playlist->currentIndex();
    if (index >= 0)
    {
        m_player->pause();
        m_playerState = QMediaPlayer::PausedState;
        m_playlist->removeMedia(index);
    }

    addToPlaylist(fileNames);

    playFile();
    positionChange(0);
    setWindowTitle(m_player->currentMedia().canonicalUrl().fileName());//在title处显示文件名
}

void MainWindow::playFile()
{
    if (m_player->playlist()->isEmpty())
    {
        return;
    }
    if (m_playerState != QMediaPlayer::PlayingState)
    {
        m_playerState = QMediaPlayer::PlayingState;
        m_player->play();
        ui->bt_Play->setText(tr("暂停"));
    }
    else
    {
        m_playerState = QMediaPlayer::PausedState;
        m_player->pause();
        ui->bt_Play->setText(tr("播放"));
    }

}

void MainWindow::stopVideo()
{
    m_playerState = QMediaPlayer::StoppedState;
    m_player->stop();
    ui->bt_Play->setText(tr("播放"));
    positionChange(0);
}
void MainWindow::positionChange(qint64 position)
{

    if(m_player->duration()/1000 != ui->progressBar->maximum())
    {
        ui->progressBar->setMaximum(static_cast<int>(m_player->duration()/1000));
    }
    ui->progressBar->setValue(static_cast<int>(position/1000));

    QTime initTime(0, 0, 0);
    QTime curtime = initTime.addMSecs(static_cast<int>(position));
    QTime totaltime = initTime.addMSecs(static_cast<int>(m_player->duration()));
    timeStatus->clear();
    timeStatus->setText(curtime.toString() + "/" +totaltime.toString());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
    QWidget::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    openFile(urls.first().toLocalFile());
    QWidget::dropEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    int volume = m_player->volume();
    if (numDegrees.y() > 0)
    {
       volume++;
    }
    else
    {
        volume--;
    }
    m_player->setVolume(volume);
    QMainWindow::wheelEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->progressBar)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            //获取当前鼠标位置
            int currentX = static_cast<QMouseEvent*>(event)->x();
            //计算当前位置占整个slider的百分比
            double percentage = currentX*1.0/ui->progressBar->width();
            //计算具体只
            int value = percentage*(ui->progressBar->maximum() - ui->progressBar->minimum()) + ui->progressBar->minimum();
            ui->progressBar->setValue(value);

            qDebug()<<ui->progressBar->value();
            qint64 newValue = ui->progressBar->value() * 1000;
            if (!m_player->currentMedia().isNull())
            {
                m_player->setPosition(newValue);
            }

        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
