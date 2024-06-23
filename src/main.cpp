#include <QAction>
#include <QApplication>
#include <QFontDatabase>
#include <QMainWindow>

#include "clockwidget.h"
#include "config.h"

#include <utils.h>

class ClockWindow final : public QMainWindow {
public:
    explicit ClockWindow() {
        setCentralWidget(&mClockWidget);

        mExitAction.setShortcut(Qt::Key_Escape);
        connect(&mExitAction, &QAction::triggered,
            this, &QMainWindow::close);
        addAction(&mExitAction);

        mFullscreenAction.setShortcut(Qt::Key_F);
        connect(&mFullscreenAction, &QAction::triggered,
            this, &ClockWindow::onToggleFullscreen);
        addAction(&mFullscreenAction);
    }

protected:
    void showEvent(QShowEvent* event) override {
        QMainWindow::showEvent(event);

        if (mFirstShow) {
            mFirstShow = false;

            if (Config::get().startFullscreen())
                showFullScreen();
        }
    }

private:
    void onToggleFullscreen() {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    }

    ClockWidget mClockWidget;
    QAction mExitAction;
    QAction mFullscreenAction;
    bool mFirstShow = true;
};

int main(int argc, char** argv) {
    QApplication::setApplicationName("SunClock");
    QApplication::setOrganizationDomain("coresmith.com");

    QApplication app(argc, argv);

    const auto& conf = Config::get();

    // Font
    const auto fontFileName = ":/DMMono-Medium.ttf";

    const int fontId = QFontDatabase::addApplicationFont(fontFileName);
    utils::ASSERT_OR_EXIT(fontId >= 0, "Failed to load font: %1", fontFileName);
    QApplication::setFont(conf.fontName());
    // utils::ASSERT_OR_EXIT(fams.contains(conf.fontName()), "Font family not found: %1", conf.fontName());

    // Style
    app.setStyleSheet(QString("QMainWindow { background-color: %1; }")
        .arg(conf.backgroundColor().name()));

    // Get!
    ClockWindow win;
    win.resize(conf.windowWidth(), conf.windowHeight());

    win.show();

    return QApplication::exec();
}
