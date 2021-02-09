make
cp *.nacp ./hacBrewPack/control/control.nacp
cp -r ./build/exefs ./hacBrewPack
cd ./hacBrewPack
.\hacbrewpack --romfsdir ../romfs --nspdir ../
del exefs /Q
rd exefs
del hacbrewpack_backup /Q
rd hacbrewpack_backup
cd ./control
del control.nacp
pause
