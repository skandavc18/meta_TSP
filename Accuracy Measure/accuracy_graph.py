import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

f=open("accuracy_vals")
data=f.readlines()
f.close()

cities=[]
alo_l=[]
aco_l=[]
gao_l=[]
goa_l=[]

for i in data:
    city,alo,aco,gao,goa=list(map(float,i.strip().split(",")))
    cities.append(int(city))
    alo_l.append(float(alo)*100)
    aco_l.append(float(aco)*100)
    gao_l.append(float(gao)*100)
    goa_l.append(float(goa)*100)


fig = plt.figure()
plt.plot(cities,alo_l)
plt.plot(cities,aco_l)
plt.plot(cities,gao_l)
plt.plot(cities,goa_l)
plt.legend([ 'ALO', 'ACO', 'GAO', 'GOA'], loc='upper left')
plt.title("Accuracy Comparisons")
plt.xlabel("No of Cities")
plt.ylabel("% Error against DP ")
#plt.show()
fig.savefig('Error_Comparison.png')