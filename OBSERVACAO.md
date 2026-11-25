# Observações Técnicas da Implementação C++23 (`Economia_APP_cpp`)

Versão documentada: `2026.11.22.0`
Data do snapshot técnico: `22 de novembro de 2026`
Base de compilação: `C++23` · `Qt 6.12.0` · `MSVC (Visual Studio 18 2026 MSBuild x64, v145)` e `MinGW-w64 (GCC 15.1.0)`

---

## 1. Registro de Arquitetura e Padrões Aplicados

- **Renderização Vetorial de Radicais e Frações (`source/utils/MathRenderer.cpp`, `TextFormat.cpp`)**:
  - Implementação nativa baseada em `QTextObjectInterface` + `QPainter`, eliminando geração de PNGs temporários e garantindo nitidez vetorial tanto no `HistoryContainer` quanto na impressão em PDF via `QPrinter`.
- **Subíndices HTML/Unicode em Labels e Cabeçalhos (`ui_12`, `ui_14`, `TextFormat`)**:
  - `label_tma_real` e `label_tma_nominal` renderizam subíndices ricos via `TextFormat::to_html_subscripts` (`i<sub>r</sub>`, `i<sub>a</sub>`), enquanto cabeçalhos tabulares do módulo CAUE (`VRₙ (R$)`, `Comₙ (R$)`, `CAUEₙ (R$)`) utilizam `TextFormat::to_unicode_subscripts` com fidelidade em runtime `pt_BR` e `en_US`.
- **Cadeia MSVC Puro e Isolamento Estrito CRT**:
  - Os builds MSVC foram migrados integralmente do gerador Ninja para o gerador nativo `Visual Studio 18 2026` MSBuild x64 (`build/build_Economia_APP_msvc`, `build/build_mocks_msvc`, `build/build_suite_msvc`).
  - Tanto os deploys MSVC quanto MinGW removem automaticamente DLLs da cadeia concorrente via `cmake/prune_optional_root_runtime_dlls.cmake`.
- **Distribuição Externa Limpa e Instaladores Inno Setup 7 (`library/*.iss`)**:
  - Os manifestos `.iss` empacotam apenas os binários, bibliotecas Qt6 mínimas, plugins essenciais (`platforms/qwindows.dll`, `styles/qmodernwindowsstyle.dll`, `imageformats/qico.dll`), traduções `.qm`, ativos documentais e DLLs app-local do runtime correspondente — omitindo o executável redundante `vc_redist.x64.exe`.
- **Suíte Profissional de Testes (`tests/`)**:
  - Os 10 alvos `CTest` validam os 13 serviços financeiros, casos-limite matemáticos, subíndices tipográficos, renderização vetorial em `QImage`, exportação real `%PDF`, persistência isolada de sessão, diálogos (`SobreDialog` / `Manual`), troca dinâmica `i18n`, módulos/plugins do `Qt 6.12.0`, integridade PE64/CRT e auditoria completa de documentação/versão (`find_VersionEditor --check`), sem distribuição externa.

---

## 2. Documentação Técnica Correlata

Consulte [`README.md`](./README.md), [`MANUAL.md`](./MANUAL.md) e o diretório [`docs/`](./docs/architecture.md) (`architecture.md`, `compliance.md`, `licensing.md`, `privacy-policy.md`, `security-model.md`, `user-guide.md`, `release-v2026.11.22.0.md`, `suggested-issues.md`, `technical-audit-and-roadmap.md`).

**Root Path:** `Economia_APP_cpp`

```
.gitattributes
.gitignore
CMakeLists.txt
CODE_OF_CONDUCT.md
CONTRIBUTING.md
LICENSE
main.cpp
MANUAL.md
OBSERVACAO.md
README.md
SECURITY.md
```

**Root Path:** `Economia_APP_cpp\source`

