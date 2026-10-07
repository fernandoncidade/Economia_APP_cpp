<!-- Multilanguage README.md for Economia_APP -->

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">Português (BR)</a> |
  <a href="#enus">English (US)</a>
</p>

---

## <a id="ptbr"></a>Português (BR)

> **Observação:** Este repositório refere-se à versão oficial **v2026.9.30.0** do projeto **Economia_APP**. Apoie o projeto e adquira a versão para Windows por meio do link oficial: [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9PLR0KD6KSJJ)

<details>
<summary>Clique para expandir o README em português</summary>

# Economia_APP — Calculadora Financeira e de Engenharia Econômica

Versão: v2026.9.30.0<br>
Data técnica desta revisão: 30 de setembro de 2026<br>
Autor: Fernando Nillsson Cidade<br>

## Resumo

O **Economia_APP** é uma avançada e abrangente aplicação desktop desenvolvida para análise financeira, engenharia econômica, gestão de investimentos e planejamento de amortizações de empréstimos. Projetado para estudantes, engenheiros, gestores financeiros e analistas de investimentos, o software automatiza cálculos complexos, exibe deduções passo a passo detalhadas, gera cronogramas completos de amortização e emite relatórios profissionais em formato PDF.

Desenvolvido nativamente em **C++17** com **Qt 6.11.1 Widgets**, o projeto é o resultado de uma refatoração integral, profunda e fiel do aplicativo original concebido em Python (PySide6). A nova arquitetura em C++ preserva 100% da identidade, convenções, nomenclaturas de arquivos, algoritmos matemáticos e lógica de interface do usuário, proporcionando um salto substancial em desempenho, inicialização instantânea, estabilidade contínua e consumo otimizado de recursos computacionais.

Esta versão oficial **2026.9.30.0** consolida todo o ecossistema financeiro: introdução de um **Manual do Usuário Interativo Integrado** acessível diretamente pela interface (`Ctrl+Shift+M`) com busca em tempo real e sumário dinâmico bidirecional sincronizado; nova **Janela Sobre (`Ctrl+Shift+A`)** com botões nativos de maximizar e minimizar, mecanismo de busca textual em tempo real em todas as abas e retradução dinâmica; novo motor de **Renderização Matemática e Notações Didáticas** (`MathRenderer`) que apresenta frações, expoentes e raízes com elegância visual adaptativa aos temas Claro e Escuro do Windows 11; estabilização e precisão em todas as 13 modalidades de cálculo financeiro; gerenciamento de histórico de sessão com edição inline de notas; exportação gráfica vetorial para PDF; isolamento estrito de ambientes de compilação (**MinGW 64-bit** e **MSVC x64 Ninja**); e tradução dinâmica bilíngue instantânea em tempo de execução sem reiniciar o software.

## Funcionalidades principais

- **Manual do Usuário Interativo Integrado (`Ctrl+Shift+M`)**: janela independente e não modal disponível no menu `Opções > Manual` (logo abaixo de *Sobre*), estruturada em 17 capítulos detalhados cobrindo todos os módulos do programa. Conta com sistema de busca textual instantânea que destaca todas as ocorrências em amarelo e a ocorrência ativa em laranja, contador de resultados em tempo real (`X de Y`), navegação ágil por botões e atalhos (`Enter` / `Shift+Enter`), sumário lateral sincronizado bidirecionalmente com a rolagem do texto e retradução bilíngue imediata sem fechar o diálogo.
- **Janela Institucional Sobre com Busca Rápida e Controle de Janela (`Ctrl+Shift+A`)**: diálogo independente e não modal acessível pelo menu `Opções > Sobre` ou atalho global, contendo botões nativos de maximizar e minimizar para leitura confortável em tela cheia, mecanismo de busca em tempo real com contador de ocorrências (`X de Y`), destaque visual de correspondências, transição inteligente e circular entre as 6 abas de documentação (*Histórico*, *Detalhes*, *Licenças*, *Avisos*, *Política de Privacidade* e *Notas de Versão*) e retradução dinâmica em tempo de execução ao alternar o idioma do sistema.
- **Renderização Didática de Fórmulas e Notações Matemáticas (`MathRenderer`)**: motor nativo para apresentação visual clara de fórmulas matemáticas, passos de resolução, frações alinhadas, potências sobrescritas, termos indexados e expressões com radiciação. As raízes e frações adaptam-se dinamicamente às variações de tema Claro e Escuro do Windows 11 sem depender de imagens estáticas e sem truncar expressões.
- **Juros Simples, Compostos e Comparativo Completo**: cálculo flexível de Valor Presente ($P$), Valor Futuro ($F$), Taxa de Juros ($i$) e Número de Períodos ($n$). Inclui análise comparativa detalhada entre os regimes simples e compostos com identificação analítica exata do ponto de cruzamento e indiferença.
- **Layout de Respostas Dinâmico e Proporcional**: área de respostas que redimensiona dinamicamente para ocupar o espaço ideal da tela, dividindo proporcionalmente o layout entre múltiplos cálculos sucessivos sem sobreposição e preservando todo o conteúdo na tela durante trocas de idioma com retradução ao vivo.
- **Anuidades e Séries Uniformes de Pagamentos**: resolução completa de pagamentos constantes ($A$) em regime postecipado (fim de período) e antecipado (início de período), apurando fluxos de caixa, valores presentes e montantes futuros acumulados com demonstração analítica das etapas.
- **Séries com Gradiente Aritmético e Geométrico**: análise rigorosa de fluxos com acréscimos ou decréscimos constantes em moeda ($G$) e variações percentuais periódicas contínuas ($g$), cálculo de parcelas pontuais em períodos específicos e séries perpétuas uniformes e em gradiente.
- **Equivalência e Conversão de Taxas de Juros**: transformação e equivalência precisa de taxas nominais e efetivas entre períodos diários, mensais, trimestrais, semestrais e anuais sob regime composto: $(1 + i_{destino}) = (1 + i_{origem})^{p}$.
- **Cinco Sistemas Completos de Amortização**: geração integral de planilhas de pagamento periódicas contendo Período, Prestação ($P_t$), Juros ($J_t$), Amortização ($A_t$) e Saldo Devedor ($S_t$):
  - **SAC (Sistema de Amortização Constante)**: amortização fixa periódica e prestações decrescentes.
  - **Price (Sistema Francês)**: prestações periódicas uniformes calculadas pelo fator de anuidade.
  - **SAM (Sistema de Amortização Misto)**: composição média ponderada dos sistemas SAC e Price.
  - **Sistema Americano**: quitação periódica exclusiva de juros com amortização integral do principal no período final (*bullet*).
  - **Sistema Hamburguês**: liquidação periódica com cobrança antecipada de juros sobre o saldo devedor.
  - Suporte completo a prazos de carência com ou sem capitalização de juros e desconto imediato de pagamentos de entrada.
