# Matriz de Conformidade — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-compliance-matrix--economia_app)

<a id="-conformidade-pt-br"></a>
## 🇧🇷 Conformidade Técnica, Legal e de Qualidade

Este documento consolida os controles de conformidade de engenharia, licenciamento e integridade mantidos no ciclo de release do **Economia_APP**.

| Área | Controle Aplicado | Evidência / Artefato |
|---|---|---|
| **Duplo Licenciamento** | Código-fonte sob **GNU AGPLv3**; binário oficial publicado na Microsoft Store (`9PLR0KD6KSJJ`) regido pelo **EULA** / **CLC** oficiais da loja | [`LICENSE`](../LICENSE), [`licensing.md`](./licensing.md), [`EULA`](../source/assets/EULA/EULA_pt_BR%20-%20Economia.txt) |
| **Privacidade / Zero Telemetria** | Execução 100% offline; zero coleta, telemetria, analytics ou tráfego de rede | [`privacy-policy.md`](./privacy-policy.md), [`Privacy_Policy`](../source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt) |
| **Segurança e Build** | Flags de hardening (`/GS`, `/guard:cf`, `/DYNAMICBASE`, `/NXCOMPAT`, `-fstack-protector-strong`, PIE) e isolamento estrito CRT MSVC vs MinGW | [`security-model.md`](./security-model.md), [`CMakeLists.txt`](../CMakeLists.txt) |
| **Sincronização de Versão** | Auditoria automatizada pelo utilitário `find_VersionEditor --check` | `mocks/find_VersionEditor.cpp`, suíte `test_10_tools_audit_and_docs_integrity` |
| **Testes Automatizados** | 10 módulos `Qt6::Test` / `CTest` cobrindo os 13 serviços financeiros, UI, renderização vetorial, PDF, i18n, módulos Qt e ABI PE64 | [`tests/CMakeLists.txt`](../tests/CMakeLists.txt) |
| **Distribuição Limpa** | Binários finais sem pacotes redundantes (`vc_redist.x64.exe` omitido dos `.iss`), mantendo apenas app-local CRT e plugins Qt6 estritamente necessários | [`library/Economia_APP.iss`](../library/Economia_APP.iss) |
| **Avisos de Terceiros** | Declaração integral do framework **Qt 6.12.0**, compiladores e licenças aplicáveis | [`NOTICE_pt_BR.txt`](../source/assets/NOTICES/NOTICE_pt_BR.txt) |

---

<a id="-compliance-matrix--economia_app"></a>
## 🇺🇸 Compliance Matrix — Economia_APP

- **Dual Licensing**: Public source repository governed by **GNU AGPLv3** ([`LICENSE`](../LICENSE)); official Microsoft Store release (`9PLR0KD6KSJJ`) governed by the official EULA and Custom License Contract (`source/assets/EULA/` and `source/assets/CLC/`).
- **Offline Privacy**: Zero telemetry, analytics, or external network connections; user state lives strictly inside `%LOCALAPPDATA%\Economia_APP`.
- **Automated Verification**: `find_VersionEditor --check` validates version metadata across `source/assets/`, root markdown files, `library/*.iss`, `library/AppxManifest.xml`, `library/Economia_APP_cpp.txt`, `docs/`, and UI dialogs; all 10 `CTest` suites run cleanly under both pure MSVC and MinGW toolchains.
