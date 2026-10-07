# MANUAL - Economia_APP

Versão documentada: `2026.9.30.0`
Data: `30 de setembro de 2026`

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">Português (BR)</a> |
  <a href="#enus">English (US)</a>
</p>

---

## <a id="ptbr"></a>Português (Brasil)

Este manual descreve a operação, formulações matemáticas, arquitetura e fluxos de compilação do **Economia_APP** (Calculadora Financeira e de Engenharia Econômica), desenvolvido em **C++17** e **Qt 6.11.1**.

---

## 1. Visão Geral do Aplicativo

O **Economia_APP** é uma ferramenta desktop de alta precisão projetada para estudantes, engenheiros, gestores financeiros e analistas de investimentos. O software automatiza rotinas financeiras, exibe passos de cálculo passo a passo, gera tabelas analíticas detalhadas de amortização, analisa investimentos e permite a exportação de relatórios em PDF.

### 1.1. Principais Recursos
- **Manual do Usuário Interativo Integrado**: Sistema completo de documentação acessível pelo menu **Opções → Manual** ou pelo atalho **`Ctrl+Shift+M`**, com busca em tempo real, sumário bidirecional sincronizado e alternância bilíngue dinâmica.
- **Janela Sobre Completa com Busca Rápida**: Acessível pelo menu **Opções → Sobre** ou pelo atalho **`Ctrl+Shift+A`**, com botões de maximizar e minimizar, mecanismo de busca em tempo real com contador em todas as abas (Histórico, Detalhes, Licenças, Avisos, Privacidade e Notas de Versão) e alternância bilíngue dinâmica.
- **Interface Gráfica por Abas Especializadas**: Navegação intuitiva entre todas as modalidades de cálculo financeiro e de engenharia econômica.
- **Detalhamento Matemático dos Passos**: Apresentação didática das etapas, fórmulas aplicadas e substituições numéricas em cada resolução.
- **Histórico Centralizado de Cálculos**: Painel retrátil inferior para armazenamento, visualização, edição inline, exclusão e exportação do histórico de sessões.
- **Exportação para PDF de Alta Qualidade**: Integração com geração vetorial e formatação tipográfica para emissão de tabelas e relatórios de amortização.
- **Personalização de Fontes e Escala**: Diálogo de configuração de tipografia e tamanhos de fonte em tempo real com pré-visualização.
- **Localização Dinâmica**: Alternância imediata entre Português do Brasil (`pt_BR`) e Inglês (`en_US`) sem reiniciar o software.
- **Gestão de Período de Avaliação (Trial)**: Sistema de licenciamento local baseado em registro persistente no AppData.

---

## 2. Como Utilizar o Novo Manual Interativo Integrado

A aplicação conta com um sistema de manual integrado que permite consultar todas as orientações operacionais diretamente na interface, sem necessidade de arquivos externos ou navegadores web.

### 2.1. Como Acessar o Manual
1. Pela barra de menus superior, acesse: **Opções → Manual** (localizado logo abaixo do submenu *Sobre*).
2. Pelo teclado, utilize a combinação de teclas de atalho: **`Ctrl+Shift+M`**.
3. O manual abre em uma janela independente e não modal, permitindo que você consulte as orientações e continue operando as abas de cálculo simultaneamente.
4. Se a janela do manual já estiver aberta, acionar a opção novamente ou utilizar o atalho restaura a janela caso minimizada e traz o foco para ela, evitando a abertura de janelas duplicadas.

### 2.2. Sistema de Busca em Tempo Real
A barra de pesquisa superior oferece localização instantânea de qualquer termo:
- **Localização ao Digitar**: Conforme os caracteres são digitados no campo de busca, todas as ocorrências no texto são localizadas e destacadas automaticamente em amarelo.
- **Destaque da Ocorrência Ativa**: A ocorrência em foco no momento recebe um destaque diferenciado em cor laranja, e a área de visualização rola automaticamente para exibi-la.
- **Contador de Ocorrências**: Um indicador visual no formato `X de Y` (ou `X of Y`) informa a posição atual e o total de resultados encontrados.
- **Navegação Rápida**:
  - Clique no botão **Próximo** ou pressione a tecla **`Enter`** para avançar para a próxima ocorrência.
  - Clique no botão **Anterior** ou pressione **`Shift+Enter`** para retornar à ocorrência precedente.