- **Análise de Viabilidade de Investimentos**: cálculo de Valor Presente Líquido (VPL / NPV), Valor Anual Uniforme Equivalente (VAUE), Taxa Interna de Retorno (TIR / IRR) por aproximações numéricas iterativas de Newton-Raphson, Payback Simples e Payback Descontado acumulado período a período.
- **Métodos Abrangentes de Depreciação de Bens**: acompanhamento da perda de valor contábil de ativos através de:
  - Método Linear (Quotas Constantes).
  - Soma dos Dígitos dos Anos (Método de Cole, em ordens crescente e decrescente).
  - Saldo Declinante (Depreciação acelerada por percentual fixo anual sobre o saldo contábil residual).
- **Taxas Efetivas, Nominais, Sistema Alemão e TIR Modificada**:
  - Conversão de taxas nominais para taxas efetivas capitalizadas periodicamente.
  - Cálculo de juros sob o Sistema Alemão de cobrança antecipada.
  - Taxa Interna de Retorno Modificada (TIRm) com parametrização independente de taxas de captação e taxas de reinvestimento.
- **Taxa Mínima de Atratividade (TMA)**: apuração do custo de oportunidade a partir da taxa livre de risco e prêmio pelo risco do projeto.
- **Efeito Fisher (Inflação e Juros Reais)**: cálculo analítico da taxa aparente ($i_a$), taxa real ($i_r$) e taxa de inflação ($j$) por meio da relação estrita: $(1 + i_a) = (1 + i_r)(1 + j)$.
- **Avaliação Pontual no Período K**: determinação direta e matemática do saldo devedor, juros acumulados, amortização e prestação em um período $k$ arbitrário nos sistemas SAC e Price sem a necessidade de processar a tabela integral.
- **VPL com Incidência Tributária e Financiamento**: estudo de viabilidade econômica considerando deduções fiscais de IRPJ e CSLL, benefício fiscal da depreciação contábil do ativo e dedução de juros quando associado a financiamento SAC.
- **Custo Anual Uniforme Equivalente (CAUE) e Vida Econômica**: confronto analítico de alternativas de investimentos com vidas úteis desiguais e identificação do ciclo de substituição ideal de equipamentos baseado na tabela de custos operacionais e valores residuais decrescentes.
- **Container de Histórico com Edição Inline**: painel dock retrátil inferior que armazena todos os cálculos executados na sessão, permitindo editar anotações livremente, excluir registros individuais, limpar o log ou exportar dados.
- **Exportação para PDF de Alta Resolução**: geração de documentos vetoriais em PDF com formatação estruturada, bordas finas, alinhamento de cabeçalhos e inclusão das tabelas analíticas de amortização e CAUE.
- **Configuração de Fontes e Escala Tipográfica**: diálogo dedicado para personalização da família tipográfica (monospace ou proporcional) e tamanho dos textos em pontos com pré-visualização em tempo real e persistência via `QSettings`.
- **Internacionalização Dinâmica (i18n)**: suporte bilíngue nativo e completo em **Português do Brasil (`pt_BR`)** e **Inglês dos EUA (`en_US`)**, com alternância em tempo de execução sem reiniciar o programa.
- **Privacidade Total e Operação 100% Offline**: o software não realiza coletas de dados, telemetria ou conexões com a nuvem; todas as informações e cálculos permanecem exclusivamente no computador local do usuário.

## Módulos disponíveis

