#!/bin/bash
git commit --amend -m "wip"
git push -f toomas@192.168.150.1:git $(git name-rev --name-only HEAD)
