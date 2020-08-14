from GoUtilsOpenFFIHost import *

# call foreign functions via OpenFFI

print("Calling Initials('James Tiberius Kirk'):")
res = Initials('James Tiberius Kirk')  # -- calling div
if res != 'JTK':
	print('Test failed. Expects JTK, Returned: {}'.format(res))
	exit(1)
print(res)
