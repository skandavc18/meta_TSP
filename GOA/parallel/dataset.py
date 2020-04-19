import json
import random
import dp
class generator:

    def __init__(self,sample_size=10,dim=10,a=1,b=100):
        self.sample_size=sample_size
        self.dim=dim
        self.a=a
        self.b=b
        file_name="tsp_dataset_"+str(sample_size)+"_"+str(dim)+"_d.json"
        self.file_name=file_name
        self.data={}

    def row_gen(self):
        graph=[[random.randint(self.a,self.b) for i in range(self.dim)] for j in range(self.dim)]
        for i in range(self.dim):
            graph[i][i]=0
        
        return graph 
    
    def gen(self):
        for i in range(self.sample_size):
            temp=self.row_gen()
            res=dp.tsp(self.dim,temp).solution()
            self.data[i]=[temp,int(res[0]),[int(j) for j in res[1]]]

        with open(self.file_name, 'w') as outfile:
            json.dump(self.data, outfile, sort_keys = True, indent = 4,
               ensure_ascii = True)
        
        return self.data

class iterator:

    def __init__(self,file_name="tsp_dataset_10_10_d.json"):
        self.file_name=file_name
        temp=file_name.split("_")
        self.sample_size=temp[2]
        self.dim=temp[3]

    def read(self):
        with open(self.file_name) as data_file:
            data_loaded = json.load(data_file)
        
        return data_loaded

if __name__=="__main__":
    import sys
    if len(sys.argv)==5:
        generator(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4]).gen()
    
    elif len(sys.argv)==4:
        generator(sys.argv[1],sys.argv[2],sys.argv[3]).gen()

    elif len(sys.argv)==3:
        generator(sys.argv[1],sys.argv[2]).gen()

    elif len(sys.argv)==2:
        generator(sys.argv[1]).gen()
    
    else:
        generator().gen()