```
├── 📁 assets
│   ├── 📁 ABOUT
│   │   ├── 📄 ABOUT_en_US.txt
│   │   ├── 📄 ABOUT_pt_BR.txt
│   │   ├── 📄 History_APP_en_US.txt
│   │   └── 📄 History_APP_pt_BR.txt
│   ├── 📁 CLC
│   │   ├── 📄 CLC_en_US - Economia.txt
│   │   └── 📄 CLC_pt_BR - Economia.txt
│   ├── 📁 COPYRIGHT
│   │   ├── 📄 AVISO DE COPYRIGHT E MARCA REGISTRA_pt_BR.txt
│   │   └── 📄 COPYRIGHT AND TRADEMARK NOTICE_en_US.txt
│   ├── 📁 EULA
│   │   ├── 📄 EULA_en_US - Economia.txt
│   │   └── 📄 EULA_pt_BR - Economia.txt
│   ├── 📁 LICENSES
│   │   ├── 📄 Apache License Version 2.0.txt
│   │   ├── 📄 BSD License Clause-2.txt
│   │   ├── 📄 BSD License Clause-3.txt
│   │   ├── 📄 GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 ISC LICENSE.txt
│   │   ├── 📄 MIT License.txt
│   │   ├── 📕 MS-Store-ADA-v8.9-FINAL-EN.pdf
│   │   └── 📄 PYTHON SOFTWARE FOUNDATION LICENSE.txt
│   ├── 📁 NOTICES
│   │   ├── 📄 NOTICE_en_US.txt
│   │   └── 📄 NOTICE_pt_BR.txt
│   ├── 📁 PRIVACY_POLICY
│   │   ├── 📄 Privacy_Policy_en_US.txt
│   │   └── 📄 Privacy_Policy_pt_BR.txt
│   ├── 📁 RELEASE
│   │   ├── 📄 RELEASE NOTES_en_US.txt
│   │   └── 📄 RELEASE NOTES_pt_BR.txt
│   └── 📁 icones
│       ├── 📄 Economia_APP.rc
│       ├── 📄 economia.ico
│       ├── 🖼️ economia.png
│       ├── 🖼️ economia_1440-2160.png
│       ├── 🖼️ economia_150-150.png
│       ├── 🖼️ economia_2160-2160.png
│       ├── 🖼️ economia_300-300.png
│       └── 🖼️ economia_71-71.png
├── 📁 language
│   ├── 📁 translations
│   │   ├── 📄 economia_en_US.qm
│   │   ├── 📄 economia_en_US.ts
│   │   ├── 📄 economia_pt_BR.qm
│   │   └── 📄 economia_pt_BR.ts
│   ├── ⚡ tr_01_gerenciadorTraducao.cpp
│   ├── ⚡ tr_01_gerenciadorTraducao.hpp
│   ├── ⚡ tr_02_compileTranslations.cpp
│   └── ⚡ tr_02_compileTranslations.hpp
├── 📁 services
│   ├── ⚡ sv_01_calculate_interest.cpp
│   ├── ⚡ sv_01_calculate_interest.hpp
│   ├── ⚡ sv_02_calculate_annuity.cpp
│   ├── ⚡ sv_02_calculate_annuity.hpp
│   ├── ⚡ sv_03_calculate_gradient.cpp
│   ├── ⚡ sv_03_calculate_gradient.hpp
│   ├── ⚡ sv_04_calculate_real_rate_equivalence.cpp
│   ├── ⚡ sv_04_calculate_real_rate_equivalence.hpp
│   ├── ⚡ sv_05_calculate_amortization.cpp
│   ├── ⚡ sv_05_calculate_amortization.hpp
│   ├── ⚡ sv_06_calculate_investment.cpp
│   ├── ⚡ sv_06_calculate_investment.hpp
│   ├── ⚡ sv_07_calculate_depreciation.cpp
│   ├── ⚡ sv_07_calculate_depreciation.hpp
│   ├── ⚡ sv_08_calculate_effective_rate.cpp
│   ├── ⚡ sv_08_calculate_effective_rate.hpp
│   ├── ⚡ sv_09_calculate_minimum_return.cpp
│   ├── ⚡ sv_09_calculate_minimum_return.hpp
│   ├── ⚡ sv_10_calculate_fisher.cpp
│   ├── ⚡ sv_10_calculate_fisher.hpp
│   ├── ⚡ sv_11_calculate_value_at_k.cpp
│   ├── ⚡ sv_11_calculate_value_at_k.hpp
│   ├── ⚡ sv_12_calculate_vpl_with_taxes.cpp
│   ├── ⚡ sv_12_calculate_vpl_with_taxes.hpp
│   ├── ⚡ sv_13_calculate_caue.cpp
│   └── ⚡ sv_13_calculate_caue.hpp
├── 📁 ui
│   ├── ⚡ ui_01_create_layout.cpp
│   ├── ⚡ ui_01_create_layout.hpp
│   ├── ⚡ ui_02_get_float_from_line_edit.cpp
│   ├── ⚡ ui_02_get_float_from_line_edit.hpp
│   ├── ⚡ ui_03_create_interest_tab.cpp
│   ├── ⚡ ui_03_create_interest_tab.hpp
│   ├── ⚡ ui_04_create_annuity_tab.cpp
│   ├── ⚡ ui_04_create_annuity_tab.hpp
│   ├── ⚡ ui_05_create_gradient_tab.cpp
│   ├── ⚡ ui_05_create_gradient_tab.hpp
│   ├── ⚡ ui_06_create_rates_tab.cpp
│   ├── ⚡ ui_06_create_rates_tab.hpp
│   ├── ⚡ ui_07_create_amortization_tab.cpp
│   ├── ⚡ ui_07_create_amortization_tab.hpp
│   ├── ⚡ ui_08_create_investment_tab.cpp
│   ├── ⚡ ui_08_create_investment_tab.hpp
│   ├── ⚡ ui_09_create_depreciation_tab.cpp
│   ├── ⚡ ui_09_create_depreciation_tab.hpp
│   ├── ⚡ ui_10_create_effective_rate_tab.cpp
│   ├── ⚡ ui_10_create_effective_rate_tab.hpp
│   ├── ⚡ ui_11_create_minimum_return_tab.cpp
│   ├── ⚡ ui_11_create_minimum_return_tab.hpp
│   ├── ⚡ ui_12_create_fisher_tab.cpp
│   ├── ⚡ ui_12_create_fisher_tab.hpp
│   ├── ⚡ ui_13_create_vpl_tax_tab.cpp
│   ├── ⚡ ui_13_create_vpl_tax_tab.hpp
│   ├── ⚡ ui_14_create_caue_tab.cpp
│   ├── ⚡ ui_14_create_caue_tab.hpp
│   ├── ⚡ ui_15_generate_sac_table.cpp
│   ├── ⚡ ui_15_generate_sac_table.hpp
│   ├── ⚡ ui_16_generate_sam_table.cpp
│   ├── ⚡ ui_16_generate_sam_table.hpp
│   ├── ⚡ ui_17_generate_price_table.cpp
│   ├── ⚡ ui_17_generate_price_table.hpp
│   ├── ⚡ ui_18_generate_hamburgues_table.cpp
│   ├── ⚡ ui_18_generate_hamburgues_table.hpp
│   ├── ⚡ ui_19_generate_american_table.cpp
│   ├── ⚡ ui_19_generate_american_table.hpp
│   ├── ⚡ ui_20_generate_caue_input_table.cpp
│   ├── ⚡ ui_20_generate_caue_input_table.hpp
│   ├── ⚡ ui_21_set_amort_table_row.cpp
│   ├── ⚡ ui_21_set_amort_table_row.hpp
│   ├── ⚡ ui_22_get_table_data.cpp
│   ├── ⚡ ui_22_get_table_data.hpp
│   ├── ⚡ ui_23_history_container.cpp
│   ├── ⚡ ui_23_history_container.hpp
│   ├── ⚡ ui_24_font_config_dialog.cpp
│   ├── ⚡ ui_24_font_config_dialog.hpp
│   ├── ⚡ ui_25_export_pdf.cpp
│   ├── ⚡ ui_25_export_pdf.hpp
│   ├── ⚡ ui_26_menu_bar.cpp
│   ├── ⚡ ui_26_menu_bar.hpp
│   ├── ⚡ ui_27_SobreDialog.cpp
│   ├── ⚡ ui_27_SobreDialog.hpp
│   ├── ⚡ ui_28_exibir_sobre.cpp
│   ├── ⚡ ui_28_exibir_sobre.hpp
│   ├── ⚡ ui_29_opcoes_sobre.cpp
│   ├── ⚡ ui_29_opcoes_sobre.hpp
│   ├── ⚡ ui_30_Manual.cpp
│   ├── ⚡ ui_30_Manual.hpp
│   ├── ⚡ ui_31_manual_pt_BR.cpp
│   ├── ⚡ ui_31_manual_pt_BR.hpp
│   ├── ⚡ ui_32_manual_en_US.cpp
│   ├── ⚡ ui_32_manual_en_US.hpp
│   ├── ⚡ ui_33_exibir_manual.cpp
│   └── ⚡ ui_33_exibir_manual.hpp
├── 📁 utils
│   ├── ⚡ ApplicationPathUtils.cpp
│   ├── ⚡ ApplicationPathUtils.hpp
│   ├── ⚡ CaminhoPersistenteUtils.cpp
│   ├── ⚡ CaminhoPersistenteUtils.hpp
│   ├── ⚡ DialogHelper.cpp
│   ├── ⚡ DialogHelper.hpp
│   ├── ⚡ FontManager.cpp
│   ├── ⚡ FontManager.hpp
│   ├── ⚡ IconUtils.cpp
│   ├── ⚡ IconUtils.hpp
│   ├── ⚡ LogManager.cpp
│   ├── ⚡ LogManager.hpp
│   ├── ⚡ MathRenderer.cpp
│   ├── ⚡ MathRenderer.hpp
│   ├── ⚡ SessionManager.cpp
│   ├── ⚡ SessionManager.hpp
│   ├── ⚡ TextFormat.cpp
│   ├── ⚡ TextFormat.hpp
│   ├── ⚡ TrialManager.cpp
│   └── ⚡ TrialManager.hpp
├── ⚡ fca_01_FinancialCalculatorAPP.cpp
└── ⚡ fca_01_FinancialCalculatorAPP.hpp
```

