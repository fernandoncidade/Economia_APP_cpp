# Auditoria Técnica e Roadmap — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-technical-audit-and-roadmap)

<a id="-auditoria-pt-br"></a>
## 🇧🇷 Estado Atual da Auditoria Técnica

| Eixo Técnico | Diagnóstico | Status Atual |
|---|---|---|
| **Migração Python/PySide6 -> C++23/Qt 6.12.0** | Todos os 13 serviços financeiros, utilitários e diálogos operam nativamente em C++23 | Concluído |
| **Toolchain MSVC Puro (MSBuild x64)** | Build migrado de `_msvc_ninja` para `Visual Studio 18 2026` MSBuild (`build_Economia_APP_msvc`, `build_mocks_msvc`) | Concluído |
| **Renderização Matemática Vetorial (`MathRenderer`)** | Desenho vetorial via `QTextObjectInterface` + `QPainter` em tela e PDF | Concluído |
| **Subíndices HTML/Unicode (Fisher e CAUE) e Mapeamento Reverso** | Rótulos `i<sub>r</sub>`, `i<sub>a</sub>`, `VRₙ`, `Comₙ`, `CAUEₙ` corrigidos e precedência ASCII restaurada em `TextFormat::from_unicode_*` | Concluído |
| **Organização `library/` e `images/interfaces/` + `find_VersionEditor`** | Artefatos movidos para `library/`, galerias `pt_BR`/`en_US` adicionadas ao `README.md` e auditoria unificada em 35 alvos com renomeação automática de `release-v*.md` | Concluído |
| **Suíte Automatizada (`tests/`)** | 10 executáveis `Qt6::Test` / `CTest` em `build_suite_msvc` e `build_suite_mingw` sem deploy externo | Concluído |
| **Distribuição Externa e Instaladores** | Deploy mínimo app-local e `.iss` sem `vc_redist.x64.exe` redundante | Concluído |
| **Duplo Licenciamento e Documentação** | AGPLv3 (repo) + Microsoft Store EULA/CLC (`9PLR0KD6KSJJ`) alinhados em `LICENSE`, `docs/` e `source/assets/` | Concluído |

### Próximos Marcos de Roadmap
- Manutenção contínua das compilações MSVC (`v145`) e MinGW (`GCC 15.1.0`) alinhadas a atualizações da série Qt 6.12+.
- Expansão incremental de cenários extremos na suíte `tests/` conforme novos recursos financeiros forem adicionados.

---

<a id="-technical-audit-and-roadmap"></a>
## 🇺🇸 Technical Audit and Roadmap

- Pure C++23 / Qt 6.12.0 native architecture across all 13 financial engines, math rendering, PDF export, and dual MSVC/MinGW pipelines.
- Clean pure-MSVC MSBuild build layout (`build_Economia_APP_msvc`, `build_mocks_msvc`, `build_suite_msvc`) paired with MinGW-w64 builds, minimal runtime deployments, zero redundant `vc_redist.x64.exe` installer bloat, and a 10-part `CTest` engineering suite.
