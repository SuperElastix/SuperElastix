#!/usr/bin/env bash

# Current work directory must be the root of source, i.e. the local git repo. 

GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
echo $GIT_BRANCH
if [ "$GIT_BRANCH" = "SELX-172-Deploy-develop-on-shark" ]
then
  echo "Deploy this build of develop on shark cluster"
  scp -r src sa_lkeb@shark:~/SuperElastix
else
  echo "This is not the develop branch, thus do not deploy"
fi
