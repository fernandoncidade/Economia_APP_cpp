# OBSERVAÇÃO TÉCNICA - Estado atual do projeto

Versão documentada: `2026.10.7.0`
Data do snapshot técnico: `7 de outubro de 2026`
Repositório: `Economia_APP_cpp`  

---

## 1. Escopo deste documento

Este documento consolida o estado técnico oficial do projeto **Economia_APP_cpp** após a refatoração integral e fiel do código Python original (`c:\Users\ferna\APLICATIVOS\PY\Economia_APP`) para **C++17** e **Qt 6.11.1**, além da separação estrita dos ambientes de compilação da aplicação principal e dos utilitários de mocks para os compiladores **MinGW** e **MSVC (Ninja)**.

---

## 2. Separação Estrita dos Diretórios de Build

Conforme especificado, a geração dos binários da aplicação principal (`Economia_APP`) e dos utilitários de desenvolvimento (`mocks`) ocorre de forma totalmente desacoplada e independente em 4 diretórios dedicados:

```
C:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\build\
├── build_Economia_APP_mingw/         # Executável principal compilado via GCC 13.1 (MinGW 64-bit)
├── build_Economia_APP_msvc_ninja/    # Executável principal compilado via MSVC x64 (Ninja)
├── build_mocks_mingw/                # Ferramentas auxiliares compiladas via MinGW 64-bit
└── build_mocks_msvc_ninja/           # Ferramentas auxiliares compiladas via MSVC x64 (Ninja)
```

### 2.1. Alvos Gerados por Diretório