| Módulo | Arquivos de Implementação | Finalidade Principal |
| --- | --- | --- |
| **Juros Simples e Compostos** | `sv_01_calculate_interest`, `ui_03_create_interest_tab` | Cálculos de juros simples, compostos e comparativo com ponto de cruzamento. |
| **Anuidades / Séries Uniformes** | `sv_02_calculate_annuity`, `ui_04_create_annuity_tab` | Séries uniformes de pagamentos em regimes postecipado e antecipado. |
| **Séries em Gradiente** | `sv_03_calculate_gradient`, `ui_05_create_gradient_tab` | Gradientes aritméticos, geométricos, termos pontuais e séries perpétuas. |
| **Conversão de Taxas** | `sv_04_calculate_real_rate_equivalence`, `ui_06_create_rates_tab` | Conversão e equivalência de taxas periódicas nominais e efetivas. |
| **Amortização de Empréstimos** | `sv_05_calculate_amortization`, `ui_07`, `ui_15`-`ui_19` | Cronogramas completos para SAC, Price, SAM, Americano e Hamburguês. |
| **Análise de Investimentos** | `sv_06_calculate_investment`, `ui_08_create_investment_tab` | VPL, VAUE, TIR numérica por Newton-Raphson e Payback simples/descontado. |
| **Depreciação de Bens** | `sv_07_calculate_depreciation`, `ui_09_create_depreciation_tab` | Métodos Linear, Soma dos Dígitos dos Anos (Cole) e Saldo Declinante. |
| **Taxa Efetiva / TIR / Global** | `sv_08_calculate_effective_rate`, `ui_10_create_effective_rate_tab` | Taxas efetivas, Sistema Alemão e TIR Modificada (TIRm) com captação/reinvestimento. |
| **Retorno Mínimo (TMA)** | `sv_09_calculate_minimum_return`, `ui_11_create_minimum_return_tab` | Determinação da taxa mínima de atratividade e prêmio de risco do projeto. |
| **Equação de Fisher** | `sv_10_calculate_fisher`, `ui_12_create_fisher_tab` | Avaliação da relação estrita entre taxa nominal, taxa real e taxa de inflação. |
| **Valor no Período K** | `sv_11_calculate_value_at_k` | Avaliação pontual de saldo devedor, juros e parcelas no período $k$. |
| **VPL com Tributos** | `sv_12_calculate_vpl_with_taxes`, `ui_13_create_vpl_tax_tab` | Viabilidade de projetos com impostos (IRPJ/CSLL), depreciação e financiamento SAC. |
| **CAUE - Vida Econômica** | `sv_13_calculate_caue`, `ui_14_create_caue_tab`, `ui_20` | Custo Anual Uniforme Equivalente e substituição ótima de equipamentos. |
| **Janela Sobre e Licenciamento** | `ui_27_SobreDialog`, `ui_28_exibir_sobre`, `ui_29_opcoes_sobre` | Informações institucionais, histórico do projeto, licenças, busca textual e notas de versão. |
| **Manual do Usuário Interativo** | `ui_30_Manual` a `ui_33_exibir_manual` | Documentação integrada com 17 capítulos, busca ao vivo e sumário sincronizado. |
| **Histórico e Exportação** | `ui_23_history_container`, `ui_25_export_pdf` | Painel dock inferior para edição inline, gestão de notas e emissão em PDF. |
| **Tipografia e Notações** | `FontManager`, `MathRenderer`, `ui_24_font_config_dialog` | Configuração de fontes e renderização didática de equações matemáticas. |
| **Internacionalização (i18n)** | `tr_01_gerenciadorTraducao`, `tr_02_compileTranslations` | Gestão de catálogos `.ts`/`.qm` e alternância imediata entre `pt_BR` e `en_US`. |

## Destaques técnicos da versão 2026.9.30.0

- **Novo Manual do Usuário Interativo Integrado (`ui_30` a `ui_33`)**:
  - Arquitetura inspirada no projeto *EisenPo*, desenvolvida de forma desacoplada em C++ e Qt6;
  - Acesso direto pelo menu `Opções > Manual` e pelo atalho global de teclado `Ctrl+Shift+M`;
  - Janela não modal com leitor rico (`QTextBrowser`), sumário sincronizado (`QListWidget`), pesquisa de texto em tempo real com realce de todas as ocorrências (`QTextEdit::ExtraSelection`) e navegação por botões/teclas;
  - Conexão reativa ao sinal `idioma_alterado` do `GerenciadorTraducao`, re-traduzindo todo o manual em tempo de execução sem fechar a janela;
  - Catálogo completo de 17 seções didáticas em Português (`ui_31_manual_pt_BR`) e Inglês (`ui_32_manual_en_US`).
- **Nova Janela Sobre com Busca Textual e Controles de Janela (`ui_27_SobreDialog`, `ui_28_exibir_sobre`)**:
  - Incorporação dos botões nativos de maximizar e minimizar para leitura de alta legibilidade em tela cheia;
  - Campo de busca em tempo real com indicador numérico (`X de Y`), destaque visual de ocorrências e navegação com teclas `Enter` e `Shift+Enter`;
  - Navegação circular inteligente que percorre sequencialmente todas as 6 abas de documentação técnica e legal;
  - Atalho global de teclado `Ctrl+Shift+A` para invocação imediata e restauração de janela com foco automático;
  - Suporte completo a retradução dinâmica (`retranslate_ui`) conectada a `GerenciadorTraducao::idioma_alterado` e `QEvent::LanguageChange`, sem fechamento ou recriação do diálogo.
