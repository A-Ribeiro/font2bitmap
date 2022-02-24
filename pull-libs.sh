#! /bin/bash

cd libs/aRibeiroCore
git pull
./pull-submodules.sh

cd ../aRibeiroPlatform
git pull
./pull-submodules.sh

cd ../aRibeiroData
git pull
./pull-submodules.sh

cd ../aRibeiroWrappers
git pull
#./pull-submodules.sh