- **Limpeza da Busca**: Ao apagar o texto pesquisado ou clicar no botão de limpeza do campo, todos os destaques são removidos e a rolagem é preservada.

### 2.3. Sumário Dinâmico e Navegação Bidirecional
- **Sumário Lateral Esquerdo**: Apresenta a relação completa dos 17 capítulos do manual.
- **Navegação por Clique**: Ao clicar em qualquer item do sumário, o visualizador rola de forma precisa para o início da seção correspondente.
- **Sincronização Automática por Rolagem**: Ao rolar o documento no painel direito com a roda do mouse ou barra de rolagem, o item correspondente à seção visível é automaticamente selecionado no sumário esquerdo.
- **Links Internos e Âncoras**: O texto do manual possui âncoras navegáveis que permitem saltar entre tópicos correlatos com um clique.

### 2.4. Alternância Dinâmica de Idioma (Bilingual Live Switch)
- O manual oferece suporte completo em **Português do Brasil (`pt_BR`)** e **Inglês (`en_US`)**.
- Caso você altere o idioma do programa através do menu **Opções → Idioma / Language**, a janela do manual atualiza instantaneamente todo o sumário, títulos, conteúdos explicativos e rótulos da barra de busca em tempo real, sem que você precise fechar ou reabrir a janela.

### 2.5. Estrutura dos 17 Capítulos Integrados
O manual interno cobre em profundidade:
1. **Visão Geral e Apresentação do Produto**: Finalidade, escopo didático e arquitetura de operação.
2. **Barra de Menus e Configurações Globais**: Estrutura das abas, preferências do sistema e atalhos globais.
3. **Módulo 1: Juros Simples e Compostos**: Metodologias, ponto de cruzamento e cálculo de juros.
4. **Módulo 2: Anuidades e Séries Uniformes**: Regimes postecipado e antecipado, cálculo de parcelas e montantes.
5. **Módulo 3: Séries em Gradiente (Aritmético e Geométrico)**: Variações aritméticas ($G$) e geométricas ($g$), termos perpétuos e séries pontuais.
6. **Módulo 4: Conversão e Equivalência de Taxas**: Conversão entre períodos diário, mensal, trimestral, semestral e anual.
7. **Módulo 5: Sistemas de Amortização de Dívidas**: Detalhamento prático dos sistemas SAC, Price, SAM, Americano e Hamburguês, incluindo carência e entradas.
8. **Módulo 6: Análise de Viabilidade Econômica de Investimentos**: VPL, TIR por métodos numéricos iterativos, Payback Simples e Descontado.
9. **Módulo 7: Métodos de Depreciação de Ativos**: Cotas lineares, soma dos dígitos dos anos (Cole) e saldo declinante.
10. **Módulo 8: Taxa Efetiva, TIR Avançada e Custo de Capital**: Capitalização periódica, Sistema Alemão e TIRm com taxas de captação e reinvestimento.
11. **Módulo 9: Retorno Mínimo Exigido (TMA)**: Composição com taxa livre de risco e prêmio de risco do negócio.
12. **Módulo 10: Equação de Fisher e Inflação**: Avaliação de taxas aparentes, taxas reais e taxa de inflação.
13. **Módulo 11: VPL com Tributação e Benefício Fiscal**: Análise de projetos considerando IRPJ/CSLL, dedução de depreciação e juros de financiamento.
14. **Módulo 12: CAUE - Custo Anual Uniforme Equivalente e Vida Econômica**: Comparação entre ativos de vidas úteis desiguais e determinação da vida econômica ideal.
15. **Painel de Histórico (HistoryContainer) e Manipulação**: Utilização do dock inferior, edição inline de notas, remoção e exportação.
16. **Configuração de Fontes e Exportação para PDF**: Ajuste de fontes e geração de relatórios de impressão em PDF.
17. **Atalhos de Teclado, Boas Práticas e Solução de Problemas**: Recomendações operacionais, tabela de atalhos e solução de problemas.

