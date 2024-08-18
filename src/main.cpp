#include "clockapp.h"

int main(int argc, char* argv[]) {
    ClockApp app;
    while (app.isRunning()) {
        app.update();
    }
    return 0;
}