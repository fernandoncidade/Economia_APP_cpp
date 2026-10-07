#include "ui_32_manual_en_US.hpp"

namespace source::ui::manual_en_US {

using source::ui::Manual::ManualDetails;
using source::ui::Manual::ManualSection;

QString manual_intro_text()
{
    return QStringLiteral("This manual details the operation, mathematical algorithms, software architecture, and complete step-by-step workflow for all features of the Financial and Engineering Economics Calculator (Economia_APP).");
}

QString manual_toc_title()
{
    return QStringLiteral("Table of Contents");
}

QList<ManualSection> get_manual_document()
{
    return {
        {
            QStringLiteral("visao-geral"),
            QStringLiteral("1. Application Overview & Architecture"),
            {
                QStringLiteral("Economia_APP is a high-precision desktop calculation suite built with C++17 and Qt 6, specifically tailored for engineering students, industrial economists, financial analysts, project managers, and forensic accountants."),
                QStringLiteral("The platform automates complex engineering economy routines, replacing error-prone spreadsheets with verified deterministic algorithms, clear step-by-step mathematical derivations, rendered typographic formulas, rich interactive amortization tables, and professional PDF reports."),
                QStringLiteral("The main window features a tab-based navigation layout coupled with a retractable History Container (HistoryContainer) integrated on the output side of each tab, delivering ergonomic data entry, full calculation history tracking, and high-fidelity PDF publishing.")
            },
            {
                QStringLiteral("12 Specialized Calculation Tabs encompassing classical financial mathematics and advanced engineering economy."),
                QStringLiteral("Step-by-Step Mathematical Derivations presenting explicit formula substitutions, intermediate arithmetic, and final answers."),
                QStringLiteral("Integrated History Container (HistoryContainer) with real-time text search, inline record editing, selective record deletion, and quick export to TXT or PDF."),
                QStringLiteral("Executive PDF Publishing Engine using Qt6 PrintSupport with institutional headers, page numbers, and vector table formatting."),
                QStringLiteral("Typographic Math Rendering Engine supporting radical expressions (square and nth roots), superscripts, subscripts, and clean fractions."),
                QStringLiteral("Dynamic Bilingual Localization enabling instant runtime switching between Brazilian Portuguese and US English without restarting."),
                QStringLiteral("Live Font and Scaling Customization with typeface family selection and readable point size adjustment."),
                QStringLiteral("30-Day Evaluation Trial System with cryptographically protected registration stored securely under AppData.")
            },
            {
                {
                    QStringLiteral("Architecture & Data Flow"),
                    {
                        QStringLiteral("Each tab layout is partitioned into an input form on the left/top with strict numeric validation (QDoubleValidator) and an output history dock on the right/bottom."),
                        QStringLiteral("The computation engine dispatches routines via dedicated service units (namespace source::services), sending results to the text formatter (TextFormat) and math renderer (MathRenderer).")
                    },
                    {
                        QStringLiteral("Clean separation between business logic and UI presentation layers."),
                        QStringLiteral("Preventative UI freezing safeguards during iterative operations (IRR, Newton-Raphson, multi-period amortization schedules, and CAUE optimization)."),
                        QStringLiteral("Cumulative session history preserved across tab switches.")
                    }
                }
            }
        },
        {
            QStringLiteral("barra-menus-configuracoes"),
            QStringLiteral("2. Menu Bar & Global Configuration"),
            {
                QStringLiteral("The main menu bar gathers key file management, user preference, and system documentation commands."),
                QStringLiteral("All functions can be triggered via mouse interaction or standard keyboard shortcuts.")
            },
            {
                QStringLiteral("File → Export Current: exports the calculations from the active tab into a formatted PDF document."),
                QStringLiteral("File → Export All: automatically compiles a comprehensive multi-page executive PDF report consolidating all computed tabs and active tables."),
                QStringLiteral("Configuration → Languages: submenu providing 'Português (Brasil)' and 'English (United States)'. Selecting a language instantly re-translates all labels, buttons, menus, and open dialogs."),
                QStringLiteral("Configuration → Font Configuration: opens FontConfigDialog to customize interface font family (e.g., Segoe UI, Roboto, Consolas) and point size."),
                QStringLiteral("Options → About (Ctrl+Shift+A): displays the institutional dialog with minimize/maximize window controls, real-time search across all tabs with occurrence counter, Project History, Technical Details, Third-Party Licenses, Legal Notices, Privacy Policy, and Release Notes."),
                QStringLiteral("Options → Manual (Ctrl+Shift+M): opens this interactive User Manual with bilingual support, structured table of contents, and real-time text search.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: Font Configuration"),
                    {
                        QStringLiteral("To optimize typography for high-DPI displays (4K) or personal readability preferences:")
                    },
                    {
                        QStringLiteral("1. Open the menu Configuration → Font Configuration."),
                        QStringLiteral("2. In the 'Font Family' dropdown, select your preferred typeface."),
                        QStringLiteral("3. In the 'Font Size' spinbox, select the desired point size (e.g., 9, 10, 11, or 12 pt)."),
                        QStringLiteral("4. Review changes immediately in the live preview box."),
                        QStringLiteral("5. Click 'OK' to apply the font configuration across all UI components."),
                        QStringLiteral("6. To revert to system default fonts, click 'Restore'.")
                    }
                },
                {
                    QStringLiteral("Step-by-Step: Changing Interface Language"),
                    {
                        QStringLiteral("The software utilizes a dynamic translation manager (GerenciadorTraducao):")
                    },
                    {
                        QStringLiteral("1. Open the menu Configuration → Languages."),
                        QStringLiteral("2. Choose 'Português (Brasil)' or 'English (United States)'."),
                        QStringLiteral("3. The entire interface, including tables and auxiliary dialogs, updates instantly while preserving all current user entries.")
                    }
                }
            }
        },
        {
            QStringLiteral("juros-simples-compostos"),
            QStringLiteral("3. Module 1: Simple and Compound Interest"),
            {
                QStringLiteral("The Simple and Compound Interest module calculates the time value of money under linear interest capitalization (simple interest) and exponential capitalization (compound interest)."),
                QStringLiteral("It computes Future Value (F), Present Value / Principal (P), or performs a direct side-by-side comparative analysis between both regimes.")
            },
            {
                QStringLiteral("Simple Interest Formulation: F = P * (1 + i * n) and Total Interest J = P * i * n."),
                QStringLiteral("Compound Interest Formulation: F = P * (1 + i)^n and Principal P = F / (1 + i)^n."),
                QStringLiteral("Compare JS vs JC Mode: provides a simultaneous evaluation highlighting exponential growth and patrimonial divergence over time."),
                QStringLiteral("Interest Rate (i) expressed as a percentage per period corresponding to the compounding interval of n."),
                QStringLiteral("Automated input validation preventing negative or invalid values.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("Follow these steps to run the interest computation:")
                    },
                    {
                        QStringLiteral("1. Select the 'Simple and Compound Interest' tab."),
                        QStringLiteral("2. In the first dropdown, select your target: 'Calculate Future Value (F)', 'Calculate Principal (P)', or 'Compare JS vs JC'."),
                        QStringLiteral("3. In the second dropdown, select the interest regime ('Compound Interest' or 'Simple Interest')."),
                        QStringLiteral("4. Enter the known parameters: Principal Value (P), Future Value (F), Interest Rate (% per period), and Number of Periods (n)."),
                        QStringLiteral("5. Click 'Calculate' or press Enter in any input field."),
                        QStringLiteral("6. Review the detailed step-by-step breakdown displayed in the History Container."),
                        QStringLiteral("7. Use the bottom action buttons to Clear Inputs, Clear Output, Edit Calculation, or Export PDF.")
                    }
                }
            }
        },
        {
            QStringLiteral("anuidades-series-uniformes"),
            QStringLiteral("4. Module 2: Annuities & Uniform Payment Series"),
            {
                QStringLiteral("This module solves uniform series of constant periodic payments or deposits (A), applying standard financial engineering equivalence factors."),
                QStringLiteral("Supports ordinary annuities (postpaid payments due at the end of each period) and annuities due (prepaid payments due at the beginning of each period, such as leases).")
            },
            {
                QStringLiteral("Ordinary Annuity - Present Value: P = A * [((1 + i)^n - 1) / (i * (1 + i)^n)], via the Present Value Factor (PVF)."),
                QStringLiteral("Ordinary Annuity - Future Value: F = A * [((1 + i)^n - 1) / i], via the Capital Accumulation Factor (CAF)."),
                QStringLiteral("Ordinary Annuity - Payment from P: A = P * [(i * (1 + i)^n) / ((1 + i)^n - 1)], via the Capital Recovery Factor (CRF)."),
                QStringLiteral("Annuity Due: all equivalence factors are multiplied by (1 + i) to account for early cash flow timing."),
                QStringLiteral("Number of Periods (n) Computation: analytical logarithmic solving of the required timeframe to amortize a balance or accumulate a fund.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("To solve annuity streams:")
                    },
                    {
                        QStringLiteral("1. Select the 'Annuities' tab."),
                        QStringLiteral("2. In the top dropdown, pick the desired objective: 'Calculate Present Value (P)', 'Calculate Future Value (F)', 'Calculate Payment (A)', or 'Calculate Periods (n)'."),
                        QStringLiteral("3. In the annuity type selector, choose 'Ordinary / End of Period' or 'Annuity Due / Start of Period'."),
                        QStringLiteral("4. Fill in the available variables: Present Value (P), Payment Amount (A), Interest Rate (% per period), and Periods (n)."),
                        QStringLiteral("5. Click 'Calculate'."),
                        QStringLiteral("6. Review the output including intermediate financial factors and total interest paid or earned.")
                    }
                }
            }
        },
        {
            QStringLiteral("series-em-gradiente"),
            QStringLiteral("5. Module 3: Gradient Series (Arithmetic & Geometric)"),
            {
                QStringLiteral("The Gradient Series module models non-uniform cash flows where amounts change systematically over time."),
                QStringLiteral("It supports Arithmetic Gradients (fixed monetary increment or decrement G per period) and Geometric Gradients (fixed percentage variation rate g per period).")
            },
            {
                QStringLiteral("Arithmetic Gradient: each payment k is given by Ak = A1 + (k - 1) * G."),
                QStringLiteral("Arithmetic Gradient - Total Present Value: P = P_base + P_G, where P_G = (G / i) * [((1 + i)^n - 1)/(i * (1 + i)^n) - n / (1 + i)^n]."),
                QStringLiteral("Arithmetic Gradient - Uniform Equivalent Payment: A = A1 + G * [1 / i - n / ((1 + i)^n - 1)]."),
                QStringLiteral("Geometric Gradient: Ak = A1 * (1 + g)^(k - 1). For i != g, P = (A1 / (i - g)) * [1 - ((1 + g)/(1 + i))^n]. If i == g, P = n * A1 / (1 + i)."),
                QStringLiteral("Specific Value at Period k: computes the exact cash flow amount in any designated year or month.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("To model gradient cash flows:")
                    },
                    {
                        QStringLiteral("1. Go to the 'Gradients' tab."),
                        QStringLiteral("2. Select the gradient mode: 'Arithmetic Gradient' or 'Geometric Gradient'."),
                        QStringLiteral("3. Choose your objective: 'Present Value (P)', 'Equivalent Payment (A)', or 'Value at Period k'."),
                        QStringLiteral("4. Enter base payment (A1), gradient magnitude (G in currency or g in %), discount rate (i %), and periods (n)."),
                        QStringLiteral("5. To inspect an intermediate point, input the target period k."),
                        QStringLiteral("6. Click 'Calculate' to generate complete formula steps and the financial summary.")
                    }
                }
            }
        },
        {
            QStringLiteral("conversao-equivalencia-taxas"),
            QStringLiteral("6. Module 4: Rate Equivalence & Inflation Conversion"),
            {
                QStringLiteral("The Rate Conversion tab provides two independent panels arranged across an adjustable splitter: Compound Rate Equivalence and Real vs. Apparent (Nominal) Rates."),
                QStringLiteral("This dual setup allows simultaneous temporal conversions across compounding bases and adjustment for inflationary erosion.")
            },
            {
                QStringLiteral("Compound Rate Equivalence: converts rates between daily, monthly, quarterly, semi-annual, and annual terms: (1 + i_target) = (1 + i_source)^(period_target / period_source)."),
                QStringLiteral("Standard Time Bases: Commercial year (360 days), Civil year (365 days), Working year (252 days), Month (30 days), Quarter (90 days), and Semester (180 days)."),
                QStringLiteral("Real vs. Apparent Rate: Irving Fisher relationship (1 + i_apparent) = (1 + i_real) * (1 + inflation)."),
                QStringLiteral("Real Rate Solving: i_real = ((1 + i_apparent) / (1 + inflation)) - 1, isolating true purchasing power gain."),
                QStringLiteral("Apparent Rate Solving: i_apparent = (1 + i_real) * (1 + inflation) - 1, determining the required contract rate under anticipated inflation.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Use"),
                    {
                        QStringLiteral("To convert interest rates:")
                    },
                    {
                        QStringLiteral("1. In the 'Rate Equivalence' panel, enter Current Interest Rate (% per period)."),
                        QStringLiteral("2. Enter the current period length in days (e.g., 30 for monthly) and the target period in days (e.g., 360 for annual)."),
                        QStringLiteral("3. Click 'Calculate Equivalence' to display the fractional exponent formulation and final rate."),
                        QStringLiteral("4. In the 'Real / Apparent Rate' panel, choose whether to 'Calculate Real Rate' or 'Calculate Apparent Rate'."),
                        QStringLiteral("5. Enter the nominal apparent rate and the inflation rate (% per period)."),
                        QStringLiteral("6. Click 'Calculate' to review the mathematical decomposition.")
                    }
                }
            }
        },
        {
            QStringLiteral("sistemas-amortizacao"),
            QStringLiteral("7. Module 5: Loan Amortization Schedules"),
            {
                QStringLiteral("The Amortization module is a flagship engine within Economia_APP, empowering users to simulate and export comprehensive loan repayment schedules."),
                QStringLiteral("It integrates five major amortization systems used in corporate finance and mortgage lending, with custom grace periods and dedicated PDF publishing.")
            },
            {
                QStringLiteral("SAC (Constant Amortization System): constant principal reduction A = P / n. Installments and interest payments decrease steadily over time."),
                QStringLiteral("PRICE (French System): constant installments P = P_0 * [i*(1+i)^n / ((1+i)^n - 1)]. Principal amortization increases as interest charges decrease."),
                QStringLiteral("SAM (Mixed Amortization System): each installment and amortization equals the arithmetic mean of the SAC and Price schedules."),
                QStringLiteral("American System: periodic payment of interest only, with 100% of principal paid off as a bullet payment at maturity (period n)."),
                QStringLiteral("Hamburg System: interest paid in advance at the beginning of each period over outstanding balance, with constant amortization."),
                QStringLiteral("Grace Period Options: define non-amortization periods with optional interest capitalization into principal or periodic interest servicing."),
                QStringLiteral("Direct Period k Lookup: query the outstanding balance, interest, and payment for any intermediate installment without scrolling."),
                QStringLiteral("PDF Schedule Export: dedicated one-click export generating zebra-striped tables, column summaries, and formal headers.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: Generating Amortization Tables"),
                    {
                        QStringLiteral("To generate an amortization schedule:")
                    },
                    {
                        QStringLiteral("1. Open the 'Amortization' tab."),
                        QStringLiteral("2. In the dropdown, select the system: 'SAC', 'PRICE', 'SAM', 'American', or 'Hamburg'."),
                        QStringLiteral("3. Enter Principal Amount (P), Interest Rate (% per period), and Total Number of Installments (n)."),
                        QStringLiteral("4. If a grace period applies, enter the periods in 'Grace Period' and check 'Capitalize grace interest?' if accrued interest should be added to the principal balance."),
                        QStringLiteral("5. To inspect a single line, specify the index in 'Period k'."),
                        QStringLiteral("6. Click 'Calculate Amortization'."),
                        QStringLiteral("7. Inspect the schedule displaying columns: Period, Payment, Interest, Amortization, and Outstanding Balance, along with bottom totals."),
                        QStringLiteral("8. To save the schedule as an executive report, click 'Export PDF'.")
                    }
                }
            }
        },
        {
            QStringLiteral("analise-investimentos"),
            QStringLiteral("8. Module 6: Capital Investment Analysis"),
            {
                QStringLiteral("This module implements core engineering economy indicators for industrial, commercial, and infrastructure capital project assessment."),
                QStringLiteral("It measures economic value creation, intrinsic rate of return, and capital recovery timeframes, producing immediate decision feasibility verdicts.")
            },
            {
                QStringLiteral("Net Present Value (NPV / VPL): NPV = sum(CF_t / (1 + MARR)^t) - I_0. If NPV > 0, the project creates net economic value."),
                QStringLiteral("Equivalent Annual Cost / Worth (EAC / VAUE): translates the NPV into an equivalent uniform annual cash flow over the project lifespan."),
                QStringLiteral("Internal Rate of Return (IRR / TIR): the discount rate where NPV equals zero. Solved via iterative Newton-Raphson numerical algorithm. If IRR > MARR, the project is profitable."),
                QStringLiteral("Simple Payback: nominal time required for cash inflows to fully recover initial outlay I0 without discounting."),
                QStringLiteral("Discounted Payback: true economic recovery timeframe accounting for the time value of money discounted at the MARR."),
                QStringLiteral("Sensitivity Analysis: evaluates NPV responsiveness against percentage shocks (+- Delta %) applied to operating revenues or costs.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Evaluate Projects"),
                    {
                        QStringLiteral("To evaluate an investment proposal:")
                    },
                    {
                        QStringLiteral("1. Open the 'Investment Analysis' tab."),
                        QStringLiteral("2. In the dropdown, pick the analysis type: 'Uniform NPV and EAC', 'Detailed NPV (Cash Flow)', 'Simple and Discounted Payback', 'Sensitivity Analysis', or 'Internal Rate of Return (IRR)'."),
                        QStringLiteral("3. Enter Initial Investment (I0), annual cash flows (or operating revenues and costs), Minimum Attractive Rate of Return (MARR %), and lifespan (n)."),
                        QStringLiteral("4. If running Sensitivity Analysis, enter the variation percentage (+- %)."),
                        QStringLiteral("5. Click 'Calculate Analysis'."),
                        QStringLiteral("6. Read the detailed feasibility opinion displayed in the History Container.")
                    }
                }
            }
        },
        {
            QStringLiteral("metodos-depreciacao"),
            QStringLiteral("9. Module 7: Asset Depreciation Methods"),
            {
                QStringLiteral("The Depreciation module models the systematic allocation of depreciable cost for tangible capital assets (machinery, vehicles, buildings, equipment) across their operational lifespan."),
                QStringLiteral("Supports the three primary methods recognized under international corporate and tax accounting standards.")
            },
            {
                QStringLiteral("Straight-Line Method: constant annual depreciation D_k = (C_0 - V_r) / n."),
                QStringLiteral("Sum-of-the-Years'-Digits (SYD / Cole): accelerated declining method. Annual quota D_k = (C_0 - V_r) * (n - k + 1) / S, where S = n * (n + 1) / 2."),
                QStringLiteral("Declining Balance Method: applies a constant percentage d = 1 - (V_r / C_0)^(1/n) to the beginning-of-year book value."),
                QStringLiteral("Book Value & Depreciable Base: explicitly computes Depreciable Base = C_0 - V_r and Remaining Book Value after each period.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("To compute asset depreciation:")
                    },
                    {
                        QStringLiteral("1. Open the 'Depreciation' tab."),
                        QStringLiteral("2. In the method selector, choose 'Straight-Line', 'Sum-of-the-Years'-Digits', or 'Declining Balance'."),
                        QStringLiteral("3. Enter Initial Acquisition Cost (P or C0), Estimated Salvage Value (Vr), Useful Life in years (n), and consultation year (k)."),
                        QStringLiteral("4. Click 'Calculate Depreciation'."),
                        QStringLiteral("5. The History Container will output the year k depreciation charge, accumulated depreciation, and net book value.")
                    }
                }
            }
        },
        {
            QStringLiteral("taxa-efetiva-tir-global"),
            QStringLiteral("10. Module 8: Effective Rates, Advanced IRR & Capital Costs"),
            {
                QStringLiteral("This module gathers advanced analytical tools for cost of capital modeling, multi-period compounding, and correcting banking distortions."),
                QStringLiteral("Covers everything from nominal-to-effective rate translation to Modified Internal Rate of Return (MIRR).")
            },
            {
                QStringLiteral("Effective Annual and Periodic Rate: converts nominal rates with daily, monthly, quarterly, or semi-annual compounding: i_eff = (1 + i_nom / m)^m - 1."),
                QStringLiteral("Discounted / Upfront Interest Rate: determines True Total Effective Cost in bank discount facilities where interest is withheld upfront: i_eff = i / (1 - i)."),
                QStringLiteral("Global Real Rate under Multi-Inflation: calculates compounded real return for multi-year investments subject to varying annual inflations (m1, m2, m3)."),
                QStringLiteral("Analytical MARR Breakdown: computes cost of capital by summing Risk-Free Rate, Project Risk Premium, and Opportunity Cost."),
                QStringLiteral("Newton-Raphson IRR: high-performance iterative root-finding with quadratic convergence guaranteed."),
                QStringLiteral("Modified Internal Rate of Return (MIRR / TIRM): solves multiple IRR anomalies by applying explicit reinvestment rates for cash inflows and financing rates for outflows.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Operate"),
                    {
                        QStringLiteral("To run advanced rate analyses:")
                    },
                    {
                        QStringLiteral("1. Select the 'Effective Rate / IRR / Global Rate' tab."),
                        QStringLiteral("2. Choose the desired tool from the dropdown menu."),
                        QStringLiteral("3. The form fields will update dynamically to match the selected calculation mode."),
                        QStringLiteral("4. Input the financial parameters."),
                        QStringLiteral("5. Click 'Calculate Rate / IRR'."),
                        QStringLiteral("6. Inspect the resulting mathematical derivations and explicit numerical breakdown.")
                    }
                }
            }
        },
        {
            QStringLiteral("retorno-minimo-tma"),
            QStringLiteral("11. Module 9: Minimum Attractive Rate of Return (MARR)"),
            {
                QStringLiteral("The Minimum Return module calculates the baseline monetary profit and future accumulated capital required to justify an investment at the investor's MARR over n periods."),
                QStringLiteral("Serves as a preliminary screening benchmark for asset allocation and business proposals.")
            },
            {
                QStringLiteral("Minimum Required Profit: Profit = P * [(1 + MARR)^n - 1]."),
                QStringLiteral("Minimum Future Worth: F = P * (1 + MARR)^n."),
                QStringLiteral("Opportunity Cost: quantification of foregone earnings from the next best alternative of equal risk.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("Procedure:")
                    },
                    {
                        QStringLiteral("1. Open the 'Minimum Return (MARR)' tab."),
                        QStringLiteral("2. Enter Invested Capital (P), MARR (% per period), and Number of Periods (n)."),
                        QStringLiteral("3. Click 'Calculate Minimum Return'."),
                        QStringLiteral("4. Review the target future balance and required profit margin.")
                    }
                }
            }
        },
        {
            QStringLiteral("equacao-de-fisher"),
            QStringLiteral("12. Module 10: Fisher Effect & Inflation"),
            {
                QStringLiteral("Implements the classical formulation established by Irving Fisher, defining the relationship between nominal interest rate, real interest rate, and inflation rate."),
                QStringLiteral("Essential for assessing true economic yields in inflationary macroeconomic climates.")
            },
            {
                QStringLiteral("Fundamental Formulation: (1 + i_nominal) = (1 + i_real) * (1 + inflation)."),
                QStringLiteral("Nominal Rate Solving: i_nominal = (1 + i_real) * (1 + inflation) - 1, identifying the necessary contract rate to secure target real growth."),
                QStringLiteral("Real Rate Solving: i_real = ((1 + i_nominal) / (1 + inflation)) - 1, isolating real capital accumulation.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Compute"),
                    {
                        QStringLiteral("To apply the Fisher equation:")
                    },
                    {
                        QStringLiteral("1. Open the 'Fisher Equation' tab."),
                        QStringLiteral("2. Choose 'Calculate Nominal Rate' or 'Calculate Real Rate'."),
                        QStringLiteral("3. Input known rates into the active fields."),
                        QStringLiteral("4. Click 'Calculate Fisher'."),
                        QStringLiteral("5. Observe the calculation memory with step-by-step cross-term expansion.")
                    }
                }
            }
        },
        {
            QStringLiteral("vpl-com-tributos"),
            QStringLiteral("13. Module 11: After-Tax NPV & Depreciation Tax Shield"),
            {
                QStringLiteral("Corporate feasibility studies that ignore corporate taxation and tax deductions produce distorted investment appraisals."),
                QStringLiteral("The 'NPV with Taxes' module factors in corporate income tax (IRPJ and CSLL), straight-line depreciation tax shields, debt financing tax deductibility, and capital gain taxes upon equipment disposal.")
            },
            {
                QStringLiteral("Tax Rates: IRPJ (standard 15% or 25% with surtax) and CSLL (standard 9%)."),
                QStringLiteral("Depreciation Tax Shield: Annual Savings = Depreciation_Charge * (IRPJ + CSLL), boosting net operational cash flows."),
                QStringLiteral("Debt Financing Integration: tax deductibility of loan interest expenses."),
                QStringLiteral("Salvage Value Disposal: capital gains taxation = Tax_Rate * (Sale_Price - Book_Value)."),
                QStringLiteral("After-Tax Free Cash Flow (FCF): year-by-year cash projections discounted at the company's MARR to yield the True After-Tax NPV.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: Modeling Taxed Projects"),
                    {
                        QStringLiteral("To evaluate after-tax viability:")
                    },
                    {
                        QStringLiteral("1. Open the 'NPV with Taxes' tab."),
                        QStringLiteral("2. Enter Initial Capital Outlay and projected Gross Operating Profit before taxes."),
                        QStringLiteral("3. Specify Useful Life in years, IRPJ tax rate (%), and CSLL tax rate (%)."),
                        QStringLiteral("4. Enter corporate hurdle rate (MARR %)."),
                        QStringLiteral("5. If financed, check 'Financed Acquisition?' and specify debt interest rate and loan term in years."),
                        QStringLiteral("6. If asset is disposed at project termination, enter Sale Year and estimated market Resale Value."),
                        QStringLiteral("7. Click 'Calculate NPV with Taxes'."),
                        QStringLiteral("8. Analyze the complete cash schedule displaying yearly Net Cash Flows, discounted values, and final NPV.")
                    }
                }
            }
        },
        {
            QStringLiteral("caue-vida-economica"),
            QStringLiteral("14. Module 12: EAC / CAUE & Economic Life Analysis"),
            {
                QStringLiteral("The Equivalent Annual Cost (EAC / CAUE) module is the definitive tool for optimizing equipment replacement timing for machinery, fleets, and industrial plant assets."),
                QStringLiteral("It resolves the classical replacement dilemma between retaining aging equipment (incurring escalating repair costs and breakdowns) versus purchasing new units (incurring high capital recovery charges).")
            },
            {
                QStringLiteral("Capital Recovery Cost (CRC): CRC_k = [P - VR_k * (1 + i)^(-k)] * [i * (1 + i)^k / ((1 + i)^k - 1)], where P is purchase price and VR_k is salvage value at year k."),
                QStringLiteral("Annualized Operating Cost (AOC): present value of cumulative maintenance and operating expenses through year k, converted into an equivalent annual series."),
                QStringLiteral("Total Equivalent Annual Cost: CAUE_k = CRC_k + AOC_k."),
                QStringLiteral("Optimal Replacement Year k*: the algorithm automatically identifies the year that minimizes the total CAUE curve, indicating the optimal replacement date."),
                QStringLiteral("Interactive Input Grid: flexible table enabling annual entry of expected resale values and operating/maintenance costs.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: Optimizing Asset Replacement"),
                    {
                        QStringLiteral("Follow this step-by-step workflow:")
                    },
                    {
                        QStringLiteral("1. Open the 'CAUE - Economic Life' tab."),
                        QStringLiteral("2. Enter Initial Asset Purchase Cost (P), MARR (% per year), and Maximum Planning Horizon in years (N)."),
                        QStringLiteral("3. Click 'Generate Input Table' to instantiate the yearly cost grid."),
                        QStringLiteral("4. In the generated table, enter the expected Resale Value and Operating Cost for each year k."),
                        QStringLiteral("5. Click 'Calculate CAUE'."),
                        QStringLiteral("6. The system populates the output table with CRC, AOC, and Total CAUE for each prospective year."),
                        QStringLiteral("7. Review the highlighted recommendation stating the Optimal Replacement Year and Minimum CAUE.")
                    }
                }
            }
        },
        {
            QStringLiteral("historico-calculos"),
            QStringLiteral("15. History Container & Record Management"),
            {
                QStringLiteral("Every calculation tab in Economia_APP integrates an intelligent History Container (HistoryContainer), designed to ensure full auditability, inline editing, and preservation of calculation records throughout the session.")
            },
            {
                QStringLiteral("Built-in Search Bar: real-time keyword and numeric search with visual highlighting and occurrence navigation."),
                QStringLiteral("Inline Editing: clicking 'Edit Calculation' unlocks the selected record for manual annotations and corrections, confirmed via the same button."),
                QStringLiteral("Selective Deletion: removes unwanted calculation entries via 'Delete Selection'."),
                QStringLiteral("Structured Clearing: 'Clear Inputs' resets form controls, 'Clear Output' clears tab history, and 'Clear All' resets both simultaneously."),
                QStringLiteral("Quick Exporting: direct export to formatted plain text (.txt) or structured PDF documents (.pdf)."),
                QStringLiteral("Session Persistence: calculation history remains preserved when switching between different tabs.")
            },
            {
                {
                    QStringLiteral("Productivity Tips"),
                    {
                        QStringLiteral("Maximizing dock functionality:")
                    },
                    {
                        QStringLiteral("Adjust the proportion between form and output by dragging the splitter handle with the mouse."),
                        QStringLiteral("Select any calculation block to inspect or edit its detailed derivation."),
                        QStringLiteral("Use the search bar to locate specific numeric values across extensive multi-step sessions.")
                    }
                }
            }
        },
        {
            QStringLiteral("configuracao-fontes-exportacao-pdf"),
            QStringLiteral("16. Font Configuration & PDF Export Engine"),
            {
                QStringLiteral("Economia_APP is built to generate executive-ready reports for engineering documentation, academic presentations, and legal appraisal exhibits."),
                QStringLiteral("The PDF engine leverages Qt6 PrintSupport (QPrinter and QPainter) to ensure balanced margins, sharp vector text, and clean tabular alignment.")
            },
            {
                QStringLiteral("Active Tab Export: triggered via File → Export Current or by the tab's dedicated 'Export PDF' button."),
                QStringLiteral("Consolidated Export: triggered via File → Export All, compiling all computed sections into a single multi-page report."),
                QStringLiteral("Automatic Pagination: includes formal headers, 'Page X of Y' numbering, timestamping, and smart line/table break controls."),
                QStringLiteral("MathRenderer Engine: typographic rendering of radical roots, superscripts, subscripts, and fractions."),
                QStringLiteral("Typographic Customization: font changes propagate instantly to both the screen interface and the print engine.")
            },
            {
                {
                    QStringLiteral("Step-by-Step: How to Export PDF Reports"),
                    {
                        QStringLiteral("Procedure:")
                    },
                    {
                        QStringLiteral("1. Perform your calculations in the desired tab (or across multiple tabs for a consolidated report)."),
                        QStringLiteral("2. Click 'Export PDF' or choose File → Export Current / Export All."),
                        QStringLiteral("3. In the save file dialog, pick a target folder and specify the file name."),
                        QStringLiteral("4. Click 'Save'."),
                        QStringLiteral("5. The software will generate the publication-grade PDF document.")
                    }
                }
            }
        },
        {
            QStringLiteral("atalhos-e-dicas"),
            QStringLiteral("17. Keyboard Shortcuts, Best Practices & Troubleshooting"),
            {
                QStringLiteral("Follow these recommendations to maximize workflow efficiency and prevent common operating errors.")
            },
            {
                QStringLiteral("Ctrl+Shift+A: Opens the About dialog from any screen or tab, featuring full-text search across all documentation tabs and minimize/maximize window controls."),
                QStringLiteral("Ctrl+Shift+M: Opens this User Manual from any screen or tab."),
                QStringLiteral("Enter Key: Triggers the active tab's calculation button when focused on any input field."),
                QStringLiteral("Tab / Shift+Tab: Navigates forward or backward across input fields."),
                QStringLiteral("Numeric Formatting: Use standard decimal points (.) or commas (,) according to system regional settings."),
                QStringLiteral("Time Consistency: Always ensure interest rate compounding intervals match the time unit of periods n (e.g., monthly rate with months; annual rate with years). Use the Rate Conversion tab when needed."),
                QStringLiteral("AppData Storage: User preferences and evaluation records are safely maintained under Windows AppData (%APPDATA%/Economia_APP)."),
                QStringLiteral("Audit Logging: Continuous system telemetry is handled via LogManager for fast diagnostics.")
            },
            {
                {
                    QStringLiteral("Common Troubleshooting"),
                    {
                        QStringLiteral("Quick fixes for common issues:")
                    },
                    {
                        QStringLiteral("Calculation does not trigger: Verify all mandatory fields contain valid numeric inputs. Blank or non-numeric characters prevent execution."),
                        QStringLiteral("IRR returns convergence error: Cash flow streams with multiple sign reversals may lack a unique real root. Try the 'Effective Rate / IRR' tab in MIRR mode."),
                        QStringLiteral("Trial Evaluation Expired: If your 30-day evaluation period has ended, contact developer Fernando Nillsson Cidade to renew your license.")
                    }
                }
            }
        }
    };
}

} // namespace source::ui::manual_en_US