**Root Path:** `Economia_APP_cpp\tests`

```
├── 📄 CMakeLists.txt
├── ⚡ TestSupport.hpp
├── ⚡ test_01_core_interest_annuity_gradient.cpp
├── ⚡ test_02_rates_fisher_minreturn_effrate.cpp
├── ⚡ test_03_amortization_depreciation.cpp
├── ⚡ test_04_investment_vpl_taxes_caue.cpp
├── ⚡ test_05_textformat_mathrenderer_typography.cpp
├── ⚡ test_06_history_session_pdf_and_persistence.cpp
├── ⚡ test_07_ui_dialogs_about_manual_and_i18n.cpp
├── ⚡ test_08_qt_plugins_modules_and_cpp23.cpp
├── ⚡ test_09_toolchain_isolation_and_binaries.cpp
└── ⚡ test_10_tools_audit_and_docs_integrity.cpp
```

**Root Path:** `Economia_APP_cpp\mocks`

```
├── 📁 icones
│   ├── 📄 economia.ico
│   ├── 🖼️ economia.png
│   └── 📄 mocks.rc
├── 📄 CMakeLists.txt
├── ⚡ CompileTranslationsGUI.cpp
├── ⚡ CompileTranslationsGUI.hpp
├── ⚡ CompileTranslationsTerminal.cpp
├── ⚡ CompileTranslationsTerminal.hpp
├── ⚡ find_VersionEditor.cpp
└── ⚡ find_VersionEditor.hpp
```

