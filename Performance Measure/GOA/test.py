import dataset 
import subprocess
import csv
import time
import sys

if len(sys.argv)==1:
    dim=10
else:
    dim=int(sys.argv[1])

def mat2list(mat):
    l=[]
    for i in mat:
        l+=i
    
    l=list(map(str,l))

    l=' '.join(l)
    
    return l.encode()

def run(algo="goa",data={}):

    time_taken=[]

    time_s = time.time()
    for i in data:
        start_time=time.perf_counter()
        process = subprocess.Popen(["./"+algo], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        process.stdin.write(mat2list(data[i]))
        end_time=time.perf_counter()
        res=process.communicate()[0].strip()
        print(res)
        time_taken.append(abs(start_time-end_time))
        process.stdin.close()
    
    #print(time_taken)
    #print(time.time()-time_s)
    #return sum(time_taken)/len(time_taken)
    return (time.time()-time_s)/len(data)

obj=dataset.generator(1,dim,1,1000)
data=obj.input_gen()
#print(mat2list(data[0]))

print("hi")
goa_p=run("goa_p",data)
print("hi")
#goa_s=run("goa_s",data)
#print("hi")
with open('times.csv', 'a', newline='') as file:
    writer = csv.writer(file)
    #writer.writerow([str(dim),str(goa_s),str(goa_p)])
    writer.writerow([str(dim),str(goa_p)])
