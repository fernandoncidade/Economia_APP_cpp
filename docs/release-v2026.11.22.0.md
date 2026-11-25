# Notas de Lançamento — Economia_APP v2026.11.22.0

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`

[🇺🇸 English version below](#-release-notes--economia_app-v202611220)

<a id="-lancamento-pt-br"></a>
## 🇧🇷 Destaques da Release

1. **Engenharia C++23 & Qt 6.12.0 Dual Toolchain (MSVC puro + MinGW)**:
   - Migração completa da cadeia MSVC do gerador Ninja para **MSVC puro** (`Visual Studio 18 2026` MSBuild x64, toolset `v145,host=x64,version=14.52.37025`, Windows SDK `10.0.28000.0`), mantendo paridade com **MinGW-w64** (`GCC 15.1.0`) e builds padronizados em `build/build_Economia_APP_msvc` e `build/build_mocks_msvc`.
2. **Suíte Completa de Testes Automatizados (`tests/`)**:
   - Criação de **10 suítes `Qt6::Test` / `CTest`** integradas ao VS Code (`Qt6: Compilar e Executar SUITE (MSVC)` e `Qt6: Compilar e Executar SUITE (MinGW)`) cobrindo os 13 serviços financeiros, renderização vetorial `MathRenderer`, exportação PDF, sessão salva, diálogos e verificação PE64/CRT — sem distribuição externa.
3. **Correção Tipográfica de Subíndices e Mapeamento Reverso**:
   - Correção dos subíndices HTML/Unicode nas abas **Equação de Fisher** (`i<sub>r</sub>`, `i<sub>a</sub>`) e **CAUE - Vida Econômica** (`VRₙ (R$)`, `Comₙ (R$)`, `CAUEₙ (R$)`) nos dois idiomas, além da correção de precedência ASCII em `TextFormat::from_unicode_subscripts` e `from_unicode_superscripts`.
4. **Reorganização `library/`, `images/interfaces/` e `find_VersionEditor` Unificado**:
   - Movimentação dos scripts Inno Setup 7, `AppxManifest.xml` e `Economia_APP_cpp.txt` para `library/`; inclusão das 14 capturas de tela da GUI (`images/interfaces/pt_BR/` e `en_US/`) no `README.md`; e expansão do `find_VersionEditor` para auditar/sincronizar 35 alvos (incluindo `docs/`, arquivos raiz e renomeação automática de `docs/release-v*.md`).
5. **Documentação Completa, Duplo Licenciamento e Instaladores Enxutos**:
   - Portabilidade completa da documentação bilíngue em `docs/` e raiz (GNU AGPLv3 no código-fonte + Microsoft Store EULA/CLC `9PLR0KD6KSJJ`), atualização do Manual Integrado (`ui_31` / `ui_32`) e omissão de `vc_redist.x64.exe` redundante nos instaladores `.iss`.

---

<a id="-release-notes--economia_app-v202611220"></a>
## 🇺🇸 Release Notes — Economia_APP v2026.11.22.0

**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

- Pure **MSVC** (`Visual Studio 18 2026` MSBuild x64, `v145`, Windows SDK `10.0.28000.0`) and **MinGW-w64** (`GCC 15.1.0`) **C++23 + Qt 6.12.0** builds under `build/build_Economia_APP_msvc` and `build/build_mocks_msvc`, paired with a 10-suite self-contained `Qt6::Test` / `CTest` engineering suite (`tests/`).
- Verified HTML & Unicode subscripts in **Fisher Equation** (`iᵣ`, `iₐ`) and **CAUE / EAC** (`VRₙ`, `Comₙ`, `CAUEₙ`), plus `TextFormat` reverse-subscript ASCII precedence fix and built-in bilingual Manual synchronization (`ui_31` / `ui_32`).
- Reorganized packaging manifests under `library/`, added 14 GUI screenshots under `images/interfaces/{pt_BR,en_US}/` to `README.md`, upgraded `find_VersionEditor` across all 35 tracked targets (including automatic `docs/release-v*.md` renaming), and documented the dual-licensing model (`GNU AGPLv3` + Microsoft Store EULA/CLC `9PLR0KD6KSJJ`).