### 2.6. Como Utilizar a Janela Sobre e Atalho `Ctrl+Shift+A`
A janela **Sobre** do Economia_APP oferece documentação institucional completa com facilidades ergonômicas avançadas:
- **Acesso Rápido**: Pode ser aberta a qualquer instante pelo menu **Opções → Sobre** ou pelo atalho global **`Ctrl+Shift+A`**.
- **Instância Única e Não Modal**: Permite consulta simultânea às abas de cálculo sem bloquear o restante do programa. Se já estiver aberta, utilizar o atalho ou menu restaura a janela caso minimizada e traz o foco para ela.
- **Botões de Maximizar e Minimizar**: Suporta redimensionamento livre, permitindo expandir a leitura em tela cheia ou recolher para a barra de tarefas.
- **Barra de Pesquisa "Buscar:" Integrada**:
  - Localiza qualquer termo nas 6 abas de documentação (*Histórico*, *Detalhes*, *Licenças*, *Avisos*, *Política de Privacidade* e *Notas de Versão*).
  - Destaca todas as ocorrências encontradas no visualizador de texto.
  - Exibe um contador de resultados no formato `X de Y`.
  - Navega sequencialmente entre ocorrências usando os botões de seta anterior/próximo, ou via tecla **`Enter`** (próximo) e **`Shift+Enter`** (anterior).
  - Transita de forma inteligente entre abas caso o termo pesquisado conste em outra seção.
- **Tradução Dinâmica em Tempo Real**: Altera instantaneamente todos os cabeçalhos, textos dos documentos, abas e controles de busca ao alternar o idioma do sistema em tempo de execução sem fechar a janela.

---

## 3. Módulos de Cálculo e Serviços

### 3.1. Juros Simples e Compostos (`sv_01_calculate_interest`, `ui_03`)
Calcula Valor Presente ($P$), Valor Futuro ($F$), Taxa de Juros ($i$) e Número de Períodos ($n$), além de comparativo com identificação do ponto de indiferença.
- **Juros Simples**:
  $$F = P \cdot (1 + i \cdot n)$$
  $$J = P \cdot i \cdot n$$
- **Juros Compostos**:
  $$F = P \cdot (1 + i)^n$$
  $$P = \frac{F}{(1 + i)^n}$$

### 3.2. Anuidades / Séries Uniformes (`sv_02_calculate_annuity`, `ui_04`)
Resolução de pagamentos constantes ($A$) em regime postecipado ou antecipado:
- **Postecipada (Fim do Período)**:
  $$P = A \cdot \left[ \frac{(1+i)^n - 1}{i \cdot (1+i)^n} \right]$$
  $$F = A \cdot \left[ \frac{(1+i)^n - 1}{i} \right]$$
- **Antecipada (Início do Período)**: Os valores de $P$ e $F$ são multiplicados pelo fator de capitalização $(1 + i)$.

### 3.3. Séries em Gradiente (`sv_03_calculate_gradient`, `ui_05`)
- **Gradiente Aritmético**: Série em que as parcelas sofrem acréscimo ou decréscimo constante $G$ a cada período:
  $$P = \frac{G}{i} \cdot \left[ \frac{(1+i)^n - 1}{i \cdot (1+i)^n} - \frac{n}{(1+i)^n} \right]$$
- **Gradiente Geométrico**: Série em que as parcelas sofrem variação percentual constante $g$ a cada período.

### 3.4. Equivalência de Taxas (`sv_04_calculate_real_rate_equivalence`, `ui_06`)
Conversão e equivalência de taxas de juros entre prazos diários, mensais, trimestrais, semestrais e anuais:
$$(1 + i_{destino}) = (1 + i_{origem})^{p}$$

