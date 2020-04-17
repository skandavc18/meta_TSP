g++ alo.cpp -o alo 
g++ gao.cpp -o gao 
g++ goa.cpp -o goa

for i in {10..15}
do
    python.exe dataset.py 10 $i
    python3 accuracy_test.py $i
done

python3 accuracy_graph.py
rm goa gao alo accuracy_vals