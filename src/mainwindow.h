#pragma once

#include <QAction>
#include <QMainWindow>
#include <QTimer>

#include "clockfaces/abstractclockface.h"
#include "config.h"
#include "gridwidget.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(Config& conf);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateTime();
    void onTileActivated(int index);
    void toggleClockFace(int faceIndex);
    QList<AbstractClockFace*> createClockFaces(ClockData& data);

    QTimer mTimer;
    GridWidget mGridWidget;
    Config& mConf;
    QAction mExitAction;
    QList<QList<AbstractClockFace*>> mClockFaces;
};