| Diretório de Build | Alvos Principais | Finalidade |
| --- | --- | --- |
| `build_Economia_APP_mingw` | `Economia_APP.exe`, `teste1.exe` | Produto final com runtime MinGW |
| `build_Economia_APP_msvc_ninja` | `Economia_APP.exe`, `teste1.exe` | Produto final com runtime MSVC |
| `build_mocks_mingw` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe`, `CompileTranslationsTerminal.exe` | Utilitários de versão e tradução MinGW |
| `build_mocks_msvc_ninja` | `find_VersionEditor.exe`, `CompileTranslationsGUI.exe`, `CompileTranslationsTerminal.exe` | Utilitários de versão e tradução MSVC |

---

## 3. Prevenção e Imunização contra Falhas de DLL Ausente

Para assegurar que qualquer binário executável gerado inicialize diretamente (seja via VS Code, terminal comum ou duplo clique no Windows Explorer) sem relatar erros de bibliotecas dinâmicas ausentes, cada alvo de compilação implementa rotinas automáticas de deploy via `POST_BUILD` no CMake:

### 3.1. Tratamento Específico de Cada Dependência Crítica

1. **`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6PrintSupport.dll`**:
   - `windeployqt.exe` é invocado automaticamente após a linkedição.
   - Um comando adicional do CMake copia preventivamente tais DLLs da pasta `bin/` do Qt para o diretório raiz do executável caso ainda não estejam presentes.
2. **`libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`**:
   - Para compilações sob MinGW, o CMake copia explicitamente os runtimes do GCC diretamente de `C:\Qt\Tools\mingw1310_64\bin\` ou `C:\Qt\6.11.1\mingw_64\bin\` para o diretório de saída do binário. Isso elimina de forma definitiva a mensagem:
     `"A execução de código não pode continuar porque libgcc_s_seh-1.dll não foi encontrado."`
3. **`platforms/qwindows.dll`**:
   - O plugin de plataforma QPA do Qt é copiado para a subpasta `platforms/` adjacente ao executável, garantindo que o subsistema gráfico do Windows seja inicializado sem requerer variáveis de ambiente globais.
4. **Recursos e Ativos (`source/assets` e `source/language/translations`)**:
   - As pastas completas de ícones, termos legais e traduções binárias (`.qm`) são sincronizadas em tempo de compilação para o diretório de execução.

---

## 4. Arquitetura Modular do Código-Fonte

A base em C++ espelha com fidelidade absoluta a estrutura original do projeto:

### 4.1. Serviços de Cálculo Financeiro (`source/services/`)
- `sv_01_calculate_interest`: Cálculos de Juros Simples e Compostos.
- `sv_02_calculate_annuity`: Séries uniformes de pagamentos (postecipada e antecipada).
- `sv_03_calculate_gradient`: Séries com gradiente aritmético e geométrico.
- `sv_04_calculate_real_rate_equivalence`: Conversão e equivalência de taxas.
- `sv_05_calculate_amortization`: Motor matemático de amortização.
- `sv_06_calculate_investment`: Avaliação de investimentos (VPL, TIR, Payback).
- `sv_07_calculate_depreciation`: Métodos de depreciação contábil.
- `sv_08_calculate_effective_rate`: Cálculo da taxa efetiva por capitalização periódica.
- `sv_09_calculate_minimum_return`: Determinação de Taxa Mínima de Atratividade (TMA).
- `sv_10_calculate_fisher`: Aplicação do efeito Fisher (inflação x juros reais).
- `sv_11_calculate_value_at_k`: Avaliação direta do saldo no período $k$.
- `sv_12_calculate_vpl_with_taxes`: VPL com tributação e benefício fiscal.
- `sv_13_calculate_caue`: Custo Anual Uniforme Equivalente.

### 4.2. Módulos de Interface com o Usuário (`source/ui/`)
- `ui_01_create_layout`: Montagem da janela principal e distribuição em abas.
- `ui_02_get_float_from_line_edit`: Sanitização e extração segura de entradas numéricas com suporte a vírgula/ponto.
- `ui_03_create_interest_tab` a `ui_14_create_caue_tab`: Construção de cada uma das abas especializadas.
- `ui_15_generate_sac_table` a `ui_20_generate_caue_input_table`: Geradores de tabelas de amortização (SAC, SAM, PRICE, Hamburguês, Americano e tabela de CAUE).
- `ui_21_set_amort_table_row` e `ui_22_get_table_data`: Manipulação e leitura de células de tabelas.
- `ui_23_history_container`: Container dock inferior para visualização, edição, remoção e exportação do histórico.
- `ui_24_font_config_dialog`: Configuração interativa de famílias e tamanhos tipográficos.
- `ui_25_export_pdf`: Exportação vetorial de tabelas para documentos PDF.
- `ui_26_menu_bar`: Barra de menus com ações de configuração, visualização e ajuda (incluindo atalho `Ctrl+Shift+M` para o Manual).
- `ui_27_SobreDialog`, `ui_28_exibir_sobre`, `ui_29_opcoes_sobre`: Caixa de diálogo Sobre com navegação de licenças e termos.
- `ui_30_Manual`: Estruturas de dados (`ManualSection`, `ManualDetails`, `ManualBlock`), gerador de blocos ricos, mapeamento de posições e utilitários de texto.
- `ui_31_manual_pt_BR`: Conteúdo textual e sumário de 17 capítulos detalhados em Português do Brasil.
- `ui_32_manual_en_US`: Conteúdo textual e sumário equivalente de 17 capítulos detalhados em Inglês (US).
- `ui_33_exibir_manual`: Diálogo não modal do Manual Interativo com busca instantânea (`ExtraSelection`), contador de ocorrências, navegação bidirecional sincronizada entre sumário e visualizador, e suporte a alternância dinâmica de idioma.

### 4.3. Utilitários do Sistema (`source/utils/`)
- `ApplicationPathUtils`: Resolução de caminhos locais e diretórios de execução.
- `CaminhoPersistenteUtils`: Localização de armazenamento no AppData (`%LOCALAPPDATA%/Economia_APP`).
- `DialogHelper`: Tradução e localização dinâmica em tempo de execução dos botões padrão de caixas de diálogo (`QMessageBox`), garantindo "Sim"/"Não" e "Yes"/"No" de acordo com o idioma ativo.
- `FontManager`: Gerenciamento e aplicação uniforme de fontes na interface.
- `IconUtils`: Carregamento desacoplado de ícones vetoriais e rasterizados.
- `LogManager`: Registro unificado de diagnóstico em disco.
- `MathRenderer`: Renderização tipográfica e visual de fórmulas, frações, expoentes e expressões com radiciação.
- `SessionManager`: Serialização, restauração e expurgo completo de cálculos e históricos em disco e memória com confirmação de usuário.
- `TextFormat`: Utilitários de formatação de moedas e percentuais.
- `TrialManager`: Controle do período de testes com criptografia leve/registro persistente.

### 4.4. Internacionalização (`source/language/`)
- `tr_01_gerenciadorTraducao`: Instalação e alternância de tradutores `QTranslator` em tempo de execução.
- `tr_02_compileTranslations`: Compilação de catálogos de tradução `.ts` para binários `.qm`.

### 4.5. Subsistema do Manual Interativo
- O subsistema de manual foi desenhado de forma desacoplada e independente:
  - Disparado pelo menu **Opções → Manual** ou pelo atalho **`Ctrl+Shift+M`** gerenciado em `ui_26_menu_bar.cpp` e conectado a `FinancialCalculatorApp::abrir_manual()`.
  - Instância única inteligente (`s_manualDialog`): se já estiver aberta, restaura o foco sem duplicar janelas.
  - Conexão reativa com o sinal `idioma_alterado` de `GerenciadorTraducao`: atualiza dinamicamente textos, sumários e controles de busca em tempo real caso o usuário mude o idioma da aplicação.

### 4.6. Melhorias Estruturais e Funcionalidades da Versão 2026.10.7.0
- **Quadros de Resultados Independentes com Rolagem Vertical (`ui_23_history_container`)**:
  - Cada cálculo gera um card desacoplado encapsulado em `QScrollArea`, mantendo seu tamanho nativo original e política de redimensionamento sem disputar área de exibição ou esmagar saídas analíticas vizinhas;
  - Navegação suave por scrollbar vertical em todos os 12 módulos de cálculo financeiro.
- **Tabelas de Amortização Dinâmicas no Histórico (`sv_05_calculate_amortization`, `ui_07`, `ui_15`-`ui_19`)**:
  - Cronogramas completos (SAC, Price, SAM, Americano e Hamburguês) são instanciados e acoplados diretamente ao card de cálculo no container de histórico de resultados, seguindo o padrão já consolidado da aba CAUE;
  - Eliminação definitiva de widgets de tabela esmagados e compactados no layout principal da aba de Amortização;
  - Possibilidade de gerar, comparar e manter múltiplos cronogramas de financiamento na mesma sessão de trabalho.
- **Gerenciamento e Limpeza Completa de Sessão (`SessionManager`, `DialogHelper`, `fca_01_FinancialCalculatorAPP`)**:
  - Função `SessionManager::limpar_sessao()` acionada pelo menu **Arquivos → Limpar Sessão Salva**;
  - Apresenta caixa de diálogo de confirmação ("Limpar Sessão") com botões localizados dinamicamente ("Sim" / "Não");
  - Ao confirmar, expurga em cascata todos os quadros, respostas textuais e cronogramas de todas as 12 abas analíticas (tanto de cálculos ativos em memória quanto restaurados de arquivo prévio);
  - Exclui com segurança o arquivo de persistência em disco (`%LOCALAPPDATA%/Economia_APP/sessao.json`) e exibe notificação informativa de conclusão com sucesso.
- **Tradução Dinâmica de Diálogos em Tempo de Execução (`DialogHelper`)**:
  - Implementação de utilitário dedicado para sobrescrever e traduzir os botões padrão das janelas de diálogo do Qt (`QMessageBox`), garantindo "Sim" / "Não" em português e "Yes" / "No" em inglês em todos os diálogos de restauração e limpeza de sessão.
- **Exportação Bilíngue Inteligente de Relatórios em PDF (`ui_25_export_pdf`, `fca_01_FinancialCalculatorAPP`)**:
  - Ajuste dinâmico automático dos nomes de arquivos sugeridos e títulos em **Arquivos → Exportar Atual** e **Arquivos → Exportar Todos** de acordo com a linguagem ativa da aplicação (`pt_BR` exporta nomes como `amortizacao.pdf`, enquanto `en_US` gera `amortization.pdf`).

---

## 5. Ferramentas Auxiliares em `mocks/`

- **`find_VersionEditor`**: Utilitário GUI e de linha de comando (`--check`) para sincronização e validação de versões em todos os documentos legais (`ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE`), código C++ e manuais.
- **`CompileTranslationsGUI`**: Interface gráfica amigável para compilação selecionada ou em lote de arquivos `.ts` para `.qm`.
- **`CompileTranslationsTerminal`**: Utilitário de terminal com suporte a flags (`--all`, `--module economia`, `--list`).
- **`launch_detached_gui.cmake`**: Script auxiliar para disparo desacoplado de interfaces gráficas a partir do CMake.
- **`limpar_build_qt6_regeneravel.ps1`**: Script PowerShell para limpeza segura de pastas de build regeneráveis.

### Diretório Principal (`root`)

Economia_APP_cpp\teste.cpp
Economia_APP_cpp\.gitattributes
Economia_APP_cpp\CMakeLists.txt
Economia_APP_cpp\CMakePresets.json
Economia_APP_cpp\LICENSE
Economia_APP_cpp\main.cpp
Economia_APP_cpp\MANUAL.md
Economia_APP_cpp\OBSERVACAO.md
Economia_APP_cpp\README.md

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\source`

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

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\cmake`

```
├── 📄 copy_msvc_runtime.cmake
├── 📄 copy_optional_directory.cmake
├── 📄 copy_qm_translations.cmake
├── 📄 deploy_optional_qt_binary.cmake
├── 📄 prune_optional_root_runtime_dlls.cmake
└── 📄 remove_compiler_runtime_installer.cmake
```

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\.vscode`