- **Renderizador Matemático Dedicado (`MathRenderer.hpp / .cpp`)**:
  - Algoritmo para formatação visual elegante de radiciação, frações e expressões com sobrescritos e subscritos;
  - Adaptação cromática automática aos temas Claro e Escuro do Windows 11 sem gerar imagens rasterizadas;
  - Tratamento aprimorado de caracteres subscritos compostos (como $i_{eq}$, $i_{periodo}$, $i_{anual}$) em todas as abas.
- **Estabilização dos Módulos de Cálculo e Interface**:
  - Correção dos motores matemáticos de Anuidades e Gradientes;
  - Suporte completo a todos os métodos de depreciação de bens (Linear, Cole crescente/decrescente e Saldo Declinante);
  - Refinamento do cálculo de TIR Modificada (TIRm) com equações bem desenhadas;
  - Eliminação da sobreposição visual de tabelas na aba CAUE;
  - Preservação e tradução em tempo real de resultados anteriores no histórico ao trocar de idioma.
- **Isolamento Estrito dos Perfis de Compilação (MinGW e MSVC)**:
  - 4 perfis independentes em `CMakePresets.json` eliminando conflitos de runtime;
  - Automação de deploy com `windeployqt` e cópia preventiva de dependências para execução livre de falhas de DLLs.

## Requisitos do sistema

- **Sistema operacional**: Windows 10 ou Windows 11 (arquitetura 64-bit).
- **CMake**: Versão 3.20 ou superior (recomendado 3.24+).
- **Compilador C++**: C++17 compatível (MinGW-w64 GCC 13.1.0+ ou MSVC v143+ / Visual Studio 2022/18).
- **Qt 6**: Qt 6.5 ou superior (testado e homologado sob **Qt 6.11.1**).
  - Módulos necessários: `Core`, `Gui`, `Widgets`, `PrintSupport`.
- **Build Tool**: Ninja 1.12 ou superior.

## Compilação rápida

O fluxo de compilação oficial utiliza o arquivo `CMakePresets.json` configurado na raiz do repositório, garantindo ambientes isolados de build.

### 1. Aplicação Principal com MinGW (GCC 13.1)

```powershell
cmake --preset host-qt6-mingw-release
cmake --build --preset host-qt6-mingw-release --target Economia_APPDeploy --parallel
```

### 2. Aplicação Principal com MSVC Ninja

```powershell
cmake --preset host-qt6-msvc-release
cmake --build --preset host-qt6-msvc-release --target Economia_APPDeploy --parallel
```

### 3. Utilitários de Mock e Tradução com MinGW

```powershell
cmake --preset mocks-mingw-release
cmake --build --preset mocks-mingw-release --target MocksRun --parallel
```

### 4. Utilitários de Mock e Tradução com MSVC

```powershell
cmake --preset mocks-msvc-release
cmake --build --preset mocks-msvc-release --target MocksRun --parallel
```

## Diretórios de compilação

A arquitetura do projeto mantém 4 pastas de saída rigorosamente isoladas em `build/`:

| *Preset* | Diretório de Saída | Alvos Principais | Finalidade |
| --- | --- | --- | --- |
| `host-qt6-mingw-release` | `build/build_Economia_APP_mingw` | `Economia_APP.exe` | Produto final com runtime MinGW |
| `host-qt6-msvc-release` | `build/build_Economia_APP_msvc_ninja` | `Economia_APP.exe` | Produto final com runtime MSVC |
| `mocks-mingw-release` | `build/build_mocks_mingw` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe` | Utilitários de versão e tradução MinGW |
| `mocks-msvc-release` | `build/build_mocks_msvc_ninja` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe` | Utilitários de versão e tradução MSVC |

## Prevenção e Imunização contra Falhas de DLL

Todos os executáveis gerados integram rotinas de pós-compilação com `windeployqt.exe` e sincronização automática de runtimes críticos:
- **Bibliotecas Qt**: `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6PrintSupport.dll`.
- **Runtimes MinGW**: `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` copiados preventivamente.
- **Plugins de Plataforma**: `platforms/qwindows.dll` sincronizado para a pasta do binário.
- **Recursos**: pastas `assets` e `translations` copiadas automaticamente para a raiz de execução.

## Persistência local

O aplicativo armazena preferências do usuário e controle de licença localmente em:
`%LOCALAPPDATA%/Economia_APP`

Arquivos gerenciados:
- `trial.dat`: registro persistente do período de avaliação com criptografia leve.
- Configurações de fontes tipográficas e preferências visuais persistidas via registro `QSettings`.

## Exportação e Relatórios

- **Exportar Atual (`Ctrl+P`)**: gera um documento PDF estilizado contendo exclusivamente a memória de cálculo e eventuais tabelas analíticas da aba que estiver aberta no momento.
- **Exportar Todos (`Ctrl+Shift+P`)**: compila em um único relatório consolidado em PDF todas as seções calculadas na sessão acompanhadas do cronograma de amortização.
- **Tabelas de Amortização**: exportação vetorial de tabelas completas com colunas de Período, Prestação, Juros, Amortização e Saldo Devedor.

## Solução de problemas

