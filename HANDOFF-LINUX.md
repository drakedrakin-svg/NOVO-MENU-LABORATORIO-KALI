# HANDOFF — Squid-Ext

> **Status deste snapshot:** é uma cópia de `Squid-Ext.zip` de **22/10/2026 21:25**.
> Ele **NÃO** contém o trabalho de licença (ver seção 6). Leia antes de assumir que
> algo está pronto.

---

## 0. REGRA NÚMERO UM — LEIA ANTES DE ESCREVER QUALQUER LINHA

**NÃO INVENTE CÓDIGO, FUNÇÕES, OFFSETS, STRUCTS, IOCTLs, NOMES DE DEVICE OU
ASSINATURAS QUE VOCÊ NÃO TENHA LIDO NO REPOSITÓRIO.**

1. **Abra o arquivo e leia a linha.** Não deduza pelo nome da função.
2. **Cite arquivo e linha** em toda afirmação. Sem linha, não vale.
3. **Se não achou, diga "não encontrei"** — nunca preencha com valor plausível.
4. **Não confie em comentário.** `src/Src/Core/SDK/Memory.cpp:349` diz
   `// Replace with actual URL` sobre um link **que está em uso**.
5. **Não invente offset** — leia de `Core/SDK/Offsets.hpp`.
6. **Não invente IOCTL** — são os cinco da seção 3.
7. **Não invente struct** — estão em `Driver.hpp:27-62`.
8. **Se faltar dado, PARE e pergunte.**

---

## 1. POR QUE NÃO COMPILA NO LINUX — NÃO TENTE

Não é "complexo". **É impossível.** Três travas concretas:

### Trava 1 — `#pragma comment(lib, ...)` é ignorado fora do MSVC

O projeto linka libs por pragma, não pelo `vcxproj`:

```
src/Ext/Includes/Includes.hpp    ws2_32, Normaliz, Crypt32, Wldap32,
                                 libcurl, bcrypt, d3d11, D3DX11
src/Ext/Globals.hpp              d3d11, D3DX11
src/Src/Core/SDK/SDK.hpp         D3DX11
```

MinGW/clang **não interpretam esse pragma**. As libs sairiam do link
silenciosamente — você receberia centenas de símbolos não resolvidos **sem
indicação de qual lib faltou**.

### Trava 2 — `D3DX11.lib` não existe fora do Windows

Binário COFF da Microsoft. **Não há `.a` equivalente** e não dá para gerar.

### Trava 3 — `D3DX11CreateShaderResourceViewFromMemory` só vive em `d3dx11_43.dll`

```
Overlay.cpp   L219         1 chamada
Overlay.cpp   L468-L564    ~94 chamadas
Overlay.hpp   L49-L50      D3DX11_IMAGE_LOAD_INFO, ID3DX11ThreadPump
```

**~95 pontos.** O header `d3dx11.h` nem é o obstáculo real: sem o `.lib` e sem a
DLL da Microsoft, nada linka.

**Extensões MSVC-only ainda presentes:** `__try`/`__except` (SEH),
`__forceinline`, `__declspec`, `__int64`, `#pragma comment(lib,...)`.

**Conclusão: o build é no Windows, com MSVC/v143. Você lê, revisa e escreve
patches; o build roda na máquina do dono.**

---

## 2. O QUE VOCÊ PODE FAZER SEM COMPILAR

**A parte mais valiosa não precisa de build.** Os sete defeitos da seção 4 são
todos verificáveis por leitura.

Ciclo recomendado: **você lê e escreve o patch → o dono aplica e compila →
você recebe o resultado.**

---

## 3. OS CINCO IOCTLs — OS ÚNICOS QUE EXISTEM

`src/Ext/Driver/Driver.hpp:12-16`

| Macro | Código | Struct | Wrapper | Chamado? |
|---|---|---|---|---|
| `req_rw` | `0x32F1` | `rw` | `read_physical` / `write_physical` | sim |
| `req_base` | `0x32F2` | `ba` | `get_base_address` | sim |
| `req_cr3` | `0x32F3` | `dtb` | `CR3` | sim |
| `req_mouse` | `0x4563` | `mousenigga` | `MOVE` | **NÃO** |
| `req_peb` | `0x35F5` | `PEB_REQUEST` | `get_peb_address` | **NÃO** |

**Não existe um sexto. Não invente.** `_ga` está declarado e nunca usado — não
invente IOCTL para ele.

Device: `#define drvname L"GN6Qn-554SFf"` (`Driver.hpp:21`).

---

## 4. OS SETE DEFEITOS DO DRIVER — VERIFICADOS POR LEITURA

### 4.1 O driver certo está no código e é ignorado

`src/Ext/Driver/Driver.hpp:195`

```cpp
inline unsigned char driver_data[42136] = {
    0x4D, 0x5A, 0x90, 0x00, ...     // "MZ" — é um PE
```

- Declarado `[42136]`, preenchido com **42136 elementos** (conferido por contagem)
- Strings do array: `\Device\GN6Qn-554SFf`, `\DosDevices\GN6Qn-554SFf`
- **Bate com `Driver.hpp:21`**
- Referencia `crl.comodoca.com/AAA Certificate Services` → **assinado**

**Nunca é usado. Nenhuma referência fora da própria declaração.**

### 4.2 `LoadDriver()` baixa outro driver

`src/Src/Core/SDK/Memory.cpp:349-361`

