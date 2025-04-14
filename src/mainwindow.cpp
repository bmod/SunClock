#include "mainwindow.h"

#include "clockfaces/clockbasic.h"
#include "clockfaces/clockusa.h"

MainWindow::MainWindow(Config& conf) : mConf(conf) {
    mExitAction.setShortcut(Qt::Key_Escape);
    addAction(&mExitAction);
    connect(&mExitAction, &QAction::triggered, [this] { close(); });

    setCentralWidget(&mGridWidget);


    const auto& clocks = conf.clocks();
    for (int i = 0, len = clocks.size(); i < len; ++i) {
        auto faces = createClockFaces(*clocks[i].get());
        mClockFaces.append(faces);
        mGridWidget.setFace(i, faces[0]);
    }

    connect(&mTimer, &QTimer::timeout, this, &MainWindow::onTimer);
        mTimer.setInterval(1000);
    mTimer.start();

    connect(&mGridWidget, &GridWidget::tileClicked, this, &MainWindow::onTileActivated);

}

MainWindow::~MainWindow() {
    for (auto faces: mClockFaces) {
        for (const auto face: faces) {
            face->deleteLater();
        }
    }
}

void MainWindow::onTimer() {
    const auto time = QDateTime::currentDateTimeUtc();
    for (auto& clock: mConf.clocks()) {
        clock->setTime(time);
    }
}

void MainWindow::onTileActivated(int index) {
    // toggleClockFace(index);
}

void MainWindow::toggleClockFace(int index) {
    auto& faces = mClockFaces.at(index);
    const auto face = mGridWidget.face(index);
    const auto faceIndex = faces.indexOf(face);
    const auto newIndex = (faceIndex + 1) % faces.size();

    qDebug() << "New Index: " << newIndex;
    // Set new face index
    const auto newFace = faces.at(newIndex);
    mGridWidget.setFace(index, newFace);
}

QList<AbstractClockFace*> MainWindow::createClockFaces(const ClockData& data) {
    QList<AbstractClockFace*> clockFaces;
    clockFaces << new ClockBasic(data);
    clockFaces << new ClockUsa(data);
    return clockFaces;
}