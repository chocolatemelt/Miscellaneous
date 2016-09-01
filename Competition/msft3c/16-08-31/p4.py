import networkx as nx
from networkx import *

def generate_graph(words):
    from string import ascii_lowercase as lowercase
    G = nx.Graph(name="words")
    lookup = dict((c,lowercase.index(c)) for c in lowercase)
    def edit_distance_one(word):
        for i in range(len(word)):
            left, c, right = word[0:i], word[i], word[i+1:]
            j = lookup[c] # lowercase.index(c)
            for cc in lowercase[j+1:]:
                yield left + cc + right
    candgen = ((word, cand) for word in sorted(words)
               for cand in edit_distance_one(word) if cand in words)
    G.add_nodes_from(words)
    for word, cand in candgen:
        G.add_edge(word, cand)
    return G


inf = open('p4','r')

input_num = inf.readline().split()
dict_num = input_num[0]
queries = input_num[1]
words = set()

for i in range(0, int(dict_num)):
    words.add(inf.readline().strip())

for i in range(0, int(queries)):
    line = inf.readline().split()
    start = line[0]
    end = line[1]
    if start not in words:
        print("-1")
    elif end not in words:
        print("-1")
    else:
        G = generate_graph(words)
        try:
            sp = shortest_path(G, start, end)
            print(len(sp)-1)
        except nx.NetworkXNoPath:
            print("-1")
