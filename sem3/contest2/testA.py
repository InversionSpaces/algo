from random import randrange
from os import system
from filecmp import cmp
from pprint import pprint
import threading

def run_test():
    id = threading.get_ident()
    global stop
    for i in range(10 ** 10):
        if (stop): return
        if (i % 1000 == 0):
            print(str(id) + " " + str(i))
        n = randrange(2, 10)

        edges = dict()
        for i in range(randrange(1, n * (n - 1))):
            if len(edges) == 5: break
            a = randrange(1, n + 1)
            b = randrange(1, n + 1)
            if a == b or a == n or b == 1: continue

            c = randrange(1, 10)
            edges.update({(a, b) : c})

        infile = "in" + str(id)
        with open(infile, "w") as f:
            print(n, len(edges), file=f)
            for ((a, b), c) in edges.items():
                print(a, b, c, file=f)

        meoutfile = "meout" + str(id)
        cfoutfile = "cfout" + str(id)
        system("./me < {} > {}".format(infile, meoutfile))
        system("./cf < {} > {}".format(infile, cfoutfile))
        if (not cmp(meoutfile, cfoutfile)):
            print(id, " resulted")
            stop = True
            pprint(edges)
            break

stop = False
for i in range(30):
    t = threading.Thread(target=run_test)
    t.start()
