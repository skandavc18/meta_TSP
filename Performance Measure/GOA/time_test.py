import matplotlib.pyplot as plt

f=open("times.csv")
data=f.readlines()
f.close()

cities=[]
goa_s=[]
goa_p=[]
speedup=[]

for i in data[1:]:
    city,serial,parallel=list(map(float,i.strip().split(",")))
    cities.append(int(city))
    goa_s.append(serial)
    goa_p.append(parallel)
    speedup.append(serial/parallel)




fig = plt.figure()
#plt.plot(cities, speedup)
plt.plot(cities, goa_s)
plt.plot(cities,goa_p)


plt.legend(['Serial GOA', 'Parallel GOA' ] ,loc='upper left')
plt.title("Comparison of time taken by serial and parallel implementations of GOA")
plt.xlabel("No of Cities")
plt.ylabel("Time Taken in seconds")
#plt.show()
fig.savefig("Performance.png")