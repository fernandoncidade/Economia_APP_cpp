# Manual Técnico e Operacional — `Economia_APP`

Versão documentada: `2026.11.22.0`
Data: `22 de novembro de 2026`
Plataforma: `Windows 10/11 (x64)` · `C++23` · `Qt 6.12.0 Widgets`
Autor: **Fernando Nillsson Cidade**

---

## 1. Visão Geral do Aplicativo

O **`Economia_APP`** integra 12 abas de Engenharia Econômica e Matemática Financeira com renderização vetorial nativa de fórmulas (`MathRenderer`), cartões editáveis (`HistoryContainer`), persistência local (`SessionManager`), troca de idioma em runtime (`pt_BR` / `en_US`) e exportação offline em PDF (`PdfExport`).

Para o guia funcional detalhado das 12 abas, consulte também [`docs/user-guide.md`](./docs/user-guide.md).

---

## 2. Compilação, Execução e Suíte de Testes

### 2.1 Construção do Aplicativo e Ferramentas (`MSVC puro` e `MinGW`)

```powershell
# Aplicativo Principal — MSVC Puro (Visual Studio 18 2026 MSBuild x64) -> build/build_Economia_APP_msvc
cmake --preset host-qt6-msvc-release
cmake --build --preset host-qt6-msvc-release --target Economia_APPDeploy --parallel

# Aplicativo Principal — MinGW-w64 GCC 15.1.0 -> build/build_Economia_APP_mingw
cmake --preset host-qt6-mingw-release
cmake --build --preset host-qt6-mingw-release --target Economia_APPDeploy --parallel

# Mocks / Ferramentas de Suporte (MSVC Puro e MinGW)
cmake --preset mocks-msvc-release
cmake --build --preset mocks-msvc-release --target MocksDeploy --parallel
cmake --preset mocks-mingw-release
cmake --build --preset mocks-mingw-release --target MocksDeploy --parallel
```

### 2.2 Suíte Completa de Testes (`tests/` — Sem Distribuição Externa)

```powershell
# MSVC Puro -> build/build_suite_msvc
cmake --preset suite-msvc-release
cmake --build --preset suite-msvc-release --target EconomiaSuiteDeploy --parallel
ctest --preset suite-msvc-release --output-on-failure

# MinGW -> build/build_suite_mingw
cmake --preset suite-mingw-release
cmake --build --preset suite-mingw-release --target EconomiaSuiteDeploy --parallel
ctest --preset suite-mingw-release --output-on-failure
```

No VS Code, utilize as tarefas:
- `Qt6: Compilar e Executar SUITE (MSVC)`
- `Qt6: Compilar e Executar SUITE (MinGW)`

### 2.3 Verificação de Sincronização de Versões

```powershell
build\build_mocks_msvc\find_VersionEditor.exe --check
build\build_mocks_msvc\find_VersionEditor.exe --apply 2026.11.22.0 "22 de novembro de 2026" 2026.11.22.0 "November 22, 2026"
```

---

## 3. Persistência Local e Licenciamento

- Os dados do usuário são gravados estritamente em `%LOCALAPPDATA%\Economia_APP` (`session_calculations.json`, `font_config.json`, preferências de idioma e logs).
- O projeto utiliza **duplo licenciamento**: código-fonte sob **GNU AGPLv3** ([`LICENSE`](./LICENSE)) e pacote oficial da Microsoft Store (`9PLR0KD6KSJJ`) sob **EULA/CLC** ([`docs/licensing.md`](./docs/licensing.md)), enquanto os manifestos de empacotamento e instaladores residem em [`library/`](./library/Economia_APP.iss).
