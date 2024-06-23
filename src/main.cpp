#include <QAction>
#include <QApplication>
#include <QFontDatabase>
#include <QMainWindow>

#include "clockwidget.h"

#include "config.h"

class ClockWindow final : public QMainWindow {
public:
    explicit ClockWindow(const Config& config) : mClockWidget(config) {
        setCentralWidget(&mClockWidget);

        const auto action = new QAction("Exit", this);
        action->setShortcut(Qt::Key_Escape);
        connect(action, &QAction::triggered, [this] {
            close();
        });
        addAction(action);
    }

private:
    ClockWidget mClockWidget;
    QAction* mExitAction;
};


int main(int argc, char** argv) {
    QApplication::setApplicationName("SunClock");
    QApplication::setOrganizationDomain("coresmith.com");

    QApplication app(argc, argv);

    const Config conf;

    // Font
    QFontDatabase::addApplicationFont(":/DMMono-Medium.ttf");
    QApplication::setFont(conf.fontName());

    // Style
    app.setStyleSheet(QString("QMainWindow { background-color: %1; }").arg(conf.backgroundColor().name()));

    // Get!
    ClockWindow win(conf);
    win.resize(1280, 720);

    win.show();

    return app.exec();
}
