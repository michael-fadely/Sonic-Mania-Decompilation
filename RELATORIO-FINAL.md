# Sonic Mania Plus → Dreamcast — Relatório de Build (madrugada 2026-07-28)

## TL;DR
**O jogo foi buildado com sucesso do zero até um CDI bootável.**

Artefato final (no ryzen):
`/mnt/200GB/sonic-mania-dreamcast/workspace/SonicManiaDC.cdi` — **710 MB, bootável**

Branch: `requeijaum/dc-build-fixes` (base: upstream `sf94/dreamcast-kallistios-pvr`)

---

## Pipeline executado (tudo EXIT=0)

| Etapa | Resultado |
|-------|-----------|
| 1. Toolchain Docker | imagem `sonic-dc-toolchain` sha256:b54664e0 — SH4 gcc 15.2, AICA arm-eabi 8.5, KOS, sh4zam, pvrtex, wav2adpcm, **mkdcdisc**, ffmpeg, imagemagick |
| 2. Engine | `RSDKv5.elf` linkado (rev2, GAME_STATIC, sh4zam, libogg/theora vendored) — só warnings benignos |
| 3. Assets | Data.rsdk (207 MB) → 388 MB DC-nativo: .dtex (PVR), ADPCM, MPEG1, meshes otimizados. `-- DONE --` |
| 4. Disco | cd-root 587 MB (Data.rsdk + Data/<media>) → CDI 710 MB via mkdcdisc |

## Verificação final de boot (Flycast standalone buildado na workspace)
O Flycast foi clonado e compilado localmente com símbolos de debug em
`workspace/flycast/build/flycast`. O console serial do Dreamcast foi usado para
localizar e corrigir o bootloop.

Resultado do teste final de 45 s:
- **0 resets** e **0 fatal/SH4 exception/panic/assert**
- `Data.rsdk`, GameConfig, Title Stage e StringsEN carregados do GD-ROM
- 17 assets da Title Scene carregados
- Logo, Sonic e background alocados com sucesso na VRAM PVR
- Rafael ouviu a música/áudio do jogo funcionando no Flycast com tela real
- Instrumentação temporária `[BOOTDBG]` removida no build final

Conclusão: **o CDI boota e chega à tela de título; o bootloop foi corrigido.**

## Bugs corrigidos no caminho (commitados)
1. `makejobs=-j12` quebrava o KOS build → número puro (`makejobs=12`)
2. `mania-mesh-optimizer` exigia C++20 `<format>` (gcc 13+, host tem 12) →
   shim `mmo::format`. Sem isso, nenhuma Special Stage 3D convertia.
3. Container root poluía a árvore (`__pycache__`, `cmake-build-release`) e
   quebrava `rsync --delete` → `cmd_fixperms` + containers rodam `--user $uid:$gid`
4. O stub GDB do KOS iniciava automaticamente com `RSDK_DEBUG=1`, bloqueava a
   serial e resetava após ~13 s → agora exige `RSDK_KOS_GDB_STUB=1` explícito.
5. `KOS_USER_DIR` era declarado como `option()` booleano e virava `"OFF"` →
   convertido para `CACHE STRING` e fixado como `/cd/` pelo build.
6. `mkdcdisc -d cd-root` incluía o próprio diretório, criando
   `/cd/cd-root/Data.rsdk` → trocado por `-D`, gerando `/cd/Data.rsdk`.
7. Data.rsdk de 207 MB era configurado para buffer integral nos 16 MB de RAM →
   KallistiOS agora faz streaming sob demanda do GD-ROM.

## Como jogar
- **Emulador:** abrir `SonicManiaDC.cdi` no Flycast (standalone recomendado)
- **Console real:** GDEMU / MODE / USB-GDROM lê o CDI direto
  (710 MB só cabe em CD-R via overburn)

## Próximos passos possíveis (opcionais)
- [x] Confirmar boot e áudio no Flycast
- [ ] Testar gameplay prolongado e saves/VMU
- [ ] Testar no Dreamcast real com GDEMU/MODE
- [ ] Puxar o CDI pra ideapad ou pendrive
- [ ] Gerar `.gdi` se o loader preferir
- [ ] Trimar o Data.rsdk pra logic-only (economiza ~200 MB)

## Arquivos-chave
- `docker/Dockerfile`, `docker/build.sh` (image/engine/assets/disc)
- `BUILD-DREAMCAST.md` (documentação completa do processo)
- `workspace/` (remoto, gitignored): Data.rsdk, cd-data, cd-root, ELF, **CDI**
- Logs: `workspace/{image-rebuild,engine-build,assets-build4,disc-build,flycast-boot*}.log`
