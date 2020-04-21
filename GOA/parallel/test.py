import dataset 
import subprocess

data=dataset.iterator("tsp_dataset_10_10_d.json").read()

def mat2list(mat):
    l=[]
    for i in mat:
        l+=i
    
    l=list(map(str,l))
    #print(' '.join(l))
    return ' '.join(l).encode()

actual_results=[]

meta_results=[]

for i in data:
    process = subprocess.Popen(["./goa_p.exe"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    process.stdin.write(mat2list(data[i][0]))
    res=int(process.communicate()[0])
    meta_results.append(res)
    actual_results.append(data[i][1])
    process.stdin.close()

print("actual results ",actual_results)
print("GOA results ",meta_results)



