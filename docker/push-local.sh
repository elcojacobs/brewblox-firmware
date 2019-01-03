#! /usr/bin/env bash
set -e

# The build scripts generate :local tags of images
# This script retags and pushes those images
#
# First argument should always be the docker repo name
# Second argument is the tag name they should be pushed as
# Leave this blank to set it to the git branch name

CLEAN_BRANCH_NAME=$(echo "$(git rev-parse --abbrev-ref HEAD)" | tr '/' '-' | tr '[:upper:]' '[:lower:]');
REPO=$1
TAG=${2:-${CLEAN_BRANCH_NAME}}

docker tag ${REPO}:local ${REPO}:${TAG}
docker push ${REPO}:${TAG}
