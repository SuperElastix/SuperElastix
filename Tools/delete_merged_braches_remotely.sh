#!/usr/bin/env bash

# - Get all branches from remote
# - Make all up-to-date
# - Scan which branches have been merged into develop
# - Prompt [y/N] for each of those branches to delete remotely and locally
# - Prune all branches locally



# make all branches locally that track the remote branches. http://stackoverflow.com/questions/10312521/how-to-fetch-all-git-branches
for b in `git branch -r | grep -v -- '->'`; do git branch --track ${b##origin/} $b; done

# make everything up-to-date locally
git fetch --all
git pull --all

git checkout develop
for b in `git branch --merged | grep -v -- '* '`; do 
  read -r -p "Delete branch $b? [y/N] " response
  if [[ "$response" =~ ^([yY][eE][sS]|[yY])+$ ]]
  then
    git push origin --delete $b; 
    git branch -d $b;
  fi
done

git fetch --all --prune

