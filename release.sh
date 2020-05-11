VER=`date +"%y.%m%d-%H"`
cd /home/nextpcb/sensor
touch ./main/Version.h
idf.py build
cp ./build/sensor.bin  ./images/sensor-${VER}.bin
git add ./images/sensor-${VER}.bin
git commit -a -m "add new released sensor board binary: sensor-${VER}.bin"
git push
