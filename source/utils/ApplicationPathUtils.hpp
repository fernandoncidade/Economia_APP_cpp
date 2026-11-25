#ifndef APPLICATION_PATH_UTILS_HPP
#define APPLICATION_PATH_UTILS_HPP

#include <QString>

QString get_app_base_path();
QString get_project_root();
QString get_text_file_path(const QString& filename, const QString& folder = QString());
QString load_text_file(const QString& filename, const QString& folder = QString(), const QString& encoding = "UTF-8");

namespace ApplicationPathUtils {
    inline QString get_app_base_path() { return ::get_app_base_path(); }
    inline QString get_project_root() { return ::get_project_root(); }
    inline QString get_text_file_path(const QString& filename, const QString& folder = QString()) {
        return ::get_text_file_path(filename, folder);
    }
    inline QString load_text_file(const QString& filename, const QString& folder = QString(), const QString& encoding = "UTF-8") {
        return ::load_text_file(filename, folder, encoding);
    }
}

#endif // APPLICATION_PATH_UTILS_HPP
