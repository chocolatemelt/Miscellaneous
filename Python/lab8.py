#!/usr/bin/env python3
# Two dynamic programming problems, second one more interesting than the other.
# Basically:
# Along a highway you can open up a McDick's at D distances, where each distance returns
# P profits (This is the first and second array). You can't open a McDick's within k
# distance away from each other; what is the maximum expected profit?

def longest_sub(a):
    dp = [ 1 for l in a ]
    for x in range(len(a)):
        for y in range(x):
            if(a[y] <= a[x]):
                if(dp[x] < dp[y] + 1):
                    dp[x] = dp[y] + 1
    return dp[-1]

def max_profit(k, d, p):
    l = [ i for i in p ]
    for i in range(1, len(l)):
        l[i] = 0
    for i in range(2, len(l)):
        for j in range(0, i):
            tmp = l[j] + a(d[i], d[j]) * p[i]
            if(tmp > l[i]):
                l[i] = tmp
        if(l[i] < p[i]):
            l[i] = p[i]
    return l[-1]

def a(d1, d2):
    if(abs(d1 - d2) >= 6):
        return 1
    else:
        return 0


if __name__ == '__main__':
    print('longest nondecreasing subsequence')
    print(longest_sub([75, 15, -30, 10, -5, 40, 10]))
    print('max expected profit')
    print(max_profit(6, [5,6,7,12,16,46,145], [156,143,112,90,127,55,21]))
