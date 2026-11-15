#!/usr/bin/env bash
# ==============================================================================
# sync_github_showcase.sh (Opção B — Híbrida)
#
# Modos de operação:
#   1. Modo Diário (Padrão — sem argumentos):
#      Reproduz commit a commit todas as alterações novas do branch 'development'
#      para o branch 'github-showcase' (GitHub main), preservando autor, data/hora
#      original e mensagem de cada commit:
#        - Arquivos leves (C++, Python, Config, Scripts, Markdown) sobem completos.
#        - Binários pesados (.uasset, .umap, .fbx) NÃO sobem no dia a dia (0 MB LFS),
#          mas ficam registrados no corpo do commit para rastreabilidade.
#
#   2. Modo Marco de Entrega (--with-assets):
#      Além de sincronizar os commits, atualiza os binários filtrados (~3 GB) no
#      GitHub LFS quando você quiser publicar uma versão jogável atualizada.
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_DIR"

MODE="${1:-commits-only}"

STASHED=0

# Garantir retorno automático ao branch development e restauração de stash mesmo em caso de erro/interrupção
cleanup() {
    local cur
    cur=$(git branch --show-current 2>/dev/null || echo "")
    if [ -n "$cur" ] && [ "$cur" != "development" ]; then
        echo "🔙 Retornando para o branch 'development'..."
        git checkout -f development > /dev/null 2>&1 || true
    fi
    if [ "$STASHED" -eq 1 ]; then
        echo "📂 Restaurando alterações locais em andamento (git stash pop)..."
        git stash pop --quiet 2>/dev/null || true
        STASHED=0
    fi
}
trap cleanup EXIT

echo "========================================================"
if [ "$MODE" == "--with-assets" ]; then
    echo "  🚀 GitHub Showcase — Sincronização Completa (Commits + Assets)"
else
    echo "  ⚡ GitHub Showcase — Espelhamento de Commits (0 MB LFS)"
fi
echo "========================================================"

# 1. Garantir que estamos no branch development e guardar WIP se houver
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "development" ]; then
    git checkout -f development
fi

if [ -n "$(git status --porcelain)" ]; then
    echo "⏸️  Guardando alterações locais não commitadas temporariamente (git stash)..."
    git stash push -u -m "temp-showcase-sync" --quiet
    STASHED=1
fi

DEV_HEAD=$(git rev-parse development)

# 2. Atualizar referência do GitHub e preparar o branch github-showcase
echo "🔄 Buscando estado atual do GitHub (github/main)..."
git fetch github main

if git show-ref --verify --quiet refs/heads/github-showcase; then
    git checkout -f github-showcase > /dev/null 2>&1
    git reset --hard github/main > /dev/null 2>&1
else
    git checkout -b github-showcase github/main > /dev/null 2>&1
fi

# 3. Descobrir qual foi o último commit do 'development' sincronizado
#    (Fallback para d78fdda0, que foi o estado base sincronizado em 20/10/2026)
if [ -f .github_sync_commit ]; then
    LAST_SYNC=$(cat .github_sync_commit | tr -d '[:space:]')
else
    LAST_SYNC="d78fdda01d8946e5388a9e89f595979450d759d7"
fi

# Verificar se LAST_SYNC existe no histórico do development
if ! git cat-file -e "${LAST_SYNC}^{commit}" 2>/dev/null; then
    LAST_SYNC=$(git rev-parse "development~10" 2>/dev/null || git rev-parse development)
fi

COMMITS_TO_SYNC=$(git rev-list --reverse "${LAST_SYNC}..development")

if [ -z "$COMMITS_TO_SYNC" ] && [ "$MODE" != "--with-assets" ]; then
    echo "✅ Todos os commits do 'development' já estão sincronizados no GitHub!"
    exit 0
fi

