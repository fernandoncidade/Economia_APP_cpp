# Guia do Usuário — Economia_APP v2026.11.22.0

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-user-guide--economia_app-v202611220)

<a id="-guia-do-usuario-pt-br"></a>
## 🇧🇷 Módulos Financeiros Disponíveis

O **Economia_APP** reúne 12 abas analíticas integradas em interface desktop Qt6:

1. **Juros Simples e Compostos**: Cálculo de Montante ($F$), Capital Inicial ($P$) e comparação direta entre capitalização simples e composta.
2. **Séries de Pagamentos (Anuidades)**: Séries postecipadas e antecipadas para obtenção de prestação ($A$) ou valor presente ($P$), incluindo taxa nula.
3. **Séries Gradientes**: Gradiente aritmético ($P$, perpetuidade e termo $G_k$) e gradiente geométrico ($P$ e termo $X_k$).
4. **Equivalência de Taxas e Taxa Real**: Conversão equivalente entre períodos arbitrários e cálculo de taxa aparente/real pela equação de Fisher.
5. **Sistemas de Amortização**: Tabelas completas de **Price**, **SAC**, **SAM**, **Americano** e **Hamburguês** (com suporte a carência e capitalização de juros), consulta por parcela $k$ e cartões com tabela anexa.
6. **Análise de Investimentos**: **VPL / VAUE** uniforme ou detalhado (receitas e custos), **Payback Descontado** e **Análise de Sensibilidade**.
7. **Depreciação**: Métodos **Linear**, **Soma dos Dígitos Decrescente**, **Soma dos Dígitos Crescente** e **Saldo Declinante**.
8. **Taxa Efetiva, TIR e Taxas Especiais**: 7 modos integrados — Taxa Efetiva, TIR (Newton-Raphson), Taxa Global (inflação acumulada em 3 períodos), Cobrança Antecipada, TIRm, TMA vs Rentabilidade e Juros Reais.
9. **Retorno Mínimo**: Amortização / receita mínima necessária para recuperar o capital inicial à TMA informada.
10. **Equação de Fisher**: Cálculo bidirecional entre TMA Nominal ($i_a$), TMA Real ($i_r$) e Inflação ($\theta$).
11. **VPL com Impostos e Financiamento**: Fluxo líquido com depreciação, IRPJ, CSLL, parcelamento financiado, valor residual e venda antecipada.
12. **CAUE — Vida Econômica**: Simulação tabular interativa com custo de aquisição, valores de revenda ($VR_n$), custos anuais ($Com_n$) e indicação automática da vida econômica ótima ($CAUE_n$).

### Recursos do Espaço de Trabalho
- **Histórico em Cartões Independentes**: Editar cálculo inline, excluir selecionados, limpar entrada/saída ou limpar tudo.
- **Renderização Matemática Vetorial (`MathRenderer`)**: Frações alinhadas e radicais vetoriais redesenhados nativamente em temas Claro e Escuro.
- **Exportação PDF (`Arquivo > Exportar...`)**: Geração offline de relatórios formatados com tabelas e fórmulas vetoriais.
- **Idioma e Fonte**: Troca instantânea `pt_BR` / `en_US` em runtime e configuração persistente de tipografia.

---

<a id="-user-guide--economia_app-v202611220"></a>
## 🇺🇸 User Guide — Economia_APP (`v2026.11.22.0`)

**Economia_APP** provides 12 analytical Financial Mathematics & Engineering Economics modules (Interest, Annuities, Gradients, Rate Conversions & Fisher, 5 Amortization schedules including French/SAC/SAM/American/Hamburgues, Investment Appraisal VPL/VAUE/Payback/Sensitivity, 4 Depreciation models, Effective Rate / IRR / MIRR / Global Rate, Minimum Return, Fisher Equation, After-Tax NPV with Financing, and EAC Economic Life), complete with inline card editing, vector math rendering, offline PDF export, session save/restore, and live `pt_BR`/`en_US` switching.