- **O programa reporta mensagem de componente ou DLL ausente**:
  - Execute a compilação utilizando o alvo de deploy correspondente (`Economia_APPDeploy`), que invoca o `windeployqt` e copia todos os runtimes necessários para a pasta do executável.
- **A janela do Manual ou a janela Sobre não abre**:
  - Verifique se os atalhos `Ctrl+Shift+M` (Manual) ou `Ctrl+Shift+A` (Sobre) foram acionados, ou abra através do menu `Opções`. Caso as janelas já estejam abertas ou minimizadas, elas serão restauradas para o tamanho normal e trazidas ao primeiro plano automaticamente.
- **Os textos aparecem em português após selecionar inglês**:
  - O aplicativo utiliza catálogos compilados `.qm` em `source/language/translations/`. Caso adicione novos termos nos arquivos `.ts`, compile-os através do utilitário `CompileTranslationsGUI` ou execute o alvo CMake para recompilar os arquivos `.qm`.
- **As tabelas de amortização não geram**:
  - Certifique-se de que os valores numéricos informados são maiores que zero e que eventuais prazos de carência sejam menores que o número total de períodos ($n$).

## Atalhos úteis de teclado

| Atalho | Ação Executada |
| --- | --- |
| `Ctrl+Shift+M` | Abre a janela do **Manual do Usuário Interativo**. |
| `Ctrl+Shift+A` | Abre a janela institucional **Sobre** e termos legais (com busca integrada). |
| `Ctrl+P` | Exporta a aba de cálculo ativa para **PDF**. |
| `Ctrl+Shift+P` | Exporta **todos os cálculos** da sessão para um relatório PDF completo. |
| `Enter` | Na busca do Manual ou Sobre: avança para a **próxima ocorrência**. |
| `Shift+Enter` | Na busca do Manual ou Sobre: retorna para a **ocorrência anterior**. |

## Licenças, avisos e privacidade

