cd ../../ALO/ALO_Serial
g++ -o alo_s alo.c
mv alo_s ../'Performance Measure'/ALO
cd ../ALO_Parallel
make
mv alo ../../'Performance Measure'/ALO/alo_p
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