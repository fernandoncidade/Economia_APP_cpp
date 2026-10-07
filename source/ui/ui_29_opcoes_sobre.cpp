#include "ui_29_opcoes_sobre.hpp"
#include "../utils/ApplicationPathUtils.hpp"
#include "../utils/LogManager.hpp"

namespace OpcoesSobre {

const QString SITE_LICENSES =
    "https://www.gnu.org/licenses/lgpl-3.0.html.en\n"
    "https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.en\n"
    "https://opensource.org/licenses/BSD-2-Clause\n"
    "https://opensource.org/licenses/BSD-3-Clause\n"
    "https://www.apache.org/licenses/LICENSE-2.0\n"
    "https://python-pillow.org/\n"
    "https://opensource.org/licenses/MIT\n"
    "https://opensource.org/license/isc-license-txt\n";

QString get_license_text_pt_br() {
    return load_text_file("EULA_pt_BR - Economia.txt", "EULA");
}

QString get_license_text_en_us() {
    return load_text_file("EULA_en_US - Economia.txt", "EULA");
}

QString get_notice_text_pt_br() {
    return load_text_file("NOTICE_pt_BR.txt", "NOTICES");
}

QString get_notice_text_en_us() {
    return load_text_file("NOTICE_en_US.txt", "NOTICES");
}

QString get_about_text_pt_br() {
    return load_text_file("ABOUT_pt_BR.txt", "ABOUT");
}

QString get_about_text_en_us() {
    return load_text_file("ABOUT_en_US.txt", "ABOUT");
}

QString get_privacy_policy_pt_br() {
    return load_text_file("Privacy_Policy_pt_BR.txt", "PRIVACY_POLICY");
}

QString get_privacy_policy_en_us() {
    return load_text_file("Privacy_Policy_en_US.txt", "PRIVACY_POLICY");
}

QString get_history_app_pt_br() {
    return load_text_file("History_APP_pt_BR.txt", "ABOUT");
}

QString get_history_app_en_us() {
    return load_text_file("History_APP_en_US.txt", "ABOUT");
}

QString get_release_notes_pt_br() {
    return load_text_file("RELEASE NOTES_pt_BR.txt", "RELEASE");
}

QString get_release_notes_en_us() {
    return load_text_file("RELEASE NOTES_en_US.txt", "RELEASE");
}

} // namespace OpcoesSobre
