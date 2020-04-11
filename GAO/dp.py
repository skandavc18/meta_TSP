import numpy as np
def get_subset_pair(snum):

	city = 0
	poly = 1
	tmp = snum
	res = []
	while tmp > 0:
		if tmp % (poly * 2) == poly:
			res.append((city + 1, snum - poly))
			tmp -= poly
		poly *= 2
		city += 1
			
	return res

def get_zero_citys(snum, n):

	city = 0
	poly = 1
	res = []
	while city < n:
		if snum % (poly * 2) == 0:
			res += [city + 1]
		else:
			snum -= poly
		poly *= 2
		city += 1
	return res






class tsp:
	def __init__(self, cityNum, dist):
		self.N = cityNum
		self.dist = dist 
		self.d = np.zeros((self.N, (2 ** (self.N-1)))) 
													
		self.mark = []
		for i in range(self.N):
			self.mark.append([])
			for j in range((2 ** (self.N-1))):
				self.mark[i].append([None])
		
		
		
	def solution(self):
		for i in range(1, self.N):
			self.d[i][0] = self.dist[i][0]
			
		for snum in range(1,2 ** (self.N - 1)):
			for city in get_zero_citys(snum, self.N - 1):
				pathList = [(ncity,nsnum,self.dist[city][ncity] + self.d[ncity][nsnum]) for (ncity, nsnum) in get_subset_pair(snum)]
				pathList.sort(key = lambda tup: tup[2])
				self.d[city][snum] = pathList[0][2]
				self.mark[city][snum] = (pathList[0][0], pathList[0][1])
			
			
		pathList = [(city,snum,self.dist[0][city] + self.d[city][snum])for (city, snum) in get_subset_pair(2 ** (self.N - 1) - 1)]
		pathList.sort(key = lambda tup: tup[2])
		self.d[0][-1] = pathList[0][2]
		self.mark[0][-1] = (pathList[0][0],pathList[0][1])
		

		path = []
		last = self.mark[0][-1]
		for i in range(self.N - 1):   
			current = (last[0])
			last = self.mark[last[0]][last[1]]
			path.append(current)
		path.append(0);last=0
		return (int(self.d[0][-1]),[int(i) for i in path])
			

cityNum = 10
dist = [[0,69,90,58,45,58,77,85,16,87],[24,0,61,48,71,61,72,3,34,8],[87,15,0,36,75,26,25,8,30,56],[22,60,49,0,17,95,98,76,91,91],[9,24,69,76,0,48,98,94,25,27],[53,90,83,34,14,0,52,62,16,91],[39,67,68,41,13,78,0,23,28,57],[15,79,67,100,32,77,51,0,5,97],[96,28,82,40,46,10,81,33,0,5],[52,44,11,87,31,61,97,32,15,0]]
	
tsp1 = tsp(cityNum, dist).solution()
print(tsp1)

