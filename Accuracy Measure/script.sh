cd ../ALO/ALO_Serial
g++ alo.c -o alo 
mv alo ../../'Accuracy Measure'
cd ../../GOA/GOA_Serial
g++ goa.cpp -o goa
mv goa ../../'Accuracy Measure'
cd ../../'Accuracy Measure'
g++ gao.cpp -o gao 

for i in {10..10}
do
    python.exe dataset.py 10 $i
    python3 accuracy_test.py $i
done

python3 accuracy_graph.py
rm goa gao alo accuracy_vals