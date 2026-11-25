# Guia de Contribuição / Contributing Guide — Economia_APP

**Versão:** `2026.11.22.0` | **Data:** `22 de novembro de 2026`  
**Version:** `2026.11.22.0` | **Date:** `November 22, 2026`

## 🇧🇷 Como Contribuir

1. **Pré-requisitos**:
   - Qt `6.12.0` (MSVC 2022/2026 x64 e/ou MinGW 64-bit `GCC 15.1.0`)
   - CMake `3.21+` e Visual Studio 18 2026 (`v145`, Windows SDK `10.0.28000.0`) para builds MSVC puros
2. **Compilação e Verificação Obrigatórias**:
   - Execute a suíte de testes completa antes de propor alterações:
     - **MSVC**: `cmake --preset suite-msvc-release && cmake --build --preset suite-msvc-release --target EconomiaSuiteDeploy --parallel && ctest --preset suite-msvc-release --output-on-failure`
     - **MinGW**: `cmake --preset suite-mingw-release && cmake --build --preset suite-mingw-release --target EconomiaSuiteDeploy --parallel && ctest --preset suite-mingw-release --output-on-failure`
   - Verifique a consistência de versões executando `find_VersionEditor.exe --check`.
3. **Licenciamento das Contribuições**:
   - Todo código contribuído para este repositório público é disponibilizado sob os termos da licença **GNU AGPLv3** ([`LICENSE`](LICENSE)). Consulte [`docs/licensing.md`](docs/licensing.md) para detalhes sobre o duplo licenciamento com a versão distribuída na Microsoft Store.

---

## 🇺🇸 Contributing Guidelines

- Build and validate changes against both `suite-msvc-release` (Pure MSVC MSBuild x64) and `suite-mingw-release` (MinGW GCC 15.1.0), ensuring all 10 `CTest` suites and `find_VersionEditor --check` pass before opening a pull request.
- Public repository contributions are governed by **GNU AGPLv3** (`LICENSE`); see [`docs/licensing.md`](docs/licensing.md).