### 3.5. Sistemas de Amortização (`sv_05_calculate_amortization`, `ui_07`, `ui_15`-`ui_19`)
Geração completa da tabela de amortização com colunas de Período, Prestação ($P_t$), Juros ($J_t$), Amortização ($A_t$) e Saldo Devedor ($S_t$):
- **SAC (Sistema de Amortização Constante)**: Amortização periódica constante $A_t = \frac{S_0}{n}$.
- **PRICE (Sistema Francês)**: Prestações constantes calculadas pelo fator de anuidade.
- **SAM (Sistema de Amortização Misto)**: Médias aritméticas das prestações e amortizações dos sistemas SAC e Price.
- **Sistema Americano**: Pagamento periódico exclusivo de juros, com amortização total do principal no último período.
- **Sistema Hamburguês**: Pagamento periódico de juros antecipados com amortização constante.
- Suporta carência com capitalização ou pagamento simples de juros, além de abatimento imediato de entrada.

### 3.6. Análise de Investimentos (`sv_06_calculate_investment`, `ui_08`)
- **Valor Presente Líquido (VPL / NPV)**:
  $$VPL = \sum_{t=1}^n \frac{FC_t}{(1 + TMA)^t} - I_0$$
- **Taxa Interna de Retorno (TIR / IRR)**: Taxa de desconto para a qual $VPL = 0$, calculada por aproximação numérica iterativa.
- **Payback Simples e Descontado**: Ponto de recuperação do investimento inicial com consideração do custo de capital.

### 3.7. Métodos de Depreciação (`sv_07_calculate_depreciation`, `ui_09`)
- **Método Linear (Quotas Constantes)**:
  $$D_t = \frac{C_0 - V_r}{n}$$
- **Soma dos Dígitos (SDA / Cole)**: Fração decrescente ou crescente do valor depreciável baseada na soma aritmética dos anos.
- **Saldo Declinante**: Depreciação acelerada por percentual fixo aplicado ao valor contábil residual.

### 3.8. Taxa Efetiva Anual e Periódica (`sv_08_calculate_effective_rate`, `ui_10`)
Conversão de taxas nominais declaradas para taxas efetivas capitalizadas periodicamente:
$$i_{efetiva} = \left(1 + \frac{i_{nominal}}{m}\right)^m - 1$$

### 3.9. Taxa Mínima de Atratividade - TMA (`sv_09_calculate_minimum_return`, `ui_11`)
Determinação da TMA a partir de taxa livre de risco, prêmio de risco do projeto e custo de oportunidade.

### 3.10. Efeito Fisher - Inflação e Taxas Aparentes (`sv_10_calculate_fisher`, `ui_12`)
Cálculo da relação estrita entre taxa de juros aparente ($i_a$), taxa de juros real ($i_r$) e taxa de inflação ($j$):
$$(1 + i_a) = (1 + i_r) \cdot (1 + j)$$

### 3.11. Valor no Período K (`sv_11_calculate_value_at_k`)
Determinação rápida do saldo devedor, juros acumulados ou valor futuro em um período intermediário $k$ sem necessidade de gerar a planilha integral.

### 3.12. VPL com Tributação e Impostos (`sv_12_calculate_vpl_with_taxes`, `ui_13`)
Análise de viabilidade com desconto de alíquotas de imposto de renda, benefício fiscal de depreciação do ativo e dedução de juros quando há financiamento ativo.

### 3.13. Custo Anual Uniforme Equivalente - CAUE / EAC (`sv_13_calculate_caue`, `ui_14`, `ui_20`)
Comparação econômica entre alternativas de projetos ou ativos com vidas úteis desiguais através da conversão do VPL em série anual uniforme equivalente:
$$CAUE = VPL \cdot \left[ \frac{i \cdot (1+i)^n}{(1+i)^n - 1} \right]$$

---

## 4. Instruções de Compilação e Deploy

O projeto conta com 4 perfis de saída independentes:

| Diretório de Build | Compilador / Gerador | Alvo |
| --- | --- | --- |
| `build/build_Economia_APP_mingw` | MinGW 64-bit (GCC 13.1) / Ninja | Aplicação Principal (`Economia_APP.exe`) |
| `build/build_Economia_APP_msvc_ninja` | MSVC x64 (Visual Studio 2026/18) / Ninja | Aplicação Principal (`Economia_APP.exe`) |
| `build/build_mocks_mingw` | MinGW 64-bit (GCC 13.1) / Ninja | Utilitários de Mock e Tradução |
| `build/build_mocks_msvc_ninja` | MSVC x64 (Visual Studio 2026/18) / Ninja | Utilitários de Mock e Tradução |

### 4.1. Compilação via CMakePresets

1. **Economia_APP (MinGW)**:
   ```powershell
   cmake --preset host-qt6-mingw-release
   cmake --build --preset host-qt6-mingw-release
   ```

2. **Economia_APP (MSVC Ninja)**:
   ```powershell
   cmake --preset host-qt6-msvc-release
   cmake --build --preset host-qt6-msvc-release
   ```

3. **Mocks (MinGW)**:
   ```powershell
   cmake --preset mocks-mingw-release
   cmake --build --preset mocks-mingw-release
   ```

4. **Mocks (MSVC Ninja)**:
   ```powershell
   cmake --preset mocks-msvc-release
   cmake --build --preset mocks-msvc-release
   ```

### 4.2. Prevenção de Falhas de Execução (Missing DLLs)
Os scripts de build executam `windeployqt.exe` com flags automáticas e copiam diretamente para a pasta de saída:
- `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6PrintSupport.dll`
- Plugins de plataforma: `platforms/qwindows.dll`
- Compilador MinGW: `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`
- Recursos: pasta `source/assets` e catálogo `source/language/translations`.

---

## <a id="enus"></a>English (United States)

This manual outlines the operation, mathematical algorithms, software architecture, and compilation procedures for **Economia_APP** (Financial and Engineering Economics Desktop Calculator), built in **C++17** and **Qt 6.11.1**.

---

## 1. Application Overview

**Economia_APP** is a specialized financial computation software engineered for university students, engineers, and financial professionals. It automates complex engineering economy routines, outputs explicit step-by-step mathematical derivations, produces complete loan amortization schedules, evaluates capital investments, and generates clean PDF reports.

### 1.1. Core Capabilities
- **Built-in Interactive User Manual**: Comprehensive documentation dialog accessible from **Options → Manual** or via the shortcut **`Ctrl+Shift+M`**, featuring live search, synchronized two-way table of contents, and dynamic bilingual translation.
- **Institutional About Dialog with Fast Search**: Accessible from **Options → About** or via the global shortcut **`Ctrl+Shift+A`**, featuring maximize and minimize window buttons, real-time search with match counter across all documentation tabs (History, Details, Licenses, Notices, Privacy Policy, and Release Notes), and dynamic live translation.
- **Tab-Based User Interface**: Quick navigation across financial domains (Interest, Annuities, Gradients, Rates, Amortization, Investments, Depreciation, Taxes, CAUE).
- **Step-by-Step Mathematical Derivations**: Step outputs clarifying formula substitutions, derivations, and intermediary arithmetic.
- **Collapsible History Container**: Lower dock widget for tracking, inline editing, managing, and exporting historic calculations.
- **PDF Exporting Engine**: Native vector PDF rendering via `Qt6::PrintSupport` and formatted tables.
- **Dynamic Font Configuration**: Dynamic customization of interface font family and point scale with live preview.
- **Instant Localization**: Real-time switching between Brazilian Portuguese (`pt_BR`) and US English (`en_US`) without restarting.
- **Trial Protection Management**: File-based persistent registration managing trial status under AppData.

---

## 2. How to Use the New Built-in Interactive Manual

The software includes a rich interactive manual dialog enabling users to look up full operational instructions directly within the app without needing external web browsers or PDF viewers.

### 2.1. Opening the Manual
1. In the main menu bar, click: **Options → Manual** (located directly below the *About* menu item).
2. Via the keyboard shortcut: press **`Ctrl+Shift+M`**.
3. The manual opens in an independent, non-modal window, allowing simultaneous reference and interaction with calculation tabs.
4. If the manual window is already open, selecting the menu item or pressing `Ctrl+Shift+M` restores and brings the window to the front, preventing duplicate dialog instances.