**Root Path:** `Economia_APP_cpp\images\interfaces`

```
├── 📁 en_US
│   ├── 🖼️ Captura de tela 2025-10-21 155020.png
│   ├── 🖼️ Captura de tela 2025-10-21 155041.png
│   ├── 🖼️ Captura de tela 2025-10-21 155104.png
│   ├── 🖼️ Captura de tela 2025-10-21 155119.png
│   ├── 🖼️ Captura de tela 2025-10-21 155137.png
│   ├── 🖼️ Captura de tela 2025-10-21 155155.png
│   └── 🖼️ Captura de tela 2025-10-21 155215.png
└── 📁 pt_BR
    ├── 🖼️ Captura de tela 2025-10-21 154812.png
    ├── 🖼️ Captura de tela 2025-10-21 154832.png
    ├── 🖼️ Captura de tela 2025-10-21 154849.png
    ├── 🖼️ Captura de tela 2025-10-21 154905.png
    ├── 🖼️ Captura de tela 2025-10-21 154924.png
    ├── 🖼️ Captura de tela 2025-10-21 154944.png
    └── 🖼️ Captura de tela 2025-10-21 155001.png
```

**Root Path:** `Economia_APP_cpp\docs`

```
├── 📁 images
│   ├── 🖼️ banner.png
│   └── 🖼️ logo.png
├── 📝 architecture.md
├── 📝 compliance.md
├── 📝 licensing.md
├── 📝 privacy-policy.md
├── 📝 release-v2026.11.22.0.md
├── 📝 security-model.md
├── 📝 suggested-issues.md
├── 📝 technical-audit-and-roadmap.md
└── 📝 user-guide.md
```

**Root Path:** `Economia_APP_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```
