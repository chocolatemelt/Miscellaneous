inf = open('p2', 'r')

row1 = []
row2 = inf.readline().split()

colordict = {
        'red':['red'],
        'blue':['blue'],
        'yellow':['yellow'],
        'purple':['red','blue'],
        'orange':['red','yellow'],
        'green':['blue','yellow'],
        'blank':[]
        }

last_color = 'blank'
output = ''
for i in range(0,len(row2)-1):
    c1 = colordict[row2[i]]
    c2 = colordict[row2[i+1]]
    common = []
    found = False
    for color in c1:
        if color in c2:
            common.append(color)
    for color in common:
        if color != last_color:
            output += color + ' '
            last_color = color
            found = True
            break;
    if not found:
        output += 'blank '
        last_color = 'blank'

print(output)