### 2.2. Real-Time Search System
The top search bar provides instant lookup across the entire manual text:
- **Instant Filtering on Typing**: As you type in the search bar, occurrences across all sections are identified and highlighted in yellow.
- **Active Match Highlighting**: The currently selected match is clearly highlighted in orange, and the viewer automatically scrolls to display it.
- **Occurrence Counter**: A label displays `X of Y` matches, showing your current position and total matches found.
- **Fast Traversal**:
  - Click **Next** or press **`Enter`** to advance to the next occurrence.
  - Click **Previous** or press **`Shift+Enter`** to move backwards.
- **Search Clearing**: Clearing the text input removes highlights and restores normal scrolling.

### 2.3. Dynamic Two-Way Table of Contents (TOC)
- **Left Navigation Sidebar**: Lists all 17 chapters of the manual.
- **Click to Navigate**: Clicking any chapter in the list immediately scrolls the rich-text reader directly to that section's header.
- **Bidirectional Scroll Sync**: Scrolling the reader pane using the mouse wheel or scrollbar automatically selects the corresponding chapter in the left list without interrupting your workflow.
- **Internal Hyperlinks and Anchors**: The text contains hyperlinked anchors allowing you to jump across related sections with one click.

### 2.4. Live Language Switching (Bilingual Live Switch)
- The manual fully supports both **Brazilian Portuguese (`pt_BR`)** and **US English (`en_US`)**.
- When you change the application language via **Options → Language**, the open manual dialog instantly retranslates its table of contents, headers, text contents, and search interface labels on the fly without closing the window.

### 2.5. Overview of the 17 Built-in Chapters
The manual covers:
1. **Overview and Product Presentation**: Purpose, didactic scope, and operational architecture.
2. **Menu Bar and Global Settings**: Menu structure, system preferences, and global shortcuts.
3. **Module 1: Simple and Compound Interest**: Capitalization regimes, indifference point, and interest calculation.
4. **Module 2: Annuities and Uniform Series**: Ordinary and due annuities, payment and future value computations.
5. **Module 3: Gradient Series (Arithmetic and Geometric)**: Uniform ($G$) and geometric ($g$) cash flow variations.
6. **Module 4: Rate Conversion and Equivalence**: Rate transformation across daily, monthly, quarterly, semi-annual, and annual terms.
7. **Module 5: Debt Amortization Systems**: SAC, Price, SAM, American, and Hamburg loan systems with grace periods.
8. **Module 6: Economic Investment Feasibility Analysis**: NPV, numerical IRR derivations, and Simple/Discounted Payback.
9. **Module 7: Asset Depreciation Methods**: Straight-line, Sum-of-the-Years'-Digits (SYD), and Declining Balance.
10. **Module 8: Effective Rates, Advanced IRR and Cost of Capital**: Compounding frequency, German system, and MIRR.
11. **Module 9: Minimum Attractive Rate of Return (MARR / TMA)**: Risk-free rate and project risk premium valuation.
12. **Module 10: Fisher Equation and Inflation**: Evaluating real vs. nominal/apparent interest rates and inflation.
13. **Module 11: NPV with Taxation and Tax Shield**: Project valuation factoring in corporate tax, depreciation tax shield, and interest deductions.
14. **Module 12: EAC - Equivalent Annual Cost and Economic Life**: Mutually exclusive equipment alternatives and optimal economic replacement.
15. **History Panel (HistoryContainer) and Manipulation**: Dock panel operation, inline note editing, item deletion, and export.
16. **Font Configuration and PDF Exporting**: Font customization and vector PDF executive report rendering.
17. **Keyboard Shortcuts, Best Practices and Troubleshooting**: Operational guidelines, keybindings table, and quick diagnostics.

