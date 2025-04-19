#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QAction>

#include "config.h"
#include "gridwidget.h"
#include "clockfaces/abstractclockface.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow(Config& conf);
    ~MainWindow();



private:
    void updateTime();
    void onTileActivated(int index);
    void toggleClockFace(int faceIndex);
    QList<AbstractClockFace*> createClockFaces(const ClockData& data);
protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QTimer mTimer;
    GridWidget mGridWidget;
    Config& mConf;
    QAction mExitAction;
    QList<QList<AbstractClockFace*>> mClockFaces;
};