- As licenças completas de terceiros e os termos legais estão disponíveis diretamente na aplicação em `Opções > Sobre`.
- Todo o processamento matemático e os registros de histórico são executados exclusivamente em ambiente local offline, garantindo sigilo absoluto e privacidade para seus dados e análises financeiras.
- Manual completo do usuário: [MANUAL.md — Documentação Operacional](./MANUAL.md#ptbr)

## Autor

Fernando Nillsson Cidade<br>
Contato: `linceu_lighthouse@outlook.com`

---

</details>

## <a id="enus"></a>English (US)

> **Note:** This repository refers to the official **v2026.9.30.0** release of the **Economia_APP** project. Support the project and get the Windows version through the official link: [Install via Microsoft Store](https://apps.microsoft.com/detail/9PLR0KD6KSJJ)

<details>
<summary>Click to expand the README in English</summary>

# Economia_APP — Financial and Engineering Economics Calculator

Version: v2026.9.30.0<br>
Technical revision date: September 30, 2026<br>
Author: Fernando Nillsson Cidade<br>

## Summary

**Economia_APP** is an advanced desktop suite engineered for financial analysis, engineering economy, investment appraisal, and loan amortization planning. Tailored for students, engineers, corporate financial officers, and investment analysts, the software automates complex engineering economy routines, outputs explicit step-by-step mathematical derivations, produces complete loan amortization schedules, and generates clean, print-ready PDF reports.

Built natively in **C++17** using **Qt 6.11.1 Widgets**, this project represents a comprehensive, faithful, and integral rewrite of the original Python (PySide6) application. The C++ architecture preserves 100% of the original product's directory hierarchy, naming conventions, mathematical formulas, and GUI design, while achieving native execution speeds, instantaneous startup times, rock-solid stability, and minimal hardware overhead.

This official release **2026.9.30.0** delivers a complete financial ecosystem: an **Integrated Interactive User Manual** accessible directly from the interface (`Ctrl+Shift+M`) with real-time text search and two-way synchronized table of contents; a new comprehensive **About Dialog (`Ctrl+Shift+A`)** featuring native maximize and minimize window controls, real-time search with match counter across all documentation tabs, and live dynamic retranslation; a specialized **Mathematical and Didactic Notation Renderer** (`MathRenderer`) presenting fractions, powers, and roots adapted to Windows 11 Light and Dark themes; stabilized numerical engines across all 13 financial calculation domains; an interactive session history dock with inline note editing; vector PDF report export; isolated build configurations (**MinGW 64-bit** and **MSVC x64 Ninja**); and seamless live bilingual localization without application restart.

## Key features

- **Integrated Interactive User Manual (`Ctrl+Shift+M`)**: independent, non-modal window accessible via `Options > Manual` (positioned directly beneath *About*), structured into 17 chapters detailing every aspect of the software. Features instant real-time text search with yellow matching and active orange highlighting, live occurrence counters (`X of Y`), rapid keyboard traversal (`Enter` / `Shift+Enter`), a two-way synchronized table of contents sidebar, and instant bilingual retranslation without closing the dialog.
- **Institutional About Dialog with Fast Search and Window Controls (`Ctrl+Shift+A`)**: independent, non-modal dialog accessible through `Options > About` or global shortcut, featuring native maximize and minimize window controls for comfortable full-screen reading, real-time search with match counter (`X of Y`), visual highlighting, smart circular traversal across all 6 documentation tabs (*History*, *Details*, *Licenses*, *Notices*, *Privacy Policy*, and *Release Notes*), and dynamic runtime retranslation upon system language change.
- **Didactic Formula Display & Mathematical Typography (`MathRenderer`)**: native engine for rendering equations, step-by-step derivations, aligned fractions, superscript exponents, subscript indices, and roots. Root and fraction formatting dynamically adapts to Windows 11 Light and Dark themes without converting text into static raster images.
- **Simple, Compound, and Comparative Interest**: solves Present Value ($P$), Future Value ($F$), Rate ($i$), and Periods ($n$), complete with a rigorous comparative analysis identifying the crossover indifference point between linear and exponential compounding.
- **Dynamic and Proportional Layout**: calculation response cards dynamically expand to fill available window dimensions, dividing layout space proportionally between subsequent calculations without overlapping, and preserving all on-screen results during language switching with live retranslation.
- **Uniform Annuities and Cash Flow Series**: ordinary (postpaid) and due (prepaid) annuity payment computations, determining cash flows, present values, and future values with step-by-step mathematical demonstrations.
- **Arithmetic and Geometric Gradient Series**: comprehensive modeling of cash flows with constant monetary increments/decrements ($G$) or continuous percentage rate changes ($g$), single-period terms, and uniform/gradient perpetual cash flows.
- **Interest Rate Equivalence and Conversions**: exact compounding transformations between daily, monthly, quarterly, semi-annual, and annual rates: $(1 + i_{target}) = (1 + i_{source})^{p}$.
- **Five Complete Loan Amortization Systems**: generates detailed periodic repayment tables listing Period, Payment ($P_t$), Interest ($J_t$), Amortization ($A_t$), and Outstanding Balance ($S_t$):
  - **SAC (Constant Amortization System)**: constant periodic principal reduction and declining payments.
  - **Price (French Annuity System)**: uniform payments calculated via annuity factors.
  - **SAM (Mixed Amortization System)**: arithmetic mean of SAC and Price schedules.
  - **American System**: periodic interest-only payments with a lump-sum principal payoff at maturity (*bullet*).
  - **Hamburg System**: periodic amortization with interest paid in advance.
  - Full support for grace periods (with or without interest capitalization) and immediate down-payment deductions.
- **Capital Investment Appraisal**: Net Present Value (NPV), Equivalent Uniform Annual Value (EUAV), Internal Rate of Return (IRR) via iterative Newton-Raphson approximation, Simple Payback, and cumulative Discounted Payback.
- **Comprehensive Asset Depreciation Methods**: tracks asset book value over time through:
  - Straight-Line Method (Constant Quotas).
  - Sum-of-the-Years'-Digits (Cole Method, in accelerated and decelerated orders).
  - Declining Balance Method (Accelerated depreciation via fixed annual percentage).
- **Effective vs. Nominal Rates, German System, and Modified IRR**:
  - Periodic compounding translations from nominal rates to effective rates.
  - German upfront interest calculation.
  - Modified Internal Rate of Return (MIRR) with independent reinvestment and financing rate parameters.
- **Minimum Attractive Rate of Return (MARR)**: benchmark determination incorporating risk-free rates and project risk premiums.
- **Fisher Effect (Inflation and Real Interest Rates)**: exact valuation of apparent (nominal) rates ($i_a$), real rates ($i_r$), and inflation rates ($j$) via: $(1 + i_a) = (1 + i_r)(1 + j)$.
- **Period K Point Valuation**: instant closed-form formula computation of outstanding balance, cumulative interest, amortization, and installments at period $k$ for SAC and Price systems without rendering the entire schedule.
- **NPV with Taxation and Debt Financing**: feasibility analysis accounting for corporate income tax rates, accounting depreciation tax shields, and tax-deductible interest on debt financing.
- **Equivalent Annual Cost (EAC / CAUE) and Economic Life**: evaluates mutually exclusive alternatives with unequal lifespans, determining optimal asset replacement intervals by balancing rising maintenance costs against falling salvage values.
- **History Container with Inline Note Editing**: collapsible bottom dock widget that captures every calculation completed in the session, allowing users to edit notes inline, delete selected records, clear history, or export results.
- **High-Resolution Vector PDF Export**: native PDF generation with formal headers, crisp table borders, and formatted amortization and CAUE schedules.
- **Font & Scale Customization**: dedicated dialog for customizing typeface families (monospace vs. proportional) and font point sizes with real-time preview and `QSettings` persistence.
- **Dynamic Localization (i18n)**: complete bilingual support in **Brazilian Portuguese (`pt_BR`)** and **US English (`en_US`)**, switching immediately across all screens without application restarts.
- **Total Privacy and Offline Execution**: zero telemetry, tracking, or cloud connectivity; all financial calculations remain strictly on the user's local computer.

## Available modules

| Module | Source Files | Primary Purpose |
| --- | --- | --- |
| **Simple and Compound Interest** | `sv_01_calculate_interest`, `ui_03_create_interest_tab` | Simple, compound, and comparative interest analysis with indifference crossover point. |
| **Uniform Annuity Series** | `sv_02_calculate_annuity`, `ui_04_create_annuity_tab` | Ordinary (postpaid) and due (prepaid) annuity payment valuations. |
| **Gradient Series** | `sv_03_calculate_gradient`, `ui_05_create_gradient_tab` | Arithmetic and geometric cash flow gradients, perpetual series, and single-period values. |
| **Rate Conversions** | `sv_04_calculate_real_rate_equivalence`, `ui_06_create_rates_tab` | Compounding transformations between nominal and effective periodic rates. |
| **Loan Amortization** | `sv_05_calculate_amortization`, `ui_07`, `ui_15`-`ui_19` | Full schedules for SAC, Price, SAM, American, and Hamburg systems. |
| **Investment Appraisal** | `sv_06_calculate_investment`, `ui_08_create_investment_tab` | NPV, EUAV, Newton-Raphson IRR, and simple/discounted payback metrics. |
| **Asset Depreciation** | `sv_07_calculate_depreciation`, `ui_09_create_depreciation_tab` | Straight-Line, Sum-of-the-Years'-Digits (Cole), and Declining Balance schedules. |
| **Effective Rate / IRR / Global** | `sv_08_calculate_effective_rate`, `ui_10_create_effective_rate_tab` | Effective rates, German system, and Modified IRR (MIRR). |
| **Minimum Return (MARR)** | `sv_09_calculate_minimum_return`, `ui_11_create_minimum_return_tab` | Minimum attractive rate of return and project risk premium assessment. |
| **Fisher Equation** | `sv_10_calculate_fisher`, `ui_12_create_fisher_tab` | Strict inflation adjustment relating nominal, real, and inflation rates. |
| **Value at Period K** | `sv_11_calculate_value_at_k` | Closed-form balance, interest, and payment valuation at period $k$. |
| **NPV with Taxes** | `sv_12_calculate_vpl_with_taxes`, `ui_13_create_vpl_tax_tab` | Feasibility analysis incorporating corporate taxes, depreciation, and SAC financing. |
| **EAC - Economic Life** | `sv_13_calculate_caue`, `ui_14_create_caue_tab`, `ui_20` | Equivalent Annual Cost and optimal equipment replacement life. |
| **About Dialog & Licensing** | `ui_27_SobreDialog`, `ui_28_exibir_sobre`, `ui_29_opcoes_sobre` | Institutional metadata, project history, third-party licenses, text search, and release notes. |
| **Integrated User Manual** | `ui_30_Manual` to `ui_33_exibir_manual` | Built-in 17-chapter user guide with real-time search and synchronized TOC. |
| **History & PDF Export** | `ui_23_history_container`, `ui_25_export_pdf` | Lower dock widget for inline notes and high-resolution PDF reporting. |
| **Typography & Notation** | `FontManager`, `MathRenderer`, `ui_24_font_config_dialog` | Typography settings and educational mathematical formula rendering. |
| **Internationalization (i18n)** | `tr_01_gerenciadorTraducao`, `tr_02_compileTranslations` | Catalog management and runtime switching between `pt_BR` and `en_US`. |

## Technical highlights in version 2026.9.30.0

- **New Integrated Interactive User Manual (`ui_30` to `ui_33`)**:
  - Architecture inspired by *EisenPo*, implemented in clean and decoupled C++ and Qt6;
  - Instant access from `Options > Manual` and via the global keyboard shortcut `Ctrl+Shift+M`;
  - Non-modal dialog with a rich reader (`QTextBrowser`), synchronized table of contents (`QListWidget`), instant search highlighting (`QTextEdit::ExtraSelection`), and match traversal;
  - Reactive connection to `GerenciadorTraducao::idioma_alterado`, dynamically retranslating the open manual window at runtime without closing it;
  - 17 comprehensive chapters in Portuguese (`ui_31_manual_pt_BR`) and English (`ui_32_manual_en_US`).
- **Enhanced About Dialog with Integrated Search and Window Controls (`ui_27_SobreDialog`, `ui_28_exibir_sobre`)**:
  - Addition of native maximize and minimize window buttons for convenient full-screen reading of licensing terms and notes;
  - Real-time search bar with occurrence counter (`X of Y`), color highlighting (`QTextEdit::ExtraSelection`), and keyboard navigation via `Enter` / `Shift+Enter`;
  - Smart circular traversal sequentially moving through all 6 documentation tabs;
  - Global keyboard shortcut `Ctrl+Shift+A` for instant invocation and focus restoration;
  - Full integration with `GerenciadorTraducao::idioma_alterado` and `QEvent::LanguageChange` for live dynamic translation without closing or recreating the dialog.
- **Dedicated Mathematical Notation Renderer (`MathRenderer.hpp / .cpp`)**:
  - Custom algorithm for aesthetic, clear rendering of roots, fractions, exponents, and subscripts;
  - Dynamic adaptation to Windows 11 Light and Dark themes without generating static raster images;
  - Proper subscript handling for compound variables (such as $i_{eq}$, $i_{period}$, $i_{annual}$) across all tabs.
- **Engine Stabilization and UI Enhancements**:
  - Mathematical bug fixes in Annuity and Gradient calculation engines;
  - Full support for all asset depreciation methods (Straight-Line, Cole accelerated/decelerated, Declining Balance);
  - Refined Modified IRR (MIRR) formula presentation;
  - Elimination of overlapping table artifacts in the EAC / CAUE tab;
  - Preservation and real-time translation of previous calculation results in the history dock.
- **Isolated Build Presets (MinGW and MSVC)**:
  - 4 isolated profiles in `CMakePresets.json` preventing runtime library clashes;
  - Automated deployment with `windeployqt` and preventative runtime copying for zero missing DLL errors.

## System requirements

- **Operating System**: Windows 10 or Windows 11 (64-bit architecture).
- **CMake**: Version 3.20 or later (3.24+ recommended).
- **C++ Compiler**: Compatible C++17 compiler (MinGW-w64 GCC 13.1.0+ or MSVC v143+ / Visual Studio 2022/18).
- **Qt 6**: Qt 6.5 or later (tested and validated under **Qt 6.11.1**).
  - Required modules: `Core`, `Gui`, `Widgets`, `PrintSupport`.
- **Build Tool**: Ninja 1.12 or later.

## Quick build

The official build flow relies on `CMakePresets.json` configured at the root of the workspace.

### 1. Main Application with MinGW (GCC 13.1)

```powershell
cmake --preset host-qt6-mingw-release
cmake --build --preset host-qt6-mingw-release --target Economia_APPDeploy --parallel
```

### 2. Main Application with MSVC Ninja

```powershell
cmake --preset host-qt6-msvc-release
cmake --build --preset host-qt6-msvc-release --target Economia_APPDeploy --parallel
```

### 3. Development Mocks and Translation Utilities with MinGW

```powershell
cmake --preset mocks-mingw-release
cmake --build --preset mocks-mingw-release --target MocksRun --parallel
```

### 4. Development Mocks and Translation Utilities with MSVC

```powershell
cmake --preset mocks-msvc-release
cmake --build --preset mocks-msvc-release --target MocksRun --parallel
```

## Build directories

The project architecture isolates 4 dedicated output folders inside `build/`:

| Preset | Output Directory | Main Targets | Purpose |
| --- | --- | --- | --- |
| `host-qt6-mingw-release` | `build/build_Economia_APP_mingw` | `Economia_APP.exe` | Final product with MinGW runtime |
| `host-qt6-msvc-release` | `build/build_Economia_APP_msvc_ninja` | `Economia_APP.exe` | Final product with MSVC runtime |
| `mocks-mingw-release` | `build/build_mocks_mingw` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe` | Version and translation tools (MinGW) |
| `mocks-msvc-release` | `build/build_mocks_msvc_ninja` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe` | Version and translation tools (MSVC) |

## Prevention of Missing DLL Errors

All build targets incorporate post-build automation using `windeployqt.exe` and preventative runtime deployment:
- **Qt Dynamic Libraries**: `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6PrintSupport.dll`.
- **MinGW Runtimes**: `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` deployed to output directory.
- **QPA Platform Plugins**: `platforms/qwindows.dll` copied adjacent to binaries.
- **Assets**: `assets` and `translations` folders synchronized at compile time.

## Local persistence

The application stores user settings and local trial license data at:
`%LOCALAPPDATA%/Economia_APP`

Managed files:
- `trial.dat`: persistent trial evaluation record with lightweight encryption.
- Typography and window preferences persisted via `QSettings`.

## Export and Reporting

- **Export Current (`Ctrl+P`)**: outputs a print-ready PDF containing the calculation steps and analytical tables of the currently active tab.
- **Export All (`Ctrl+Shift+P`)**: compiles all calculations completed across all tabs and amortization schedules into a single comprehensive PDF report.
- **Amortization Tables**: vector export of structured schedules with columns for Period, Payment, Interest, Principal Reduction, and Outstanding Balance.

## Troubleshooting

- **Application reports missing DLL or component**:
  - Build the corresponding deployment target (`Economia_APPDeploy`), which triggers `windeployqt` and copies all required runtimes to the binary directory.
- **User Manual or About window does not open**:
  - Press `Ctrl+Shift+M` (Manual) or `Ctrl+Shift+A` (About), or open via the `Options` menu. If the windows are minimized, they will automatically restore to normal size and come to the foreground.
- **Text remains in Portuguese after selecting English**:
  - The software uses compiled `.qm` catalogs located in `source/language/translations/`. If you modify `.ts` files, compile them via `CompileTranslationsGUI` or run the CMake translation target.
- **Amortization schedule does not generate**:
  - Verify that input amounts are positive and that grace periods are strictly less than the total number of periods ($n$).

## Useful keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Ctrl+Shift+M` | Opens the **Interactive User Manual** window. |
| `Ctrl+Shift+A` | Opens the institutional **About** window and legal terms (with built-in search). |
| `Ctrl+P` | Exports the active calculation tab to **PDF**. |
| `Ctrl+Shift+P` | Exports **all session calculations** into a consolidated PDF report. |
| `Enter` | In Manual and About search: jumps to the **next occurrence**. |
| `Shift+Enter` | In Manual and About search: jumps to the **previous occurrence**. |

## Licenses, notices, and privacy

- Complete third-party licenses and legal notices are accessible within the application at `Options > About`.
- All financial calculations, mathematical operations, and session history remain strictly on your local PC with zero telemetry.
- User Manual: [MANUAL.md — Operational Manual](./MANUAL.md#enus)

## Author

Fernando Nillsson Cidade<br>
Contact: `linceu_lighthouse@outlook.com`

---

</details>
