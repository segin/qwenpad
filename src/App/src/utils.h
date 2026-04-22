#ifndef UTILS_H
#define UTILS_H

#include <QString>

namespace Utils {
    inline QString convertLineEndings(const QString &text, int target) {
        QString result = text;

        switch (target) {
        case 0:
            result.replace("\r\n", "\n").replace("\r", "\n");
            break;
        case 1:
            result.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
            break;
        case 2:
            result.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r");
            break;
        }

        return result;
    }
}

#endif // UTILS_H
