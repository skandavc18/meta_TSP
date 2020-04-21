cd ../../ALO
g++ -o alo_s alo_serial.cpp
mv alo_s ../'Performance Measure'/ALO
cd parallel
make
mv alo_p ../../'Performance Measure'/ALO
cp defs.h ../../'Performance Measure'/ALO
cp alo.cl ../../'Performance Measure'/ALO
cp alo.c ../../'Performance Measure'/ALO
make clean
cd ../../'Performance Measure'/ALO
python3 test.py 200
rm alo_p
rm alo_s
rm defs.h
rm alo.cl
rm alo.c