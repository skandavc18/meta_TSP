import dataset 
import subprocess
import matplotlib.pyplot as plt
import sys

#data=dataset.generator(100,int(sys.argv[1]),1,1000).gen()
data=dataset.iterator("./datasets/tsp_dataset_10_"+sys.argv[1]+"_d.json").read()
def mat2list(dim, mat):
    l=[dim]
    for i in mat:
        l+=i
    
    l=list(map(str,l))

    l=' '.join(l)

    return l.encode()

def run(algo="alo"):


    meta_results=[]

    for i in data:
        process = subprocess.Popen(["./"+algo], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        s=mat2list(sys.argv[1],data[i][0])
        process.stdin.write(s)
        res=int(process.communicate()[0].strip())
        print(res,data[i][1])
        meta_results.append(abs(res-data[i][1])/data[i][1])
        process.stdin.close()
    

    return sum(meta_results)/len(meta_results)


print("alo")
v1=run("alo")
print("aco")
v2=run("aco.py")
print("gao")
v3=run("gao")
print("goa")
v4=run("goa")


"""plt.scatter(alo_x, alo_x)
plt.scatter(alo_x,alo_y)
plt.scatter(aco_x,aco_y)
plt.scatter(gao_x,gao_y)

plt.legend(['DP', 'ALO', 'ACO', 'GAO'], loc='upper left')
plt.title("Comparison of accuracy of various meta heuristic algorithms against Held Karp DP algorithm")
plt.xlabel("Total Distance Values")
plt.ylabel("Total Distance Values")
plt.savefig("accuracy.png")"""

f=open("accuracy_vals","a")
f.write(sys.argv[1]+","+str(v1)+","+str(v2)+","+str(v3)+","+str(v4)+"\n")
f.close()





