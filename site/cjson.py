#!/bin/python3
#quickly convert json string to format compatible with c array

jsonStr = r'{"test":true,"hello":"eh","array":["fsdf",6,"ddd",false],"numberherer":6,"bewiobejct":{"string":"fds","number2":2,"lists":[false,false]}}'

for l in range(len(jsonStr)):
    print("'" + jsonStr[l] + "'", end='')
    if l < len(jsonStr)-1:
        print(", ", end='')

print()