```
├── ⚙️ c_cpp_properties.json
├── ⚙️ cmake-kits.json
├── ⚙️ settings.json
└── ⚙️ tasks.json
```

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\mocks`

```
├── 📁 icones
│   ├── 📁 en_US
│   │   ├── 🖼️ Captura de tela 2025-10-21 155020.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 155041.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 155104.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 155119.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 155137.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 155155.png
│   │   └── 🖼️ Captura de tela 2025-10-21 155215.png
│   ├── 📁 pt_BR
│   │   ├── 🖼️ Captura de tela 2025-10-21 154812.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 154832.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 154849.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 154905.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 154924.png
│   │   ├── 🖼️ Captura de tela 2025-10-21 154944.png
│   │   └── 🖼️ Captura de tela 2025-10-21 155001.png
│   ├── 📄 economia.ico
│   ├── 🖼️ economia.png
│   ├── 🖼️ economia_1440-2160.png
│   ├── 🖼️ economia_150-150.png
│   ├── 🖼️ economia_2160-2160.png
│   ├── 🖼️ economia_300-300.png
│   ├── 🖼️ economia_71-71.png
│   └── 📄 mocks.rc
├── 📄 CMakeLists.txt
├── ⚡ CompileTranslationsGUI.cpp
├── ⚡ CompileTranslationsGUI.hpp
├── ⚡ CompileTranslationsTerminal.cpp
├── ⚡ CompileTranslationsTerminal.hpp
├── 📄 INSTRUCOES_find_VersionEditor.txt
├── ⚡ find_VersionEditor.cpp
├── ⚡ find_VersionEditor.hpp
├── 📄 launch_detached_gui.cmake
└── 📄 limpar_build_qt6_regeneravel.ps1
```

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Economia_APP_cpp\repo`

```
├── 📄 Economia_APP.iss
├── 📄 Economia_APP_MSVC.iss
└── 📄 Economia_APP_MinGW.iss
```
