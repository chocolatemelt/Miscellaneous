from string import ascii_lowercase, ascii_uppercase

inf = open("p1", "r")

input_num = inf.readline()

for i in range(0,13):
    output = ''
    line = inf.readline().split()
    found = True
    for j in range(0,10):
        for string in line:
            if str(j) not in string:
                found = False
        if found:
            output += str(j)
        found = True
    found = True
    for c in ascii_uppercase:
        for string in line:
            if c not in string:
                found = False
        if found:
            output += c
        found = True
    found = True
    for c in ascii_lowercase:
        for string in line:
            if c not in string:
                found = False
        if found:
            output += c
        found = True
    print(output)
