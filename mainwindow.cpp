#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QMimeData>

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

   /* connect(m_player, &QMediaPlayer::positionChanged, [this](qint64 position){
        if(m_player->duration() != ui->progressBar->maximum())
        {
            ui->progressBar->setMaximum(m_player->duration()/1000);
        }
        qDebug()<<"position changed "<<position/1000;
        ui->progressBar->setValue(position/1000);
    });*/
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));

    connect(ui->progressBar, &QSlider::sliderMoved,[this](){
        disconnect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));
        qint64 newValue = ui->progressBar->value() * 1000;
        m_player->setPosition(newValue);
        b_moveSlider = true;
    });

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
    if(m_player->duration() != ui->progressBar->maximum())
    {
        ui->progressBar->setMaximum(static_cast<int>(m_player->duration()/1000));
    }
    ui->progressBar->setValue(static_cast<int>(position/1000));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (b_moveSlider)
    {
        connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));
    }
    b_moveSlider = false;
    QMainWindow::mouseReleaseEvent(event);
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
