#include "ui_31_manual_pt_BR.hpp"

namespace source::ui::manual_pt_BR {

using source::ui::Manual::ManualDetails;
using source::ui::Manual::ManualSection;

QString manual_intro_text()
{
    return QStringLiteral("Este manual detalha o funcionamento, as bases matemáticas e o passo a passo completo para utilização de todos os recursos da Calculadora de Engenharia Econômica e Finanças (Economia_APP).");
}

QString manual_toc_title()
{
    return QStringLiteral("Índice Geral");
}

QList<ManualSection> get_manual_document()
{
    return {
        {
            QStringLiteral("visao-geral"),
            QStringLiteral("1. Visão Geral e Apresentação do Produto"),
            {
                QStringLiteral("O Economia_APP é uma plataforma desktop de alta precisão desenvolvida em C++17 e Qt 6, projetada especificamente para estudantes, engenheiros de produção, peritos judiciais, gestores financeiros e analistas de investimentos."),
                QStringLiteral("O aplicativo automatiza cálculos financeiros e de engenharia econômica de alta complexidade, substituindo planilhas propensas a erros por motores numéricos verificados, com apresentação transparente de todas as etapas de cálculo passo a passo, fórmulas matemáticas renderizadas, gráficos analíticos e cronogramas completos de amortização."),
                QStringLiteral("A interface principal foi construída com navegação por abas temáticas, combinada a um contêiner de histórico retrátil integrado na saída de cada aba, garantindo ergonomia visual, controle total sobre o histórico de simulações e capacidade de exportação em relatórios PDF de alta qualidade.")
            },
            {
                QStringLiteral("12 Abas de Cálculo Especializadas cobrindo todas as áreas da matemática financeira clássica e engenharia econômica moderna."),
                QStringLiteral("Demonstração Didática Passo a Passo com fórmulas deduzidas, substituições numéricas intermediárias e resultado final com precisão configurável."),
                QStringLiteral("Painel Retrátil de Histórico (HistoryContainer) com busca integrada por texto, edição direta de registros, exclusão de itens e exportação em TXT ou PDF."),
                QStringLiteral("Exportação de Relatórios em PDF Profissional via subsistema Qt6 PrintSupport com cabeçalhos formais, paginação e suporte a impressão."),
                QStringLiteral("Renderização Matemática Tipográfica com radiciação, expoentes sobrescritos, índices subscritos e frações legíveis."),
                QStringLiteral("Suporte Bilíngue Dinâmico com alternância imediata entre Português (Brasil) e Inglês (Estados Unidos) em tempo de execução sem reinício."),
                QStringLiteral("Personalização Tipográfica em Tempo Real com seleção de família de fontes e ajuste de escala de leitura."),
                QStringLiteral("Sistema de Licenciamento Local (Trial de 30 dias) com verificação de integridade e registro protegido em pasta AppData.")
            },
            {
                {
                    QStringLiteral("Arquitetura e Fluxo de Dados"),
                    {
                        QStringLiteral("Cada aba do sistema é composta por um formulário à esquerda/topo com validação rígida de tipos numéricos (QDoubleValidator) e uma área de saída com histórico à direita/fundo."),
                        QStringLiteral("O motor de cálculo executa a rotina através de serviços independentes (namespace source::services), repassando os resultados para o formatador de texto (TextFormat) e renderizador de matemática (MathRenderer).")
                    },
                    {
                        QStringLiteral("Isolamento entre lógica de negócios e interface com o usuário."),
                        QStringLiteral("Proteção preventiva contra travamento da interface em processamentos iterativos (TIR, Newton-Raphson, Amortização longa e CAUE)."),
                        QStringLiteral("Histórico cumulativo preservado ao alternar entre abas durante a mesma sessão de trabalho.")
                    }
                }
            }
        },
        {
            QStringLiteral("barra-menus-configuracoes"),
            QStringLiteral("2. Barra de Menus e Configurações Globais"),
            {
                QStringLiteral("A barra de menus superior concentra as ferramentas essenciais de gerenciamento de arquivos, preferências do usuário e documentação de suporte do Economia_APP."),
                QStringLiteral("Todos os comandos podem ser acionados via mouse ou por atalhos rápidos de teclado padronizados.")
            },
            {
                QStringLiteral("Menu Arquivos → Exportar Atual: gera o relatório em PDF dos cálculos presentes na aba ativa com nomenclatura e cabeçalhos automáticos no idioma configurado (ex.: amortizacao.pdf ou amortization.pdf)."),
                QStringLiteral("Menu Arquivos → Exportar Todos: compila automaticamente um dossiê PDF executivo multipágina unificando todas as abas calculadas (todos_os_calculos.pdf ou all_calculations.pdf)."),
                QStringLiteral("Menu Arquivos → Restaurar Sessão Anterior: recarrega a memória de cálculo e todos os formulários da sessão anterior salvos no disco."),
                QStringLiteral("Menu Arquivos → Limpar Sessão Salva: abre uma janela de diálogo 'Limpar Sessão' solicitando confirmação ('Sim' ou 'Não'). Ao confirmar 'Sim', limpa imediatamente todos os quadros, respostas e tabelas de todas as 12 abas e métodos na tela (tanto cálculos recém-executados quanto dados restaurados), apaga o arquivo de sessão do disco e emite uma janela informativa de sucesso ('Limpar Sessão: Sessão salva limpa com sucesso.'). Se cancelado 'Não', mantém todos os dados intactos."),
                QStringLiteral("Menu Configuração → Idiomas: submenu com opções 'Português (Brasil)' e 'English (United States)'. A seleção reconfigura instantaneamente todos os rótulos, botões de diálogo ('Sim'/'Não' vs. 'Yes'/'No'), menus e janelas auxiliares abertas."),
                QStringLiteral("Menu Configuração → Configurar Fontes: abre o diálogo FontConfigDialog para personalizar a família da fonte (ex: Segoe UI, Roboto, Consolas) e o tamanho em pontos da interface."),
                QStringLiteral("Menu Opções → Sobre (Ctrl+Shift+A): abre a janela institucional detalhada contendo botões de maximizar e minimizar, mecanismo de busca em tempo real com destaque de ocorrências e contador, Histórico do Projeto, Detalhes Técnicos, Licenças de Terceiros, Avisos Legais, Política de Privacidade e Notas de Lançamento."),
                QStringLiteral("Menu Opções → Manual (Ctrl+Shift+M): abre esta janela interativa com visualização bilíngue, índice estruturado e mecanismo de busca em tempo real.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Gerenciamento e Limpeza de Sessão"),
                    {
                        QStringLiteral("Para reiniciar a área de trabalho ou descartar dados acumulados:")
                    },
                    {
                        QStringLiteral("1. Acesse o menu Arquivos → Limpar Sessão Salva."),
                        QStringLiteral("2. Na janela de diálogo de confirmação 'Limpar Sessão', clique em 'Sim' (ou 'Não' para cancelar)."),
                        QStringLiteral("3. Ao clicar em 'Sim', todas as respostas e tabelas de todas as 12 abas são apagadas e o arquivo no disco é removido."),
                        QStringLiteral("4. A janela 'Limpar Sessão: Sessão salva limpa com sucesso.' confirma a conclusão da limpeza.")
                    }
                },
                {
                    QStringLiteral("Passo a Passo: Personalização de Fontes"),
                    {
                        QStringLiteral("Para adaptar a legibilidade em monitores de alta resolução (4K) ou preferências visuais do operador:")
                    },
                    {
                        QStringLiteral("1. Acesse o menu Configuração → Configurar Fontes."),
                        QStringLiteral("2. No seletor 'Família da Fonte', escolha a tipografia de sua preferência."),
                        QStringLiteral("3. No controle 'Tamanho da Fonte', ajuste o valor em pontos (ex: 9, 10, 11 ou 12 pt)."),
                        QStringLiteral("4. Observe a alteração na caixa de pré-visualização em tempo real."),
                        QStringLiteral("5. Clique em 'OK' para aplicar imediatamente a todos os componentes do sistema."),
                        QStringLiteral("6. Caso deseje retornar ao padrão do sistema operacional, clique no botão 'Restaurar'.")
                    }
                },
                {
                    QStringLiteral("Passo a Passo: Alternância de Idioma"),
                    {
                        QStringLiteral("O software implementa um gerenciador dinâmico de traduções (GerenciadorTraducao):")
                    },
                    {
                        QStringLiteral("1. Acesse o menu Configuração → Idiomas."),
                        QStringLiteral("2. Clique em 'Português (Brasil)' ou 'English (United States)'."),
                        QStringLiteral("3. Toda a interface gráfica, inclusive botões de diálogos ('Sim'/'Não' ou 'Yes'/'No'), tabelas e janelas auxiliares abertas, é retraduzida no mesmo milissegundo, preservando integralmente os valores digitados.")
                    }
                }
            }
        },
        {
            QStringLiteral("juros-simples-compostos"),
            QStringLiteral("3. Módulo 1: Juros Simples e Compostos"),
            {
                QStringLiteral("O módulo de Juros Simples e Compostos permite apurar o valor do dinheiro no tempo sob regimes de capitalização linear (juros simples) e exponencial (juros compostos)."),
                QStringLiteral("O módulo permite resolver o Valor Futuro (Montante F), o Valor Presente (Principal P) ou realizar a análise comparativa direta entre os dois regimes.")
            },
            {
                QStringLiteral("Fórmula de Juros Simples: F = P * (1 + i * n) e Juros J = P * i * n."),
                QStringLiteral("Fórmula de Juros Compostos: F = P * (1 + i)^n e Principal P = F / (1 + i)^n."),
                QStringLiteral("Modo Comparar JS vs JC: gera uma demonstração simultânea evidenciando o efeito dos juros sobre juros e o ponto de divergência patrimonial."),
                QStringLiteral("Entrada de taxa de juros (i) expressa em porcentagem ao período correspondente à unidade de tempo de n."),
                QStringLiteral("Validação automática para impedir taxas ou prazos inconsistentes.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Siga o procedimento abaixo para executar o cálculo:")
                    },
                    {
                        QStringLiteral("1. Clique na aba 'Juros Simples e Compostos'."),
                        QStringLiteral("2. No primeiro menu suspenso, escolha o objetivo: 'Calcular Montante (F)', 'Calcular Principal (P)' ou 'Comparar JS vs JC'."),
                        QStringLiteral("3. No segundo menu suspenso, defina o regime desejado ('Juros Compostos' ou 'Juros Simples')."),
                        QStringLiteral("4. Preencha os campos habilitados: Valor Principal (P), Valor do Montante (F), Taxa de Juros (% ao período) e Número de Períodos (n)."),
                        QStringLiteral("5. Clique no botão 'Calcular' ou pressione a tecla Enter em qualquer campo de texto."),
                        QStringLiteral("6. Analise o relatório detalhado exibido no quadro de histórico com a memória de cálculo completa."),
                        QStringLiteral("7. Utilize os botões inferiores para Limpar Entrada, Limpar Saída, Editar Cálculo ou Exportar PDF.")
                    }
                }
            }
        },
        {
            QStringLiteral("anuidades-series-uniformes"),
            QStringLiteral("4. Módulo 2: Anuidades e Séries Uniformes"),
            {
                QStringLiteral("Este módulo resolve séries homogêneas de pagamentos ou depósitos periódicos constantes (A), aplicando os fatores clássicos de equivalência financeira da engenharia econômica."),
                QStringLiteral("Suporta tanto séries postecipadas (pagamentos ao término de cada período) quanto séries antecipadas (pagamentos no início do período, como em aluguéis ou parcelas com entrada imediata).")
            },
            {
                QStringLiteral("Postecipada - Valor Presente: P = A * [((1 + i)^n - 1) / (i * (1 + i)^n)], através do Fator de Valor Presente (FVP)."),
                QStringLiteral("Postecipada - Montante Futuro: F = A * [((1 + i)^n - 1) / i], através do Fator de Acumulação de Capital (FAC)."),
                QStringLiteral("Postecipada - Parcela a partir de P: A = P * [(i * (1 + i)^n) / ((1 + i)^n - 1)], através do Fator de Recuperação de Capital (FRC)."),
                QStringLiteral("Antecipada: os valores equivalentes de P e F são multiplicados pelo fator de capitalização adicional (1 + i)."),
                QStringLiteral("Cálculo do Número de Períodos (n): determinação analítica por logaritmos do prazo necessário para amortizar um saldo ou formar uma reserva.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Para solucionar anuidades:")
                    },
                    {
                        QStringLiteral("1. Selecione a aba 'Anuidades'."),
                        QStringLiteral("2. No seletor superior, escolha o que calcular: 'Calcular Valor Presente (P)', 'Calcular Montante (F)', 'Calcular Parcela (A)' ou 'Calcular Períodos (n)'."),
                        QStringLiteral("3. No tipo de anuidade, marque 'Postecipada (Fim do Período)' ou 'Antecipada (Início do Período)'."),
                        QStringLiteral("4. Digite as variáveis conhecidas nos campos Valor Presente (P), Valor da Parcela (A), Taxa de Juros (% ao período) e Número de Períodos (n)."),
                        QStringLiteral("5. Clique em 'Calcular'."),
                        QStringLiteral("6. Inspecione o memorial de cálculo com o fator financeiro aplicado e a discriminação dos juros totais contratados.")
                    }
                }
            }
        },
        {
            QStringLiteral("series-em-gradiente"),
            QStringLiteral("5. Módulo 3: Séries em Gradiente (Aritmético e Geométrico)"),
            {
                QStringLiteral("O módulo de Séries em Gradiente destina-se à modelagem de fluxos de caixa não uniformes, onde os valores variam de maneira previsível ao longo do tempo."),
                QStringLiteral("Contempla o Gradiente Aritmético (acréscimo ou decréscimo constante de valor monetário G por período) e o Gradiente Geométrico (acréscimo ou decréscimo a uma taxa percentual constante g por período).")
            },
            {
                QStringLiteral("Gradiente Aritmético: cada parcela k é dada por Ak = A1 + (k - 1) * G."),
                QStringLiteral("Gradiente Aritmético - Valor Presente Total: P = P_base + P_G, onde P_G = (G / i) * [((1 + i)^n - 1)/(i * (1 + i)^n) - n / (1 + i)^n]."),
                QStringLiteral("Gradiente Aritmético - Parcela Equivalente Uniforme: A = A1 + G * [1 / i - n / ((1 + i)^n - 1)]."),
                QStringLiteral("Gradiente Geométrico: Ak = A1 * (1 + g)^(k - 1). Para i != g, P = (A1 / (i - g)) * [1 - ((1 + g)/(1 + i))^n]. Se i == g, P = n * A1 / (1 + i)."),
                QStringLiteral("Cálculo Pontual no Período k: permite encontrar instantaneamente o valor específico da parcela em qualquer ano ou mês desejado.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Para modelar fluxos com gradiente:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Gradientes'."),
                        QStringLiteral("2. No seletor 'Modo de Cálculo', escolha entre 'Gradiente Aritmético' e 'Gradiente Geométrico'."),
                        QStringLiteral("3. No objetivo de cálculo, selecione 'Valor Presente (P)', 'Parcela Equivalente (A)' ou 'Valor no Período k'."),
                        QStringLiteral("4. Preencha o valor da primeira parcela base (A1), a magnitude do gradiente (G em R$ ou g em %), a taxa de desconto (i %) e o total de períodos (n)."),
                        QStringLiteral("5. Caso queira o valor intermediário, preencha também o período k desejado."),
                        QStringLiteral("6. Clique em 'Calcular' para obter a demonstração passo a passo e o laudo financeiro.")
                    }
                }
            }
        },
        {
            QStringLiteral("conversao-equivalencia-taxas"),
            QStringLiteral("6. Módulo 4: Conversão e Equivalência de Taxas"),
            {
                QStringLiteral("A aba 'Conversão de Taxas' é estruturada em dois painéis independentes separados por um divisor ajustável (splitter): Equivalência no Regime Composto e Taxa Real vs. Taxa Aparente."),
                QStringLiteral("Essa organização permite resolver simultaneamente conversões temporais entre diferentes prazos de capitalização e expurgar os efeitos da inflação do período.")
            },
            {
                QStringLiteral("Equivalência de Taxas: converte taxas compostas entre prazos diários, mensais, trimestrais, semestrais e anuais pela relação (1 + i_destino) = (1 + i_origem)^(prazo_destino / prazo_origem)."),
                QStringLiteral("Bases Temporais Suportadas: Ano comercial (360 dias), Ano civil (365 dias), Ano útil (252 dias), Mês (30 dias), Trimestre (90 dias) e Semestre (180 dias)."),
                QStringLiteral("Taxa Real vs. Aparente: relação de Fisher estrita (1 + i_aparente) = (1 + i_real) * (1 + inflação)."),
                QStringLiteral("Cálculo da Taxa Real: i_real = ((1 + i_aparente) / (1 + inflação)) - 1, revelando o ganho efetivo de poder de compra."),
                QStringLiteral("Cálculo da Taxa Aparente: i_aparente = (1 + i_real) * (1 + inflação) - 1, para fixação de taxa contratual indexada.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Utilizar"),
                    {
                        QStringLiteral("Para realizar as conversões:")
                    },
                    {
                        QStringLiteral("1. No painel 'Equivalência de Taxas', informe a Taxa de Juros Atual (% ao período)."),
                        QStringLiteral("2. Digite o número de dias ou a base do período atual (ex: 30 para mensal) e do período desejado (ex: 360 para anual)."),
                        QStringLiteral("3. Clique em 'Calcular Equivalência' para visualizar a formulação com expoentes fracionários e a taxa equivalente correspondente."),
                        QStringLiteral("4. No painel 'Taxa Real / Aparente', selecione no menu se deseja 'Calcular Taxa Real' ou 'Calcular Taxa Aparente'."),
                        QStringLiteral("5. Digite a taxa nominal aparente e a taxa de inflação projetada/observada (ambas em %)."),
                        QStringLiteral("6. Clique em 'Calcular' para obter a demonstração com decomposição matemática completa.")
                    }
                }
            }
        },
        {
            QStringLiteral("sistemas-amortizacao"),
            QStringLiteral("7. Módulo 5: Sistemas de Amortização de Dívidas"),
            {
                QStringLiteral("O módulo de Amortização constitui um dos motores mais avançados do Economia_APP, permitindo planejar e simular cronogramas completos de financiamentos e empréstimos bancários."),
                QStringLiteral("Integra os cinco principais sistemas de amortização utilizados no mercado financeiro nacional e internacional, gerando uma planilha analítica com totalizadores e opção de carência.")
            },
            {
                QStringLiteral("SAC (Sistema de Amortização Constante): amortização invariável A = P / n. As prestações e os juros são estritamente decrescentes ao longo do contrato."),
                QStringLiteral("PRICE (Sistema Francês): prestações periódicas uniformes P = P_0 * [i*(1+i)^n / ((1+i)^n - 1)]. A amortização é crescente e os juros são decrescentes."),
                QStringLiteral("SAM (Sistema de Amortização Misto): cada prestação e amortização corresponde à média aritmética dos sistemas SAC e Price no período correspondente."),
                QStringLiteral("Sistema Americano: o devedor efetua o pagamento periódico exclusivamente dos juros contratuais, liquidando a totalidade do principal no último período (n)."),
                QStringLiteral("Sistema Hamburguês: juros antecipados cobrados no início de cada período sobre o saldo residual com amortização periódica constante."),
                QStringLiteral("Carência e Juros Capitalizados: suporte a períodos de carência sem amortização, com opção de capitalizar juros ao saldo devedor ou pagá-los periodicamente."),
                QStringLiteral("Consulta do Período k: obtenção direta da prestação, juros, amortização e saldo devedor de uma parcela intermediária sem necessidade de buscar na tabela."),
                QStringLiteral("Tabelas Dinâmicas em Cartões no Histórico: cada cálculo gera sua própria tabela completa anexada diretamente ao cartão de resultado, permitindo comparar múltiplos financiamentos simultâneos sem sobreposição ou widgets compactados residuais."),
                QStringLiteral("Exportação da Planilha: botão dedicado e atalho no menu Arquivos para gerar o cronograma completo em documento PDF profissional bilíngue (amortizacao.pdf ou amortization.pdf).")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Gerar o Cronograma"),
                    {
                        QStringLiteral("Para simular um plano de amortização:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Amortização'."),
                        QStringLiteral("2. No primeiro menu suspenso, selecione o sistema: 'SAC', 'PRICE', 'SAM', 'Americano' ou 'Hamburguês'."),
                        QStringLiteral("3. Digite o Valor Financiado / Principal (P), a Taxa de Juros (% ao período) e o Número Total de Parcelas (n)."),
                        QStringLiteral("4. Se houver carência, informe a quantidade de períodos no campo 'Carência' e marque 'Capitalizar juros na carência?' caso os juros acumulados devam ser incorporados à dívida."),
                        QStringLiteral("5. Caso deseje inspecionar uma parcela específica, informe o índice no campo 'Período k'."),
                        QStringLiteral("6. Clique em 'Calcular Amortização'."),
                        QStringLiteral("7. O sistema gera um novo cartão no histórico com o resumo da simulação e a tabela completa embutida (Período, Prestação, Juros, Amortização e Saldo Devedor, mais totais)."),
                        QStringLiteral("8. Para navegar entre cálculos de diferentes sistemas ou prazos, use a barra de rolagem vertical à direita."),
                        QStringLiteral("9. Para exportar o cronograma em PDF com layout profissional, clique em 'Exportar PDF' ou use Arquivos → Exportar Atual.")
                    }
                }
            }
        },
        {
            QStringLiteral("analise-investimentos"),
            QStringLiteral("8. Módulo 6: Análise de Viabilidade Econômica de Investimentos"),
            {
                QStringLiteral("Este módulo implementa os critérios fundamentais de avaliação econômica de projetos industriais, comerciais e de infraestrutura."),
                QStringLiteral("Permite mensurar a criação de valor, a taxa de retorno intrínseca e o tempo de recuperação do capital, emitindo pareceres técnicos imediatos de aceitação ou rejeição.")
            },
            {
                QStringLiteral("Valor Presente Líquido (VPL / NPV): VPL = soma(FC_t / (1 + TMA)^t) - I_0. Se VPL > 0, o investimento é viável economicamente."),
                QStringLiteral("Valor Anual Uniforme Equivalente (VAUE / EAC): converte o VPL em uma anuidade uniforme equivalente ao longo da vida útil do projeto."),
                QStringLiteral("Taxa Interna de Retorno (TIR / IRR): taxa de desconto para a qual o VPL se iguala a zero. Calculada por método iterativo numérico de Newton-Raphson com alta tolerância."),
                QStringLiteral("Payback Simples: prazo exato em anos/meses para recuperação nominal do investimento inicial sem custo de oportunidade."),
                QStringLiteral("Payback Descontado: prazo exato para recuperação do investimento considerando os fluxos descontados à TMA do projeto."),
                QStringLiteral("Análise de Sensibilidade: simula o impacto de variações percentuais (+- Delta %) nas receitas ou custos operacionais sobre a viabilidade final do projeto.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Avaliar Projetos"),
                    {
                        QStringLiteral("Para realizar a avaliação de viabilidade:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Análise de Investimentos'."),
                        QStringLiteral("2. Escolha o tipo de análise no menu suspenso: 'VPL e VAUE Uniforme', 'VPL Detalhado (Fluxo de Caixa)', 'Payback Simples e Descontado', 'Análise de Sensibilidade' ou 'Taxa Interna de Retorno (TIR)'."),
                        QStringLiteral("3. Preencha o Investimento Inicial (I0), os fluxos de caixa líquidos periódicos (ou receitas e despesas anuais), a Taxa Mínima de Atratividade (TMA %) e a vida útil do projeto (n)."),
                        QStringLiteral("4. Se estiver realizando a Análise de Sensibilidade, informe a variação percentual pretendida (+- %)."),
                        QStringLiteral("5. Clique em 'Calcular Análise'."),
                        QStringLiteral("6. Leia o parecer detalhado gerado no histórico, contendo o veredito formal de viabilidade econômica e todos os indicadores associados.")
                    }
                }
            }
        },
        {
            QStringLiteral("metodos-depreciacao"),
            QStringLiteral("9. Módulo 7: Métodos de Depreciação de Ativos"),
            {
                QStringLiteral("O módulo de Depreciação possibilita apurar a perda sistemática de valor contábil de bens tangíveis do ativo imobilizado (máquinas, veículos, edifícios e equipamentos) ao longo de sua vida útil operacional."),
                QStringLiteral("Oferece suporte aos três principais métodos reconhecidos pela contabilidade societária e fiscal internacional.")
            },
            {
                QStringLiteral("Método Linear (Quotas Constantes): D_k = (C_0 - V_r) / n. A depreciação é distribuída uniformemente por todos os anos da vida útil."),
                QStringLiteral("Soma dos Dígitos dos Anos (SDA / Cole): método de depreciação acelerada decrescente. A quota anual D_k = (C_0 - V_r) * (n - k + 1) / S, onde S = n * (n + 1) / 2."),
                QStringLiteral("Saldo Declinante (Percentual Fixo): aplica uma taxa percentual fixa d = 1 - (V_r / C_0)^(1/n) sobre o saldo contábil residual do período anterior."),
                QStringLiteral("Valor Residual e Depreciável: calcula explicitamente a Base Depreciável = C_0 - V_r e o Saldo Contábil ao término de cada período.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Para calcular a depreciação de um bem:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Depreciação'."),
                        QStringLiteral("2. No seletor 'Método de Depreciação', selecione 'Linear', 'Soma dos Dígitos' ou 'Saldo Declinante'."),
                        QStringLiteral("3. Digite o Custo Inicial de Aquisição (P ou C0), o Valor Residual estimado no fim da vida (Vr), a Vida Útil em anos (n) e o ano específico de consulta (k)."),
                        QStringLiteral("4. Clique no botão 'Calcular Depreciação'."),
                        QStringLiteral("5. O contêiner de histórico apresentará a quota de depreciação do ano k, a depreciação acumulada até aquele instante e o valor contábil residual do equipamento.")
                    }
                }
            }
        },
        {
            QStringLiteral("taxa-efetiva-tir-global"),
            QStringLiteral("10. Módulo 8: Taxa Efetiva, TIR Avançada e Custo de Capital"),
            {
                QStringLiteral("Este módulo reúne ferramentas analíticas avançadas para refinamento de custos de capital, taxas compostas multinível e avaliação de distorções em produtos financeiros bancários."),
                QStringLiteral("Resolve desde a conversão de taxas nominais até a determinação da Taxa Interna de Retorno Modificada (TIRM).")
            },
            {
                QStringLiteral("Taxa Efetiva Periódica e Anual: a partir de taxa nominal com capitalização diária, mensal, trimestral ou semestral, obtém a taxa efetiva real: i_ef = (1 + i_nom / m)^m - 1."),
                QStringLiteral("Taxa com Juros Antecipados: apura o Custo Efetivo Total (CET) em operações de desconto e antecipação de recebíveis onde os juros são retidos na liberação do recurso: i_ef = i / (1 - i)."),
                QStringLiteral("Taxa Real Global com Múltiplas Inflações: calcula o rendimento real final de investimentos plurianuais submetidos a taxas sucessivas e variáveis de inflação (m1, m2, m3)."),
                QStringLiteral("TMA Analítica: decompõe a Taxa Mínima de Atratividade na soma ponderada da Taxa Livre de Risco, Prêmio de Risco do Empreendimento e Custo de Oportunidade."),
                QStringLiteral("TIR por Newton-Raphson: resolução numérica de alta velocidade com convergência garantida para séries temporais extensas."),
                QStringLiteral("TIRM (Taxa Interna de Retorno Modificada): resolve a inconsistência de taxas múltiplas da TIR padrão, assumindo taxa realista de reinvestimento para as entradas de caixa e taxa de financiamento para as saídas.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Operar"),
                    {
                        QStringLiteral("Para utilizar as análises avançadas de taxa:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Taxa Efetiva / TIR / Taxa Global'."),
                        QStringLiteral("2. No menu suspenso superior, escolha a ferramenta pretendida."),
                        QStringLiteral("3. Observe que o formulário ajusta dinamicamente os campos pertinentes ao modo escolhido."),
                        QStringLiteral("4. Preencha os parâmetros financeiros requeridos."),
                        QStringLiteral("5. Clique em 'Calcular Taxa / TIR'."),
                        QStringLiteral("6. Analise a memória de cálculo gerada com equações intermediárias e formulações matemáticas explícitas.")
                    }
                }
            }
        },
        {
            QStringLiteral("retorno-minimo-tma"),
            QStringLiteral("11. Módulo 9: Retorno Mínimo Exigido (TMA)"),
            {
                QStringLiteral("O módulo de Retorno Mínimo é direcionado à apuração direta do lucro monetário e do montante financeiro mínimo que um capital investido deve gerar para remunerar a Taxa Mínima de Atratividade do investidor."),
                QStringLiteral("Serve como benchmark preliminar para aprovação de propostas de novos negócios e alocação de ativos.")
            },
            {
                QStringLiteral("Lucro Mínimo Exigido: Lucro = P * [(1 + TMA)^n - 1]."),
                QStringLiteral("Montante Mínimo Futuro: F = P * (1 + TMA)^n."),
                QStringLiteral("Custo de Oportunidade: mensuração do rendimento que o capital auferiria caso estivesse aplicado na melhor alternativa de risco equivalente.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Procedimento de execução:")
                    },
                    {
                        QStringLiteral("1. Selecione a aba 'Retorno Mínimo (TMA)'."),
                        QStringLiteral("2. Insira o Capital Investido (P), a TMA estabelecida (% ao período) e o Número de Períodos (n)."),
                        QStringLiteral("3. Clique em 'Calcular Retorno Mínimo'."),
                        QStringLiteral("4. Obtenha a discriminação do montante acumulado e o lucro mínimo indispensável para viabilizar a alocação.")
                    }
                }
            }
        },
        {
            QStringLiteral("equacao-de-fisher"),
            QStringLiteral("12. Módulo 10: Equação de Fisher e Inflação"),
            {
                QStringLiteral("Implementa a célebre formulação de Irving Fisher que relaciona rigorosamente a taxa de juros nominal (aparente), a taxa de juros real e a taxa de inflação."),
                QStringLiteral("Essencial para desmistificar rentabilidades nominais elevadas em ambientes econômicos inflacionários.")
            },
            {
                QStringLiteral("Relação Fundamental: (1 + i_nominal) = (1 + i_real) * (1 + inflação)."),
                QStringLiteral("Cálculo da Taxa Nominal: i_nominal = (1 + i_real) * (1 + inflação) - 1, indicando quanto o contrato deve pagar para garantir ganho real i_real."),
                QStringLiteral("Cálculo da Taxa Real: i_real = ((1 + i_nominal) / (1 + inflação)) - 1, indicando o ganho patrimonial real acima da inflação.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Calcular"),
                    {
                        QStringLiteral("Para utilizar a Equação de Fisher:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'Equação de Fisher'."),
                        QStringLiteral("2. Escolha no seletor se deseja 'Calcular Taxa Nominal' ou 'Calcular Taxa Real'."),
                        QStringLiteral("3. Insira as variáveis conhecidas nos campos habilitados."),
                        QStringLiteral("4. Clique no botão 'Calcular Fisher'."),
                        QStringLiteral("5. Observe o memorial de cálculo contendo a dedução dos termos cruzados e o resultado final devidamente realçado.")
                    }
                }
            }
        },
        {
            QStringLiteral("vpl-com-tributos"),
            QStringLiteral("13. Módulo 11: VPL com Tributação e Benefício Fiscal"),
            {
                QStringLiteral("Na prática empresarial corporativa, análises de viabilidade que ignoram a incidência de impostos sobre o lucro e os benefícios fiscais de despesas dedutíveis produzem conclusões distorcidas."),
                QStringLiteral("O módulo 'VPL com Tributos' incorpora a tributação sobre o Lucro Real (IRPJ e CSLL), o benefício fiscal gerado pela depreciação do ativo (Tax Shield), opções de financiamento com amortização e o imposto sobre ganho de capital na revenda.")
            },
            {
                QStringLiteral("Alíquotas Configuráveis: IRPJ (padrão 15% ou 25% com adicional) e CSLL (padrão 9%)."),
                QStringLiteral("Benefício Fiscal da Depreciação (Tax Shield): Economia = Quota_Depreciação * (IRPJ + CSLL), aumentando o fluxo de caixa líquido da empresa."),
                QStringLiteral("Modelagem com Financiamento: dedutibilidade tributária das parcelas de juros do empréstimo bancário."),
                QStringLiteral("Alienação do Ativo no Fim da Vida Útil: apuração do Ganho de Capital = Preço de Venda - Saldo Contábil Residual e incidência de imposto."),
                QStringLiteral("Fluxo de Caixa Livre após Tributos (FCL): projeção ano a ano e desconto de todos os fluxos à TMA da empresa para obter o VPL Real.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Simular Projetos Tributados"),
                    {
                        QStringLiteral("Para executar a modelagem econômico-fiscal:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'VPL com Tributos'."),
                        QStringLiteral("2. Informe o Investimento Inicial em bens de capital e o Lucro Operacional Anual Bruto projetado (antes do IR)."),
                        QStringLiteral("3. Defina a Vida Útil do ativo em anos, a alíquota de IRPJ (%) e a alíquota de CSLL (%)."),
                        QStringLiteral("4. Informe a Taxa Mínima de Atratividade (TMA %) utilizada pela diretoria financeira."),
                        QStringLiteral("5. Se a aquisição for financiada, marque 'Aquisição Financiada?' e preencha a taxa do financiamento e o prazo em anos."),
                        QStringLiteral("6. Se o equipamento for vendido ao final, preencha o Ano da Venda e o Valor de Mercado estimado de revenda."),
                        QStringLiteral("7. Clique em 'Calcular VPL com Tributos'."),
                        QStringLiteral("8. Analise a planilha detalhada com o Fluxo de Caixa Líquido ano a ano, o valor presente de cada período e o VPL líquido final com o parecer de viabilidade.")
                    }
                }
            }
        },
        {
            QStringLiteral("caue-vida-economica"),
            QStringLiteral("14. Módulo 12: CAUE - Custo Anual Uniforme Equivalente e Vida Econômica"),
            {
                QStringLiteral("O módulo CAUE é a ferramenta definitiva para a determinação da Vida Econômica Ótima de substituição de máquinas, frotas, equipamentos industriais e ativos operacionais."),
                QStringLiteral("Resolve o clássico dilema de engenharia econômica entre manter um equipamento antigo (com custos crescentes de manutenção e quebras) ou substituí-lo por um novo (com elevado custo de recuperação de capital).")
            },
            {
                QStringLiteral("Custo de Recuperação de Capital (CRC): CRC_k = [P - VR_k * (1 + i)^(-k)] * [i * (1 + i)^k / ((1 + i)^k - 1)], onde P é o custo de aquisição e VR_k é o valor de revenda no ano k."),
                QStringLiteral("Custo Operacional Anualizado (COA): valor presente dos custos de operação acumulados até o ano k, transformados em série uniforme pela anuidade equivalente."),
                QStringLiteral("Custo Anual Uniforme Equivalente Total: CAUE_k = CRC_k + COA_k."),
                QStringLiteral("Determinação do Ano Ótimo k*: o sistema identifica analiticamente o ano k que minimiza a curva do CAUE total, definindo a vida econômica ideal de substituição do ativo."),
                QStringLiteral("Tabela Interativa de Entradas: grade dinâmica de dados que permite ao usuário informar o valor de revenda e o custo operacional previstos para cada ano.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Otimizar a Substituição"),
                    {
                        QStringLiteral("Siga o roteiro passo a passo:")
                    },
                    {
                        QStringLiteral("1. Acesse a aba 'CAUE - Vida Econômica'."),
                        QStringLiteral("2. Informe o Custo Inicial de Aquisição do Ativo (P), a TMA (% ao ano) e o Horizonte Máximo de Análise em anos (N)."),
                        QStringLiteral("3. Clique no botão 'Gerar Tabela de Entradas' para criar a grade de custos ano a ano."),
                        QStringLiteral("4. Na tabela de entradas gerada, preencha para cada linha (ano k) o 'Valor de Revenda' estimado e o 'Custo Operacional' previsto."),
                        QStringLiteral("5. Clique no botão 'Calcular CAUE'."),
                        QStringLiteral("6. O sistema preenche a tabela de resultados com CRC, COA e CAUE Total para cada ano."),
                        QStringLiteral("7. Leia o laudo final emitido pelo software, que destaca explicitamente o Ano de Substituição Recomendado e o CAUE Mínimo atingido.")
                    }
                }
            }
        },
        {
            QStringLiteral("historico-calculos"),
            QStringLiteral("15. Painel de Histórico (HistoryContainer) e Manipulação"),
            {
                QStringLiteral("Cada aba de cálculo do Economia_APP conta com um contêiner de histórico inteligente (HistoryContainer), projetado para assegurar máxima rastreabilidade, edição e preservação da memória de cálculo durante toda a sessão de trabalho.")
            },
            {
                QStringLiteral("Quadros de Resultados Independentes: cada cálculo adiciona um novo cartão completo com suas dimensões originais preservadas, eliminando esmagamento ou compressão entre saídas sucessivas."),
                QStringLiteral("Barra de Rolagem Vertical: navegação fluida por barra de rolagem vertical para percorrer confortavelmente todo o histórico acumulado na aba."),
                QStringLiteral("Seleção por Checkbox: cada cartão possui sua própria caixa de seleção para focar operações de edição, exclusão ou exportação pontual."),
                QStringLiteral("Barra de Busca Integrada: campo de pesquisa com realce visual em cores contrastantes e navegação por ocorrências nos registros de cálculo."),
                QStringLiteral("Edição Inline: clicando em 'Editar Cálculo', o bloco selecionado é liberado para edição direta de texto e anotações, confirmando as alterações pelo mesmo botão."),
                QStringLiteral("Exclusão Pontual: remoção limpa do bloco de cálculo selecionado através do botão 'Excluir Seleção'."),
                QStringLiteral("Limpeza Estruturada: botões 'Limpar Entrada' (reseta formulário), 'Limpar Saída' (limpa histórico da aba) e 'Limpar Tudo' (reseta ambos simultaneamente)."),
                QStringLiteral("Limpeza Global de Sessão: através de Arquivos → Limpar Sessão Salva, confirmação com 'Sim'/'Não' para purgar simultaneamente todas as 12 abas e o arquivo do disco."),
                QStringLiteral("Exportação Rápida: comandos diretos para exportação em arquivo de texto formatado (.txt) ou documento PDF diagramado (.pdf)."),
                QStringLiteral("Persistência em Sessão: o histórico não é perdido ao navegar entre as diferentes abas da aplicação e pode ser restaurado na inicialização.")
            },
            {
                {
                    QStringLiteral("Dicas de Utilização do Histórico"),
                    {
                        QStringLiteral("Aproveite ao máximo os recursos do painel retrátil:")
                    },
                    {
                        QStringLiteral("Navegue confortavelmente pela barra de rolagem vertical ou pela roda do mouse para comparar cálculos anteriores e recentes."),
                        QStringLiteral("Marque a caixa de seleção do cartão desejado para realizar edição pontual de texto ou exclusão específica."),
                        QStringLiteral("Ao mudar de idioma no menu Configuração, todos os cartões já existentes no histórico são retraduzidos dinamicamente mantendo valores e fórmulas intactos.")
                    }
                }
            }
        },
        {
            QStringLiteral("configuracao-fontes-exportacao-pdf"),
            QStringLiteral("16. Configuração de Fontes e Exportação para PDF"),
            {
                QStringLiteral("O Economia_APP foi projetado para produzir relatórios executivos prontos para impressão, anexação a laudos periciais ou apresentação acadêmica."),
                QStringLiteral("O motor de exportação em PDF utiliza os subsistemas nativos do Qt6 (QPrinter e QPainter) para garantir diagramação limpa, alinhamento rigoroso e vetorização nítida de elementos tipográficos e matemáticos.")
            },
            {
                QStringLiteral("Exportação da Aba Ativa: acionada pelo menu Arquivos → Exportar Atual ou pelo botão 'Exportar PDF' da própria aba, gerando automaticamente nomes e cabeçalhos em português (ex.: amortizacao.pdf) ou inglês (ex.: amortization.pdf) conforme o idioma ativo."),
                QStringLiteral("Exportação Consolidada: acionada pelo menu Arquivos → Exportar Todos, gerando um documento unificado com todas as seções e tabelas calculadas (todos_os_calculos.pdf ou all_calculations.pdf)."),
                QStringLiteral("Paginação Automática: cabeçalhos institucionais, numeração de páginas (Página X de Y), data e hora de emissão e controle inteligente de quebra de linhas e tabelas extensas."),
                QStringLiteral("Renderizador MathRenderer: renderização estética avançada de fórmulas com radiciação, expoentes sobrescritos, índices subscritos e frações."),
                QStringLiteral("Personalização Tipográfica: diálogo de fontes que propaga em tempo real o estilo e tamanho selecionados tanto para a tela quanto para o motor de impressão.")
            },
            {
                {
                    QStringLiteral("Passo a Passo: Como Gerar um Relatório em PDF"),
                    {
                        QStringLiteral("Procedimento para exportar:")
                    },
                    {
                        QStringLiteral("1. Realize os cálculos desejados na aba correspondente (ou em várias abas caso pretenda exportar o relatório geral)."),
                        QStringLiteral("2. Clique no botão 'Exportar PDF' da aba ou acesse o menu Arquivos → Exportar Atual / Exportar Todos."),
                        QStringLiteral("3. Na janela de diálogo do sistema operacional, o nome sugerido já virá no idioma ativo (ex.: amortizacao.pdf ou amortization.pdf). Escolha a pasta e clique em 'Salvar'."),
                        QStringLiteral("4. O aplicativo gerará o documento PDF diagramado com alta fidelidade visual e tabelas perfeitamente alinhadas.")
                    }
                }
            }
        },
        {
            QStringLiteral("atalhos-e-dicas"),
            QStringLiteral("17. Atalhos de Teclado, Boas Práticas e Solução de Problemas"),
            {
                QStringLiteral("Para assegurar a máxima produtividade e evitar erros operacionais, siga as recomendações práticas e familiarize-se com os atalhos de teclado do sistema.")
            },
            {
                QStringLiteral("Ctrl+Shift+A: Abre a janela Sobre (About) a partir de qualquer tela ou aba do aplicativo, com suporte a busca rápida de termos em todas as abas e botões de maximizar/minimizar."),
                QStringLiteral("Ctrl+Shift+M: Abre o Manual de Utilização a partir de qualquer tela ou aba do aplicativo."),
                QStringLiteral("Tecla Enter: Dispara automaticamente o botão de cálculo da aba ativa quando o foco está em qualquer campo de entrada."),
                QStringLiteral("Tecla Tab / Shift+Tab: Navega sequencialmente para a frente ou para trás entre os campos de digitação do formulário."),
                QStringLiteral("Formatação Numérica: Utilize ponto (.) ou vírgula (,) como separador decimal de acordo com o padrão regional configurado."),
                QStringLiteral("Consistência de Taxas e Períodos: Certifique-se sempre de que a unidade temporal da taxa de juros coincida com a unidade do número de períodos (ex: taxa mensal com períodos em meses; taxa anual com períodos em anos). Se necessário, utilize previamente a aba 'Conversão de Taxas'."),
                QStringLiteral("Armazenamento e Integridade: As configurações do usuário e o controle de avaliação (Trial) são salvos de forma segura no diretório AppData do Windows (%APPDATA%/Economia_APP)."),
                QStringLiteral("Auditoria e Logs: O sistema gera registros contínuos de operação através do LogManager, facilitando diagnósticos rápidos em caso de comportamentos anômalos.")
            },
            {
                {
                    QStringLiteral("Solução de Problemas Comuns"),
                    {
                        QStringLiteral("Orientações para resolução rápida:")
                    },
                    {
                        QStringLiteral("Resultado de cálculo não aparece: Verifique se todos os campos obrigatórios foram preenchidos com valores numéricos válidos. Campos vazios ou com caracteres alfabéticos impedem o cálculo."),
                        QStringLiteral("TIR retorna erro de convergência: Fluxos de caixa atípicos com múltiplas mudanças de sinal podem não possuir raiz real única. Tente utilizar a aba 'Taxa Efetiva / TIR / Taxa Global' no modo TIRM (TIR Modificada)."),
                        QStringLiteral("Aviso de Período de Avaliação (Trial): Caso o período de avaliação expire, entre em contato com o autor Fernando Nillsson Cidade para renovação da licença de uso.")
                    }
                }
            }
        }
    };
}

} // namespace source::ui::manual_pt_BR
