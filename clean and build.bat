del *.nsp
make clean
make
cp *.nacp ./hacBrewPack/control/control.nacp
cp -r ./build/exefs ./hacBrewPack
cd ./hacBrewPack
.\PatchTool ./control/control.nacp nacp.ips ./control/control.nacp
.\hacbrewpack --romfsdir ../romfs --nspdir ../ --keygeneration 3
del exefs /Q
rd exefs
del hacbrewpack_backup /Q
rd hacbrewpack_backup
cd ./control
del control.nacp
pause