# 4. Reproduzir cada commit individualmente preservando datas e autoria originais
COUNT=0
for COMMIT in $COMMITS_TO_SYNC; do
    COUNT=$((COUNT + 1))
    SHORT_SHA=$(git rev-parse --short "$COMMIT")
    AUTHOR_NAME=$(git show -s --format='%an' "$COMMIT")
    AUTHOR_EMAIL=$(git show -s --format='%ae' "$COMMIT")
    AUTHOR_DATE=$(git show -s --format='%aI' "$COMMIT")
    COMMIT_DATE=$(git show -s --format='%cI' "$COMMIT")
    ORIG_MSG=$(git show -s --format='%B' "$COMMIT")

    echo "  📝 [$COUNT] Espelhando $SHORT_SHA ($AUTHOR_DATE): $(git show -s --format='%s' "$COMMIT")"

    # Obter arquivos alterados no commit (comparando com o pai)
    CHANGED_FILES=$(git diff-tree --no-commit-id --name-status -r "$COMMIT")

    BINARY_LIST=""
    BIN_COUNT=0

    while IFS=$'\t' read -r STATUS FILEPATH REST; do
        [ -z "$STATUS" ] && continue
        # Tratar renames (R100 old new)
        if [[ "$STATUS" == R* ]]; then
            OLD_PATH="$FILEPATH"
            FILEPATH="$REST"
        fi

        # Ignorar binários pesados / LFS no espelhamento diário de commits
        if [[ "$FILEPATH" =~ \.(uasset|umap|zip|fbx|FBX|png|PNG|jpg|jpeg|TGA|tga|bak)$ ]]; then
            BIN_COUNT=$((BIN_COUNT + 1))
            if [ "$BIN_COUNT" -le 15 ]; then
                BINARY_LIST="${BINARY_LIST}"$'\n'"  - [${STATUS:0:1}] ${FILEPATH}"
            fi
        else
            # Arquivo leve (C++, Python, Config, Markdown, Script, etc.)
            if [ "$STATUS" == "D" ]; then
                git rm --cached -f --ignore-unmatch "$FILEPATH" > /dev/null 2>&1 || true
                rm -f "$FILEPATH"
            else
                if [[ "$STATUS" == R* ]] && [ -n "$OLD_PATH" ]; then
                    git rm --cached -f --ignore-unmatch "$OLD_PATH" > /dev/null 2>&1 || true
                    rm -f "$OLD_PATH"
                fi
                git checkout "$COMMIT" -- "$FILEPATH" 2>/dev/null || true
                git add -f "$FILEPATH" 2>/dev/null || true
            fi
        fi
    done <<< "$CHANGED_FILES"

    if [ "$BIN_COUNT" -gt 15 ]; then
        EXTRA=$((BIN_COUNT - 15))
        BINARY_LIST="${BINARY_LIST}"$'\n'"  - ... e mais ${EXTRA} arquivo(s) binário(s) UE5"
    fi

    # Registrar o commit sincronizado
    echo "$COMMIT" > .github_sync_commit
    git add -f .github_sync_commit

    # Montar mensagem preservando a original + metadados dos assets do Azure
    if [ "$BIN_COUNT" -gt 0 ]; then
        FULL_MSG="${ORIG_MSG}

[Azure DevOps Sync: ${SHORT_SHA}]
Assets binários UE5 atualizados no Azure (${BIN_COUNT} arquivo(s)):${BINARY_LIST}"
    else
        FULL_MSG="${ORIG_MSG}

[Azure DevOps Sync: ${SHORT_SHA}]"
    fi

    GIT_AUTHOR_NAME="$AUTHOR_NAME" \
    GIT_AUTHOR_EMAIL="$AUTHOR_EMAIL" \
    GIT_AUTHOR_DATE="$AUTHOR_DATE" \
    GIT_COMMITTER_NAME="$AUTHOR_NAME" \
    GIT_COMMITTER_EMAIL="$AUTHOR_EMAIL" \
    GIT_COMMITTER_DATE="$COMMIT_DATE" \
    git commit --allow-empty -m "$FULL_MSG" > /dev/null
done

