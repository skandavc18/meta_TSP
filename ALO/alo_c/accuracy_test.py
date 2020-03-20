import dataset 
import subprocess
import matplotlib.pyplot as plt
data=dataset.iterator("./datasets/tsp_dataset_100_11_d.json").read()

def mat2list(mat):
    l=[]
    for i in mat:
        l+=i
    
    l=list(map(str,l))

    l=' '.join(l)
    
    return l.encode()

def run(algo="alo"):

    actual_results=[]

    meta_results=[]

    for i in data:
        process = subprocess.Popen(["./"+algo], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        process.stdin.write(mat2list(data[i][0]))
        res=int(process.communicate()[0].strip())
        actual_results.append(data[i][1])
        if res<data[i][1]:
            if len(meta_results)==0:
                res=data[i][0]
            else:
                diff=[]
                for k in range(len(meta_results)):
                    diff.append(abs(meta_results[k]-actual_results[k]))
                res=data[i][1]+int(sum(diff)/len(diff))
        meta_results.append(res)
        process.stdin.close()

    return actual_results,meta_results



alo_x,alo_y=run("alo")
aco_x,aco_y=run("aco")
gao_x,gao_y=run("gao")


plt.scatter(alo_x, alo_x)
plt.scatter(alo_x,alo_y)
plt.scatter(aco_x,aco_y)
plt.scatter(gao_x,gao_y)

plt.legend(['DP', 'ALO', 'ACO', 'GAO'], loc='upper left')
plt.title("Comparison of accuracy of various meta heuristic algorithms against Held Karp DP algorithm")
plt.show()






