# Política de Privacidade — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-privacy-policy--economia_app)

<a id="-privacidade-pt-br"></a>
## 🇧🇷 Operação Local e Privacidade por Design

O **Economia_APP** foi projetado com compromisso estrito de **privacidade local**:

- **Zero coleta de dados**: O aplicativo não coleta, armazena remotamente ou transmite dados pessoais, parâmetros financeiros, históricos de cálculo ou informações de hardware.
- **Zero telemetria e zero rede ativa**: Não há servidores de analytics, SDKs de rastreamento, APIs de atualização automática ou tráfego de rede em segundo plano durante o uso regular dos módulos financeiros.
- **Armazenamento estritamente local (`%LOCALAPPDATA%\Economia_APP`)**:
  - `session_calculations.json`: restauração opcional de sessão anterior via `SessionManager`.
  - `font_config.json`: preferência local de tipografia (`FontManager`).
  - `idioma_config.json` / arquivo de idioma do `GerenciadorTraducao`: seleção de idioma (`pt_BR` / `en_US`).
  - Logs locais via `LogManager` exclusivamente para diagnóstico na máquina do usuário.
- **Exportação PDF (`PdfExport`)**: Os relatórios `.pdf` são gravados exclusivamente no caminho escolhido explicitamente pelo usuário via `QFileDialog`.
- Texto contratual completo empacotado: [`Privacy_Policy_pt_BR.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt) e [`Privacy_Policy_en_US.txt`](../source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt).

---

<a id="-privacy-policy--economia_app"></a>
## 🇺🇸 Privacy Policy — Economia_APP

- **100% Offline Execution**: Economia_APP collects no personal data, sends no telemetry or analytics, and maintains no background network connections.
- **Local-Only Storage**: Configuration, language preference, logs, and optional session snapshots reside solely inside the user's `%LOCALAPPDATA%\Economia_APP` directory. User-exported PDF reports are saved only where explicitly chosen by the user.
