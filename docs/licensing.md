# Modelo de Duplo Licenciamento — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-dual-licensing-model--economia_app)

<a id="-licenciamento-pt-br"></a>
## 🇧🇷 Escopo e Regras do Duplo Licenciamento

O projeto **Economia_APP** adota formalmente um modelo de **duplo licenciamento**, diferenciando de maneira clara o **código-fonte aberto deste repositório** da **distribuição oficial empacotada publicada na Microsoft Store**:

1. **Código-Fonte Público (GNU Affero General Public License v3.0 — AGPLv3)**:
   - O arquivo [`LICENSE`](../LICENSE) e sua cópia empacotada em [`source/assets/LICENSES/GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt`](../source/assets/LICENSES/GNU%20AFFERO%20GENERAL%20PUBLIC%20LICENSE%20Version3.txt) regem o código-fonte deste repositório (`source/`, `mocks/`, `tests/`, `cmake/`, scripts e documentação).
   - O licenciado pode usar, estudar, modificar e redistribuir o código-fonte nos termos da **GNU AGPLv3**, preservando avisos de direitos autorais e disponibilizando o código-fonte correspondente conforme a Seção 13 da licença.

2. **Distribuição Oficial na Microsoft Store (`9PLR0KD6KSJJ` — EULA e CLC Proprietários/Oficiais)**:
   - O pacote executável e instalador oficial publicado pelo autor (**Fernando Nillsson Cidade**) na Microsoft Store (`https://apps.microsoft.com/detail/9PLR0KD6KSJJ`) é regido pelos termos do **Contrato de Licença de Usuário Final (EULA)** e do **Contrato de Licença Customizado (CLC)** presentes em [`source/assets/EULA/`](../source/assets/EULA/EULA_pt_BR%20-%20Economia.txt) e [`source/assets/CLC/`](../source/assets/CLC/CLC_pt_BR%20-%20Economia.txt), em conjunto com o contrato padrão da Microsoft Store.
   - Em caso de uso da versão distribuída via loja oficial, aplicam-se as condições comerciais, de garantia e responsabilidade estabelecidas no EULA/CLC oficial da distribuição.

3. **Liberdade Editorial do Desenvolvedor em `source/assets/`**:
   - Textos, manuais, ícones institucionais e ativos documentais sob [`source/assets/`](../source/assets/) são mantidos para alinhamento direto com o empacotamento desktop e Microsoft Store, garantindo fidelidade jurídica, integridade de marca e clareza contratual sem gerar conflito com os direitos copyleft da **AGPLv3** sobre o código-fonte público.

4. **Caráter Auxiliar e Fornecimento `"NO ESTADO EM QUE SE ENCONTRA"` (`"AS IS"`)**:
   - Todos os cálculos de Engenharia Econômica e Matemática Financeira fornecidos pelo aplicativo possuem finalidade **educacional e auxiliar de apoio à decisão** e são fornecidos `"NO ESTADO EM QUE SE ENCONTRA"` (`"AS IS"`), sem garantias implícitas de comercialização, adequação a propósito específico ou substituição de parecer contábil/financeiro profissional certificado.

### Bibliotecas de Terceiros

- **Qt 6.12.0** (`Core`, `Gui`, `Widgets`, `PrintSupport`, e `Test` apenas na suíte interna de desenvolvimento): utilizado conforme [`source/assets/NOTICES/NOTICE_pt_BR.txt`](../source/assets/NOTICES/NOTICE_pt_BR.txt).

---

<a id="-dual-licensing-model--economia_app"></a>
## 🇺🇸 Dual Licensing Model — Economia_APP

- **Public Source Code (`GNU AGPLv3`)**: All C++ source code, CMake scripts, test suite, and technical documentation in this repository are licensed under the **GNU Affero General Public License v3.0 (AGPLv3)** as set out in [`LICENSE`](../LICENSE).
- **Official Microsoft Store Distribution (`9PLR0KD6KSJJ`)**: Official prebuilt Store releases distributed by **Fernando Nillsson Cidade** are governed by the official **EULA** ([`EULA_en_US - Economia.txt`](../source/assets/EULA/EULA_en_US%20-%20Economia.txt)) and **CLC** ([`CLC_en_US - Economia.txt`](../source/assets/CLC/CLC_en_US%20-%20Economia.txt)), alongside Microsoft Store Standard Application License Terms.
- **Auxiliary Scope & `"AS IS"` Disclaimer**: Results are intended strictly as computational support tools for Financial Mathematics and Engineering Economics and are provided **"AS IS"** without warranty of any kind.
