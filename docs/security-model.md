# Modelo de Segurança e Hardening — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

[🇺🇸 English version below](#-security-model--hardening--economia_app)

<a id="-seguranca-pt-br"></a>
## 🇧🇷 Controles de Segurança do Aplicativo e Build

1. **Superfície de Ataque Mínima (Offline-First)**:
   - O **Economia_APP** não expõe sockets, servidores HTTP, listeners IPC em rede nem parsers de scripts remotos.
   - As únicas leituras externas ocorrem sobre ativos locais (`source/assets/`, `.qm`), arquivos JSON locais gerenciados pelo próprio aplicativo e exportações explícitas selecionadas pelo usuário.

2. **Hardening de Compilação e Linkagem (`CMakeLists.txt`)**:
   - **MSVC puro (`Visual Studio 18 2026` MSBuild x64, `/std:c++latest`)**:
     - `/permissive-` (conformidade estrita de linguagem C++23), `/GS` (proteção contra estouro de buffer na pilha), `/guard:cf` (Control Flow Guard), `/DYNAMICBASE` (ASLR), `/NXCOMPAT` (DEP), `/HIGHENTROPYVA` (ASLR 64-bit) e `/utf-8`.
   - **MinGW-w64 (`GCC 15.1.0`, `-std=c++23`)**:
     - `-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`, `-Wl,--dynamicbase`, `-Wl,--nxcompat`, `-Wl,--high-entropy-va`.

3. **Validação de Entradas e Estabilidade Numérica**:
   - Campos numéricos de entrada utilizam `QDoubleValidator` com notação padrão, além de verificações explícitas contra divisão por zero, períodos não positivos (`n <= 0`), fluxos de caixa degenerados na TIR (Newton-Raphson com convergência limitada) e desvios inválidos antes da renderização.

4. **Isolamento Absoluto de Binários (MSVC vs MinGW)**:
   - Os builds `build_Economia_APP_msvc` / `build_mocks_msvc` / `build_suite_msvc` e os equivalentes MinGW (`build_Economia_APP_mingw` / `build_mocks_mingw` / `build_suite_mingw`) são rigorosamente isolados.
   - A etapa de deploy executa `cmake/prune_optional_root_runtime_dlls.cmake`, removendo qualquer DLL estrangeira (`libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` em distribuições MSVC; `msvcp140*.dll`, `vcruntime140*.dll` em distribuições MinGW).

5. **Reportando Vulnerabilidades**:
   - Consulte [`SECURITY.md`](../SECURITY.md) para o canal de divulgação responsável.

---

<a id="-security-model--hardening--economia_app"></a>
## 🇺🇸 Security Model & Hardening — Economia_APP

- **Minimal Attack Surface**: Pure offline Qt6 desktop calculator with zero network listeners or telemetry endpoints.
- **Compiler & Linker Mitigations**: Both Pure MSVC (`/GS`, `/guard:cf`, `/DYNAMICBASE`, `/NXCOMPAT`, `/HIGHENTROPYVA`, `/permissive-`) and MinGW (`-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`, `--dynamicbase`, `--nxcompat`, `--high-entropy-va`) enforce modern Windows PE32+ exploit mitigations.
- **Strict CRT Isolation**: Verified automated PE import table and directory hygiene checks inside `test_09_toolchain_isolation_and_binaries` confirm zero cross-contamination between MSVC and MinGW runtimes.
