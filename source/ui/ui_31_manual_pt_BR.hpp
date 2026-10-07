#ifndef UI_31_MANUAL_PT_BR_HPP
#define UI_31_MANUAL_PT_BR_HPP

#include "ui_30_Manual.hpp"
#include <QList>
#include <QString>

namespace source::ui::manual_pt_BR {

QString manual_intro_text();
QString manual_toc_title();
QList<source::ui::Manual::ManualSection> get_manual_document();

} // namespace source::ui::manual_pt_BR

#endif // UI_31_MANUAL_PT_BR_HPP
