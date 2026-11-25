![Economia_APP Banner](./docs/images/banner.png)

> **Observação:** Este repositório contém a implementação nativa em C++23 com Qt 6.12.0 do projeto `Economia_APP`.

# Economia_APP — Calculadora Financeira e Engenharia Econômica (`C++23` / `Qt 6.12.0`)

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus)
![Qt 6.12.0](https://img.shields.io/badge/Qt-6.12.0-41CD52?style=flat-square&logo=qt)
![Platform Windows x64](https://img.shields.io/badge/Platform-Windows%20x64-0078D6?style=flat-square&logo=windows)
![License AGPLv3 + Store EULA](https://img.shields.io/badge/Dual%20License-AGPLv3%20%7C%20Store%20EULA-blue?style=flat-square)

Versão: v2026.11.22.0<br>
Data técnica desta revisão: 22 de novembro de 2026<br>
Autor: **Fernando Nillsson Cidade** · `fernando@nillsson.com.br`<br>
Microsoft Store (Distribuição Oficial): [`9PLR0KD6KSJJ`](https://apps.microsoft.com/detail/9PLR0KD6KSJJ)

[🇺🇸 English Documentation below](#-english-documentation) · [📖 Manual do Usuário (`MANUAL.md`)](./MANUAL.md) · [🔍 Nota Técnica (`OBSERVACAO.md`)](./OBSERVACAO.md)

---

## 🇧🇷 Visão Geral e Recursos

O **Economia_APP** automatiza cálculos completos de Matemática Financeira e Engenharia Econômica em interface desktop nativa **C++23 + Qt 6.12.0 Widgets**, com 12 abas analíticas integradas:

1. **Juros Simples e Compostos** (Montante $F$, Capital $P$ e comparativo direto JS vs JC)
2. **Séries de Pagamentos / Anuidades** (Postecipada, Antecipada e taxa zero)
3. **Séries Gradientes** (Aritmético, Geométrico, Perpetuidade, termos $G_k$ e $X_k$)
4. **Equivalência de Taxas e Taxa Real** (Conversão temporal e Fisher)
5. **Sistemas de Amortização** (**Price**, **SAC**, **SAM**, **Americano** e **Hamburguês** com carência/capitalização, consulta por parcela $k$ e tabelas vinculadas ao cartão)
6. **Análise de Investimentos** (VPL/VAUE uniforme e detalhado, Payback descontado e Análise de Sensibilidade)
7. **Depreciação** (Linear, Soma dos Dígitos Decrescente/Crescente e Saldo Declinante)
8. **Taxa Efetiva, TIR, TIRm, Taxa Global, Cobrança Antecipada, TMA vs Rentabilidade e Juros Reais** (7 modos integrados)
9. **Retorno Mínimo** (TMA e recuperação mínima do investimento)
10. **Equação de Fisher** (Cálculo nominal/real com subíndices HTML/Unicode $i_r$ e $i_a$)
11. **VPL com Impostos e Financiamento** (IRPJ, CSLL, depreciação, parcelamento, residual e venda antecipada)
12. **CAUE — Vida Econômica** (Tabela interativa $VR_n$, $Com_n$, $CAUE_n$ e indicação ótima automática)

### Recursos Transversais de Engenharia
- **Renderização Vetorial Matemática (`MathRenderer`)**: Frações e radicais desenhados via `QTextObjectInterface` + `QPainter`, adaptados a temas Claro/Escuro e exportados em **PDF** (`Qt6::PrintSupport`).
- **Cartões Independentes no Histórico (`HistoryContainer`)**: Edição inline, exclusão seletiva, salvamento/restauração em `%LOCALAPPDATA%\Economia_APP\session_calculations.json` e troca dinâmica de idioma (`pt_BR` / `en_US`).
- **Execução 100% Offline**: Zero telemetria, zero rede ativa.

### 📸 Capturas de Tela da Interface (`pt-BR`)

Capturas reais da interface gráfica do **Economia_APP** localizado em **Português (Brasil)** armazenadas em [`images/interfaces/pt_BR/`](images/interfaces/pt_BR) (`1919×1079 px`), demonstrando os módulos de cálculo, barras de menu, controles de histórico e tabelas:

<p align="center">
  <strong>Juros Simples e Compostos (`pt-BR`) — Cálculo de Montante ($F$) / Juros Compostos, entradas, histórico e exportação PDF</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154812.png" alt="Economia_APP Português (BR) - Juros Simples e Compostos" width="900">
</p>

<p align="center">
  <strong>Anuidades (`pt-BR`) — Cálculo de Prestação ($A$), Série Postecipada / Antecipada</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154832.png" alt="Economia_APP Português (BR) - Anuidades" width="900">
</p>

<p align="center">
  <strong>Gradientes (`pt-BR`) — Gradiente Aritmético ($G$) / Geométrico ($g$), Valor Presente ($P$)</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154849.png" alt="Economia_APP Português (BR) - Gradientes" width="900">
</p>

<p align="center">
  <strong>Conversão de Taxas (`pt-BR`) — Equivalência de Taxas Efetivas e Taxa Real e Aparente (Inflação $\theta$)</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154905.png" alt="Economia_APP Português (BR) - Conversão de Taxas" width="900">
</p>

<p align="center">
  <strong>Amortização (`pt-BR`) — Sistema Francês (Price) com tabela (`Período (k)`, `Prestação`, `Juros`, `Amortização`, `Saldo Devedor`)</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154924.png" alt="Economia_APP Português (BR) - Amortização" width="900">
</p>

<p align="center">
  <strong>Análise de Investimentos (`pt-BR`) — Investimento Inicial, Fluxo de Caixa Líquido Periódico, TMA e Cálculo de VPL e VAUE</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20154944.png" alt="Economia_APP Português (BR) - Análise de Investimentos" width="900">
</p>

<p align="center">
  <strong>Depreciação (`pt-BR`) — Método Linear, Valor Residual Estimado (VRE), Vida Útil ($N$) e análise do ano específico $k$</strong><br><br>
  <img src="images/interfaces/pt_BR/Captura%20de%20tela%202025-10-21%20155001.png" alt="Economia_APP Português (BR) - Depreciação" width="900">
</p>

---

## Compilação e Suíte de Testes (`MSVC puro` e `MinGW`)

| Finalidade | Preset CMake (`CMakePresets.json`) | Diretório de Build |
|---|---|---|
| Aplicativo — **MSVC Puro** (`Visual Studio 18 2026` MSBuild x64, `v145`, SDK `10.0.28000.0`) | `host-qt6-msvc-release` | `build/build_Economia_APP_msvc` |
| Aplicativo — **MinGW** (`GCC 15.1.0`) | `host-qt6-mingw-release` | `build/build_Economia_APP_mingw` |
| Ferramentas (`find_VersionEditor`, `CompileTranslationsTerminal`) — **MSVC Puro** | `mocks-msvc-release` | `build/build_mocks_msvc` |
| Ferramentas — **MinGW** | `mocks-mingw-release` | `build/build_mocks_mingw` |
| **Suíte Completa (10 conjuntos CTest / Qt6Test)** — **MSVC Puro** | `suite-msvc-release` | `build/build_suite_msvc` |
| **Suíte Completa (10 conjuntos CTest / Qt6Test)** — **MinGW** | `suite-mingw-release` | `build/build_suite_mingw` |

Tarefas configuradas no VS Code (`.vscode/tasks.json`):
- **`Qt6: Compilar e Executar SUITE (MSVC)`**
- **`Qt6: Compilar e Executar SUITE (MinGW)`**

> As compilações de teste (`build_suite_msvc` e `build_suite_mingw`) são autocontidas dentro de `build/` e **não realizam distribuição externa**. Os instaladores em `library/*.iss` omitem `vc_redist.x64.exe` pois o deploy MSVC já inclui app-local exclusivamente as DLLs mínimas do runtime.

---

## Documentação Completa (`docs/`)

| Documento | Descrição |
|---|---|
| [`docs/architecture.md`](./docs/architecture.md) | Arquitetura em camadas, módulos `source/`, fluxo matemático e toolchains |
| [`docs/user-guide.md`](./docs/user-guide.md) | Guia prático das 12 abas analíticas e exportação PDF |
| [`docs/licensing.md`](./docs/licensing.md) | Detalhes formais do **duplo licenciamento** (`GNU AGPLv3` + Microsoft Store EULA/CLC) |
| [`docs/compliance.md`](./docs/compliance.md) | Matriz de conformidade técnica, legal e auditoria de versão |
| [`docs/privacy-policy.md`](./docs/privacy-policy.md) | Garantia de operação 100% offline e persistência local |
| [`docs/security-model.md`](./docs/security-model.md) | Flags de hardening (`/GS`, `/guard:cf`, ASLR/DEP) e isolamento estrito CRT |
| [`docs/release-v2026.11.22.0.md`](./docs/release-v2026.11.22.0.md) | Notas completas da versão `v2026.11.22.0` |
| [`docs/suggested-issues.md`](./docs/suggested-issues.md) | Roadmap de evolução funcional sugerido |
| [`docs/technical-audit-and-roadmap.md`](./docs/technical-audit-and-roadmap.md) | Auditoria técnica consolidada |
| Governança do Repositório | [`CODE_OF_CONDUCT.md`](./CODE_OF_CONDUCT.md) · [`CONTRIBUTING.md`](./CONTRIBUTING.md) · [`SECURITY.md`](./SECURITY.md) |

---

## Duplo Licenciamento

- **Código-Fonte Público**: Regido pela **GNU Affero General Public License v3.0 (GNU AGPLv3)** — veja [`LICENSE`](./LICENSE).
- **Distribuição Oficial Microsoft Store (`9PLR0KD6KSJJ`)**: Regida pelo **EULA** ([`source/assets/EULA/`](./source/assets/EULA/EULA_pt_BR%20-%20Economia.txt)) e **CLC** ([`source/assets/CLC/`](./source/assets/CLC/CLC_pt_BR%20-%20Economia.txt)).
- **Caráter Auxiliar `"AS IS"`**: Todos os cálculos têm caráter de apoio técnico/educacional sem garantia profissional implícita.

---

<a id="-english-documentation"></a>
## 🇺🇸 English Documentation

> **Note:** This repository contains the native C++23 with Qt 6.12.0 implementation of the `Economia_APP` project.

Version: v2026.11.22.0<br>
Technical revision date: November 22, 2026<br>
Author: **Fernando Nillsson Cidade** · `fernando@nillsson.com.br`<br>
Microsoft Store Official Listing: [`9PLR0KD6KSJJ`](https://apps.microsoft.com/detail/9PLR0KD6KSJJ)

**Economia_APP** is a 100% offline, dual-licensed Windows x64 Financial Mathematics & Engineering Economics calculator built with **C++23** and **Qt 6.12.0**, supporting both pure **MSVC** (`Visual Studio 18 2026` MSBuild x64, toolset `v145`, Windows SDK `10.0.28000.0`) and **MinGW-w64** (`GCC 15.1.0`) toolchains, complete with vector math rendering (`MathRenderer`), PDF reporting, editable calculation cards, live `pt_BR`/`en_US` translation switching, and a 10-part `Qt6::Test`/`CTest` engineering suite (`tests/`).

### 📸 Interface Screenshots (`en-US`)

Actual captures of **Economia_APP** localized in **English (US)** stored under [`images/interfaces/en_US/`](images/interfaces/en_US) (`1919×1079 px`), showcasing calculation modules, menu bar, editable history controls, and tables:

<p align="center">
  <strong>Simple and Compound Interest (`en-US`) — Calculate Amount ($F$) / Compound Interest, inputs, history &amp; PDF export</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155020.png" alt="Economia_APP English (US) - Simple and Compound Interest" width="900">
</p>

<p align="center">
  <strong>Annuities (`en-US`) — Calculate Payment ($A$), Ordinary / Due series</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155041.png" alt="Economia_APP English (US) - Annuities" width="900">
</p>

<p align="center">
  <strong>Gradients (`en-US`) — Arithmetic Gradient ($G$) / Geometric ($g$), Present Value ($P$)</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155104.png" alt="Economia_APP English (US) - Gradients" width="900">
</p>

<p align="center">
  <strong>Rate Conversion (`en-US`) — Effective Rate Equivalence &amp; Real / Apparent Rate (Inflation $\theta$)</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155119.png" alt="Economia_APP English (US) - Rate Conversion" width="900">
</p>

<p align="center">
  <strong>Amortization (`en-US`) — French System (Price) schedule table (`Period (k)`, `Installment`, `Interest`, `Amortization`, `Outstanding Balance`)</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155137.png" alt="Economia_APP English (US) - Amortization" width="900">
</p>

<p align="center">
  <strong>Investment Analysis (`en-US`) — Initial Investment, Periodic Net Cash Flow, MARR &amp; Calculate NPV and EUAW</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155155.png" alt="Economia_APP English (US) - Investment Analysis" width="900">
</p>

<p align="center">
  <strong>Depreciation (`en-US`) — Straight-Line Method, Estimated Residual Value (ERV), Useful Life ($N$) &amp; specific year $k$ analysis</strong><br><br>
  <img src="images/interfaces/en_US/Captura%20de%20tela%202025-10-21%20155215.png" alt="Economia_APP English (US) - Depreciation" width="900">
</p>
