#!/bin/python3
#quickly convert json string to format compatible with c array

jsonStr = r'"array":["test","one",0,5]'

for l in range(len(jsonStr)):
    print("'" + jsonStr[l] + "'", end='')
    if l < len(jsonStr)-1:
        print(", ", end='')

print()
