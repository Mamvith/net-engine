#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

REMOTE="https://github.com/Mamvith/ada.git"

echo "=============================================="
echo " Push net-engine to GitHub (force overwrite)"
echo "=============================================="
echo "Target: $REMOTE"
echo "Local:  $(git log -1 --oneline)"
echo ""
echo "You need a Personal Access Token (PAT), NOT your GitHub password."
echo "Create one: https://github.com/settings/tokens"
echo "  -> Generate new token (classic) -> check 'repo' -> Generate"
echo ""

read -r -p "GitHub username [Mamvith]: " GH_USER
GH_USER="${GH_USER:-Mamvith}"
read -r -s -p "Paste PAT here (starts with ghp_ or github_pat_): " GH_TOKEN
echo ""

if [[ -z "${GH_TOKEN}" ]]; then
  echo "Error: token is empty. Create a PAT and paste it — do not press Enter without pasting."
  exit 1
fi

if [[ "${GH_TOKEN}" != ghp_* && "${GH_TOKEN}" != github_pat_* ]]; then
  echo "Error: that does not look like a GitHub token."
  echo "Use a PAT from https://github.com/settings/tokens (not your account password)."
  exit 1
fi

git config --global credential.helper store
printf 'https://%s:%s@github.com\n' "$GH_USER" "$GH_TOKEN" > ~/.git-credentials
chmod 600 ~/.git-credentials

git remote set-url origin "$REMOTE"

echo "Force pushing (no password prompts — using saved token)..."
export GIT_TERMINAL_PROMPT=0
if git push --force -u origin main; then
  echo ""
  echo "Success! Open: https://github.com/Mamvith/ada"
else
  echo ""
  echo "Push failed. Common fixes:"
  echo "  1. Create a NEW token with 'repo' scope"
  echo "  2. Revoke old tokens you may have leaked"
  echo "  3. Confirm repo exists: https://github.com/Mamvith/ada"
  exit 1
fi
