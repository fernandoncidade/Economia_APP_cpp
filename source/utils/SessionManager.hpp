#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include <QString>

class FinancialCalculatorApp;

namespace SessionManager {

QString get_session_file_path();
bool has_saved_session();
bool save_session(FinancialCalculatorApp* app);
bool load_session(FinancialCalculatorApp* app);
void clear_session();

} // namespace SessionManager

#endif // SESSION_MANAGER_HPP