# 5. Se for modo --with-assets, atualizar também os binários filtrados (~3 GB)
if [ "$MODE" == "--with-assets" ]; then
    echo "📦 Modo --with-assets ativado: atualizando assets binários filtrados..."
    SAVED_README=""
    [ -f README.md ] && SAVED_README=$(cat README.md)

    git read-tree --reset -u development
    echo "$DEV_HEAD" > .github_sync_commit
    git add -f .github_sync_commit

    UNUSED_DIRS=(
        "Content/Sirv/Static/GenericMaterials/Tiles"
        "Content/Sirv/Static/GenericMaterials/Ground"
        "Content/Sirv/Static/GenericMaterials/Masters"
        "Content/Sirv/Static/GenericMaterials/00_MastersOpt"
        "Content/Sirv/Static/GenericMaterials/Concrete"
        "Content/Sirv/Static/GenericMaterials/Grass"
        "Content/Sirv/Static/GenericMaterials/RoofBitumen"
        "Content/Sirv/Static/GenericMaterials/Brick"
        "Content/Sirv/Static/GenericMaterials/Gutters_Curbs"
        "Content/Sirv/Static/GenericMaterials/RoadStencil"
        "Content/Sirv/Static/GenericMaterials/LaneMarking"
        "Content/Sirv/Static/GenericMaterials/Water"
        "Content/Sirv/Static/GenericMaterials/WetPavement"
        "Content/Sirv/Static/GenericMaterials/CheapMaterials"
        "Content/Sirv/Static/GenericMaterials/Generic_Rock_Material"
        "Content/Sirv/Static/GenericMaterials/Licenseplates/Textures"
        "Content/Sirv/Static/Rain_material"
        "Content/Sirv/Static/ConstructionSite/Demo"
        "Content/Sirv/Static/ConstructionSite/Environment/Bulldozer_01"
        "Content/Sirv/Static/ConstructionSite/Environment/MetalContainer_01"
        "Content/Sirv/Static/ConstructionSite/Environment/RoadBarrier_01"
        "Content/Sirv/Static/ConstructionSite/Environment/TileGround_01"
        "Content/Sirv/Static/ConstructionSite/Environment/Sky_01"
        "Content/Sirv/Static/ConstructionSite/Environment/Ruble_03"
        "Content/Sirv/Static/ConstructionSite/Environment/Cobble_01"
        "Content/Sirv/Static/ConstructionSite/Environment/Ground_01"
        "Content/Sirv/Static/ConstructionSite/Environment/Barrel_01"
        "Content/Sirv/Static/ConstructionSite/Environment/BlockBrick_01"
        "Content/Sirv/Static/ConstructionSite/Environment/SafetyHelmet_01"
        "Content/Sirv/Static/props/Meshes/Textures"
        "Content/Sirv/Blueprints/Vehicles/CAT7295"
        "Content/Sirv/Static/GenericMaterials/Asphalt/M_GTARoad"
        "Content/Sirv/Static/GenericMaterials/Asphalt/M_demoRoad"
    )
    git rm --cached -r --ignore-unmatch "${UNUSED_DIRS[@]}" > /dev/null

    git ls-files "Content/Sirv/Static/GenericMaterials/Asphalt/" | while read -r f; do
        base=$(basename "$f")
        case "$base" in
            "1.uasset"|"2.uasset"|"3.uasset"|"T_Asphalt02_d.uasset") ;;
            *) git rm --cached --ignore-unmatch "$f" > /dev/null ;;
        esac
    done

    if [ -n "$SAVED_README" ]; then
        echo "$SAVED_README" > README.md
        git add README.md
    fi

    if ! git diff --cached --quiet; then
        git commit -m "chore(showcase): sincroniza pacote de assets binarios filtrados do marco atual"
    fi
fi

# 6. Enviar para o GitHub
echo "☁️  Enviando $COUNT commit(s) para o GitHub (github/main)..."
git push github github-showcase:main

echo "✅ GitHub Showcase 100% sincronizado!"
