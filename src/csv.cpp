#include "csv.h"

#include <QtDebug>

using namespace qcsv;

bool qcsv::readCsvRow(QTextStream& in, QStringList* row) {

    static const int delta[][5] = {
            //  ,    "   \n    ?  eof
            {1, 2, -1, 0, -1}, // 0: parsing (store char)
            {1, 2, -1, 0, -1}, // 1: parsing (store column)
            {3, 4, 3,  3, -2}, // 2: quote entered (no-op)
            {3, 4, 3,  3, -2}, // 3: parsing inside quotes (store char)
            {1, 3, -1, 0, -1}, // 4: quote exited (no-op)
            // -1: end of row, store column, success
            // -2: eof inside quotes
    };

    row->clear();

    if (in.atEnd())
        return false;

    int state = 0, t;
    char ch;
    QString cell;

    while (state >= 0) {

        if (in.atEnd())
            t = 4;
        else {
            in >> ch;
            if (ch == ',') t = 0;
            else if (ch == '\"') t = 1;
            else if (ch == '\n') t = 2;
            else t = 3;
        }

        state = delta[state][t];

        if (state == 0 || state == 3) {
            cell += ch;
        } else if (state == -1 || state == 1) {
            row->append(cell);
            cell = "";
        }

    }

    if (state == -2)
        throw std::runtime_error("End-of-file found while inside quotes.");

    return true;

}

