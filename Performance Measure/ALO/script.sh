cd ../../GOA
g++ -o goa_s goa_serial.cpp
mv goa_s ../'Performance Measure'/GOA
cd parallel
make
mv goa_p ../../'Performance Measure'/GOA
cp defs.h ../../'Performance Measure'/GOA
cp goa.cl ../../'Performance Measure'/GOA
cp goa.c ../../'Performance Measure'/GOA
make clean
cd ../../'Performance Measure'/GOA
python3 test.py 200
rm goa_p
rm goa_s
rm defs.h
rm goa.cl
rm goa.c