```cpp
std::string url = "https://files.catbox.moe/td4bn6.sys"; // Replace with actual URL
URLDownloadToFileA(NULL, url.c_str(), "C:\\Windows\\4ware.sys", 0, NULL);
system("sc create 4ware type= kernel start= auto binPath= \"C:\\Windows\\4ware.sys\"");
system("sc start 4ware");
```

Três problemas: serviço `4ware` ≠ device `GN6Qn-554SFf`; o comentário admite que
o link é falso; **sem hash e sem verificação de assinatura**.

### 4.3 `init()` abre, nunca carrega

`Driver.hpp:71-89` — `CreateFileW` abre device existente. Não instala serviço.

### 4.4 Falha silenciosa que reporta sucesso

`Memory.cpp:378-382`

```cpp
if (!driver::init()) {
    std::thread(LoadDriver).detach();
    return 1;          // ← OpenProcByPid é BOOL: 1 = TRUE = SUCESSO
}
```

### 4.5 `ModBaseSize` recebe o endereço base

`Memory.cpp:389-390` — os dois campos recebem `driver::base_addrrs`. No caminho
usermode (`Memory.cpp:401`) o tamanho é preenchido de verdade. **Assimetria.**

### 4.6 `driver::MOVE` é IOCTL morto

`Driver.hpp:17` define `req_mouse` (`0x4563`), `Driver.hpp:146` define `MOVE`.
**Zero chamadas.** Evitaria `SetCursorPos`/`mouse_event`, bem mais visíveis.

### 4.7 `GeneralCleaner.hpp` tem 0 byte

`src/Ext/Bypass/GeneralCleaner.hpp` — 0 bytes, incluído em `main.cpp:18`, nunca
chamado. Arquivo vazio compila; é lixo morto.

---

## 5. BLOQUEIO DE BUILD — DOIS PASSOS, NA ORDEM

### Passo 1 — DirectX SDK (June 2010)

Verificado no disco da máquina do dono:

```
D3DX11.lib, d3dx11.h, D3DX11tex.h, d3dx9.lib  →  NÃO EXISTEM
Windows SDK 10.0.26100.0                       →  tem d3d.h, d3d10.h
                                                  NÃO tem D3DX
```

**Remover D3DX está descartado** — são ~95 pontos em `Overlay.cpp`.

### Passo 2 — `libcurl.lib` incompatível (só aparece depois do passo 1)

29 símbolos de CRT não resolvidos:

```
fgets, close, open, strcspn, strspn, _gmtime64, _beginthreadex,
_getpid, _fstat64, __sys_nerr, _unlink, _access, _open ...
```

`libcurl.lib` é versão **7.75.0**, CRT incompatível com o VS 2022.

**PISTA — `/DCURL_STATICLIB` é obrigatório.** Sem ele, `curl.h` declara
`__imp_curl_*` (importação de DLL) e falha com 9 erros que **parecem** ser o
`.lib` errado. **São duas causas distintas; separe-as.**

**Instalar software é decisão do dono. Pergunte.**

---

## 6. O QUE ESTE SNAPSHOT NÃO TEM

O trabalho de licença foi feito **depois** deste zip. **Não está aqui:**

- `src/Ext/Auth/Hwid.hpp` — SHA256 do HWID
- `src/Ext/Auth/License.hpp` — validação HTTP
- `src/Src/Gui/Pages/Login.hpp` — **aqui é o stub antigo** (30 linhas, só um
  `CentralizedText("LOG IN", ...)`, sem campo de chave, sem validação)
- `src/Ext/Globals.hpp` — `IsLogged` ainda é `true`
- `src/Src/Gui/Gui.cpp` — **`Login::Render()` nunca é chamado**

**Contrato do endpoint (verificado contra o servidor real), para quando for
reintegrar:**

- `POST https://harxmenus.discloud.dev/api/licenses/validate`
- Request: `{"key":"...","productId":"...","hwid":"..."}` — camelCase, nulos omitidos
- Response: `{"valid":bool,"error":"...","productId":"...","product":"...","expiresAt":"...","reason":"..."}`
- User-Agent do menu C#: `HarxMenu/1.0`
- HWID = `SHA256("{MachineName}|{OSVersion}|{VolumeSerial:X8}")[0..16]` em hex maiúsculo
- **`productId` do Squid-Ext é um placeholder** — o produto precisa ser registrado
  no `/loja` do bot

---

## 7. ARMADILHAS JÁ ENCONTRADAS

1. **Comentário mente.** `Memory.cpp:349` — `// Replace with actual URL` num link em uso.
2. **Build incremental mente.** Em projeto irmão, "0 erros" manteve o binário
   antigo. Só `-t:Rebuild` gerou o arquivo certo. **Confira a data e o conteúdo.**
3. **Busca por string no binário dá falso negativo.** Strings de C# ficam em
   **UTF-16**, não ASCII. Ofuscação cifra strings.
4. **`GeneralCleaner.hpp` com 0 byte não é erro de leitura** — verificado nos bytes.
5. **Não afirme que "não existe .sys no projeto".** Existe: embutido em
   `Driver.hpp:195`.

---

## 8. LIMITES — O QUE NÃO ESCREVER

- Manual mapper / carregamento de driver não assinado contornando o Windows
- Limpeza de rastro, evasão de detecção, spoof de HWID, contorno de ban
- Qualquer coisa que ataque a máquina de terceiros

`src/Ext/Bypass/{Replace,Prefetch}` fazem limpeza de rastro (`SetFileTime` em
arquivos, reset de processo). **Não estenda isso.**

**As correções da seção 4 são manutenção legítima** — usar os bytes que já estão
no código, acertar nomes, parar de engolir falha. **Isso faça.**
