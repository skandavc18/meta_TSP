cd ../../GOA/GOA_Serial
g++ -o goa_s goa.cpp
mv goa_s ../'Performance Measure'/GOA
cd ../GOA_Parallel
make
mv goa ../../'Performance Measure'/GOA/goa_p
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