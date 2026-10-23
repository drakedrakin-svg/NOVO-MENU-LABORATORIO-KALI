# Projeto Squid-Ext — Instruções para o Laboratório

> **Antes de tudo:** leia o `HANDOFF-LINUX.md`. Ele tem as âncoras verificadas
> (IOCTLs, structs, defeitos do driver) e responde por que o build não pode ser
> feito aqui.

---

## Regra número um

**NÃO INVENTE CÓDIGO, FUNÇÕES, OFFSETS, STRUCTS, IOCTLs, NOMES DE DEVICE OU
ASSINATURAS QUE VOCÊ NÃO TENHA LIDO NO REPOSITÓRIO.**

- **Cite arquivo e linha** em toda afirmação. Sem linha, não vale.
- **Se não achou, diga "não encontrei"** — nunca preencha com valor plausível.
- **Não confie em comentário.** Os deste projeto mentem.
- **Não invente offset** — `Core/SDK/Offsets.hpp`.
- **Não invente IOCTL** — são cinco, listados no handoff.
- **Não invente struct** — `Driver.hpp:27-62`.
- **Faltou dado? PARE e pergunte.**

Um valor inventado que "parece certo" quebra em runtime e custa horas para achar.
Um "não sei, me diga" custa trinta segundos.

---

## O que é o projeto

- **Squid-Ext** — cheat externo para FiveM
- **C++ / MSVC**, ImGui + DirectX 11
- **Solução:** `Fivem-External.sln` → `src/src.vcxproj`
- **Toolset:** `v143` / `WindowsTargetPlatformVersion 10.0`
- **Estado:** **nunca compilou.** Ver `HANDOFF-LINUX.md` §5.

### Estrutura

```
src/main.cpp                  entrada (WinMain + DllMain)
src/Ext/                      código do cheat
  Driver/Driver.hpp           driver de kernel embutido (262 KB)
  Bypass/                     Replace/, Prefetch/, GeneralCleaner.hpp (0 B)
  Security/AntiCrack.hpp      anti-debug — nunca chamado
  Includes/                   Includes.hpp, CustomWidgets/, ImGui/
src/Src/Core/                 SDK/Memory.cpp, SDK/Offsets.hpp, Features/
src/Src/Gui/                  Gui.cpp, Pages/, Overlay/
```

---

## Como trabalhar aqui

1. **Leia antes de escrever.** Cite arquivo e linha.
2. **Não invente.** Nem offset, nem IOCTL, nem função, nem struct.
3. **Não confie em comentário** — verifique o código.
4. **Confirme o binário.** "0 erros" não garante que o arquivo mudou.
5. **Teste o que você não tem certeza**, não o que já funciona.
6. **Falha silenciosa é bug.** `return true` depois de falhar é bug.
7. **Diga "não sei".** Melhor que um valor inventado.

---

## Onde começar

**Os sete defeitos do driver** (`HANDOFF-LINUX.md` §4) são o melhor ponto de
partida — todos verificáveis por leitura, nenhum precisa de build:

1. `driver_data[42136]` — **é uma DLL x64, NÃO um driver de kernel** (Subsystem
   `523`, flag `DLL` setada, zero símbolos de kernel). Nunca é usado.
2. `LoadDriver` baixa do catbox um arquivo de terceiros, com **nome de serviço
   trocado** (`4ware` vs device `GN6Qn-554SFf`)
3. `init()` só abre, nunca carrega
4. `return 1` (= `TRUE` = sucesso) depois de falhar
5. `ModBaseSize` recebendo o endereço base
6. `driver::MOVE` — IOCTL morto
7. `GeneralCleaner.hpp` — 0 byte

---

## O que NÃO escrever

- Manual mapper / carregamento de driver não assinado contornando o Windows
- Limpeza de rastro, evasão de detecção, spoof de HWID, contorno de ban
- Qualquer coisa que ataque a máquina de terceiros

As correções do driver **são** manutenção legítima: usar os bytes que já estão no
código, acertar nomes, parar de engolir falha. **Isso faça.**
