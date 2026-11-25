#ifndef UI_32_MANUAL_EN_US_HPP
#define UI_32_MANUAL_EN_US_HPP

#include "ui_30_Manual.hpp"
#include <QList>
#include <QString>

namespace source::ui::manual_en_US {

QString manual_intro_text();
QString manual_toc_title();
QList<source::ui::Manual::ManualSection> get_manual_document();

} // namespace source::ui::manual_en_US

#endif // UI_32_MANUAL_EN_US_HPP
