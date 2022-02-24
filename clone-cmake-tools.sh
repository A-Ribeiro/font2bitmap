#! /bin/bash

#rm -rf libs/aRibeiroCore/cmake libs/aRibeiroCore/cmake-modules/
#rm -rf libs/aRibeiroPlatform/cmake libs/aRibeiroPlatform/cmake-modules/
#rm -rf libs/aRibeiroData/cmake libs/aRibeiroData/cmake-modules/

#cp -r cmake libs/aRibeiroCore
#cp -r cmake libs/aRibeiroPlatform
#cp -r cmake libs/aRibeiroData

#cp -r cmake-modules libs/aRibeiroCore
#cp -r cmake-modules libs/aRibeiroPlatform
#cp -r cmake-modules libs/aRibeiroData

git add cmake
git commit -m 'script update'
git push

cd libs/aRibeiroCore
git add cmake
git commit -m 'script update'
git push

cd ../aRibeiroPlatform
git add cmake
git commit -m 'script update'
git push

cd ../aRibeiroData
git add cmake
git commit -m 'script update'
git push

