import matplotlib.pyplot as plt

f=open("times.csv")
data=f.readlines()
f.close()

cities=[]
alo_s=[]
alo_p=[]

for i in data[1:]:
    city,serial,parallel=list(map(float,i.strip().split(",")))
    cities.append(int(city))
    alo_s.append(serial*1000)
    alo_p.append(parallel*1000)




plt.plot(cities, alo_s)
plt.plot(cities,alo_p)


plt.legend(['Serial ALO', 'Parallel ALO' ] ,loc='upper left')
plt.title("Comparison of time taken by serial and parallel implementations of ALO")
plt.xlabel("No of Cities")
plt.ylabel("Time Taken in seconds")
plt.show()