#ifndef ICON_UTILS_HPP
#define ICON_UTILS_HPP

#include <QString>

QString get_icon_path(const QString& icon_name);

namespace IconUtils {
    inline QString get_icon_path(const QString& icon_name) {
        return ::get_icon_path(icon_name);
    }
}

#endif // ICON_UTILS_HPP