### 2.6. How to Use the About Dialog and `Ctrl+Shift+A` Shortcut
The **About** dialog provides complete institutional documentation with ergonomic conveniences:
- **Fast Access**: Can be opened from any screen or tab via **Options → About** or with the global shortcut **`Ctrl+Shift+A`**.
- **Single-Instance Non-Modal Window**: Allows simultaneous reference while using the main calculation tabs. Triggering the shortcut when open restores and brings the window into focus without duplicating dialogs.
- **Minimize & Maximize Buttons**: Complete window control allowing full-screen reading or minimizing to the taskbar.
- **Built-in Search System ("Search:")**:
  - Searches keywords across all 6 tabs (*History*, *Details*, *Licenses*, *Notices*, *Privacy Policy*, and *Release Notes*).
  - Highlights occurrences directly in the text browser.
  - Visual occurrence indicator in `X of Y` format.
  - Traversal with previous/next buttons, **`Enter`** (next), and **`Shift+Enter`** (previous).
  - Automatically transitions across tabs when matches exist in other sections.
- **Dynamic Bilingual Retranslation**: Retranslates all headers, document texts, tabs, and search controls in real time upon language switch without closing the dialog.

---

## 3. Calculation Engines and Services

1. **Simple and Compound Interest** (`sv_01_calculate_interest`, `ui_03`): Solves Present Value ($P$), Future Value ($F$), Rate ($i$), and Number of periods ($n$), with comparative analysis and indifference points.
2. **Uniform Annuity Series** (`sv_02_calculate_annuity`, `ui_04`): Ordinary (postpaid) and due (prepaid) annuity payment computations.
3. **Arithmetic and Geometric Gradients** (`sv_03_calculate_gradient`, `ui_05`): Calculates present worth and equivalent annuity of uniformly increasing/decreasing cash flow streams.
4. **Rate Equivalence & Conversions** (`sv_04_calculate_real_rate_equivalence`, `ui_06`): Transforms compounding periods (daily, monthly, quarterly, semi-annual, annual).
5. **Loan Amortization Systems** (`sv_05_calculate_amortization`, `ui_07`, `ui_15`-`ui_19`): Generates full schedules for SAC (Constant Amortization), Price (French Annuity), SAM (Mixed), American, and Hamburg systems with grace period interest handling and down payments.
6. **Capital Investment Analysis** (`sv_06_calculate_investment`, `ui_08`): Computes Net Present Value (NPV), Internal Rate of Return (IRR), and Simple/Discounted Payback periods.
7. **Asset Depreciation** (`sv_07_calculate_depreciation`, `ui_09`): Straight-line, Sum-of-the-Years'-Digits (SYD), and Declining Balance methods.
8. **Effective Interest Rates** (`sv_08_calculate_effective_rate`, `ui_10`): Translates nominal rates into true effective rates per compounding interval.
9. **Minimum Attractive Rate of Return - MARR** (`sv_09_calculate_minimum_return`, `ui_11`): Risk-free plus project risk premium valuation.
10. **Fisher Effect** (`sv_10_calculate_fisher`, `ui_12`): Computes exact inflation-adjusted real vs. apparent (nominal) rates.
11. **Value at Period K** (`sv_11_calculate_value_at_k`): Directly calculates outstanding balance and interest at period $k$.
12. **NPV with Taxation** (`sv_12_calculate_vpl_with_taxes`, `ui_13`): Project valuation incorporating corporate tax rates and depreciation tax shields.
13. **Equivalent Annual Cost - CAUE / EAC** (`sv_13_calculate_caue`, `ui_14`, `ui_20`): Comparing mutually exclusive equipment alternatives with unequal lifespans.

---

## 4. Build & Deployment Manual

Builds are kept strictly isolated in 4 dedicated target directories:
- `build/build_Economia_APP_mingw` (MinGW 64-bit / Ninja)
- `build/build_Economia_APP_msvc_ninja` (MSVC x64 / Ninja)
- `build/build_mocks_mingw` (MinGW 64-bit / Ninja)
- `build/build_mocks_msvc_ninja` (MSVC x64 / Ninja)

Each build target executes automated deployment ensuring no runtime DLL errors (`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6PrintSupport.dll`, `libgcc_s_seh-1.dll`, `platforms/qwindows.dll`).
