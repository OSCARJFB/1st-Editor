import math as math
s = [10, 12, 13, 10, 12, 11.5]
print(s)
l = len(s)
m = 0
for i in s:
	m += i
m /= l

v = 0
su = 0
for i in s:	
	su += (i - m) * (i - m)
v = su / l
sd = math.sqrt(v)
print(sd)