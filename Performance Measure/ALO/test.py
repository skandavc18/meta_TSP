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

def run(algo="alo",data={}):

    time_taken=[]

    for i in data:
        start_time=time.perf_counter()
        process = subprocess.Popen(["./"+algo], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        process.stdin.write(mat2list(data[i]))
        end_time=time.perf_counter()
        res=process.communicate()[0].strip()
        time_taken.append(abs(start_time-end_time))
        process.stdin.close()

    return sum(time_taken)/len(time_taken)


obj=dataset.generator(10,dim,1,1000)
data=obj.input_gen()
alo_p=run("alo_p",data)
alo_s=run("alo_s",data)
with open('times.csv', 'a', newline='') as file:
    writer = csv.writer(file)
    writer.writerow([str(alo_s),str(alo_p)])









