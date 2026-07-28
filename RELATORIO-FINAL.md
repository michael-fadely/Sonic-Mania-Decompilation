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

## Verificação de boot (Flycast headless no ryzen, via RetroArch core)
Evidência técnica de que **o disco boota e o engine roda**:
- CDI reconhecido: `retro_load_game: SonicManiaDC.cdi`
- Hardware DC resetou/executou bootstrap: `SB/HOLLY: System reset requested`
- Renderer GL (llvmpipe) OK: FBO, texturas, `glBlitFramebuffer test successful`
- Loop de frames a 59.94 FPS (`SET_GEOMETRY` repetido continuamente)
- **Zero "File not found", zero exception/abort** → assets em `/cd/Data/` localizados = layout de disco correto
- Rodou 55-75 s sem crashar

**NÃO confirmado visualmente:** um screenshot do menu. Limitação de captura
headless — o RetroArch+libretro renderiza num surface GL próprio que
xwd/import não capturam da root window do Xvfb (não há flycast standalone
instalado no ryzen). Isto é limitação da captura, NÃO do disco.
→ **Confirmação final precisa do Rafael:** abrir no Flycast com tela real,
  ou num Dreamcast com GDEMU/MODE.

## Bugs corrigidos no caminho (commitados)
1. `makejobs=-j12` quebrava o KOS build → número puro (`makejobs=12`)
2. `mania-mesh-optimizer` exigia C++20 `<format>` (gcc 13+, host tem 12) →
   shim `mmo::format`. Sem isso, nenhuma Special Stage 3D convertia.
3. Container root poluía a árvore (`__pycache__`, `cmake-build-release`) e
   quebrava `rsync --delete` → `cmd_fixperms` + containers rodam `--user $uid:$gid`
4. `cmd_disc` montava layout errado → corrigido: `/cd/Data.rsdk` (datapack) +
   `/cd/Data/<media>` soltos (o engine sobrepõe o rsdk com os arquivos DC-nativos)

## Como jogar
- **Emulador:** abrir `SonicManiaDC.cdi` no Flycast (standalone recomendado)
- **Console real:** GDEMU / MODE / USB-GDROM lê o CDI direto
  (710 MB só cabe em CD-R via overburn)

## Próximos passos possíveis (à escolha do Rafael)
- [ ] Confirmar boot visual (Flycast com tela / Dreamcast real)
- [ ] Puxar o CDI pra ideapad ou pendrive
- [ ] Gerar `.gdi` se o loader preferir
- [ ] Trimar o Data.rsdk pra logic-only (economiza ~200 MB — hoje ele ainda
      carrega a mídia original não-usada)

## Arquivos-chave
- `docker/Dockerfile`, `docker/build.sh` (image/engine/assets/disc)
- `BUILD-DREAMCAST.md` (documentação completa do processo)
- `workspace/` (remoto, gitignored): Data.rsdk, cd-data, cd-root, ELF, **CDI**
- Logs: `workspace/{image-rebuild,engine-build,assets-build4,disc-build,flycast-boot*}.log`
