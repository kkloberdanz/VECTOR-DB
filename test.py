#!/usr/bin/env python3

import requests
from multiprocessing.pool import Pool as P
import multiprocessing

host = 'http://localhost:8000'
#host = 'http://homeflix.local:8000'
path = '/set/float/asdf/0/{row}/{value}'

MAX = 1000

def insert_num(row):
    route = host + path.format(row=row, value=row)
    resp = requests.post(route)
    if resp.status_code != 200:
        raise Exception('bad status')

with P(2 * multiprocessing.cpu_count()) as workers:
    list(workers.map(insert_num, range(0, MAX)))

requests.post(host + f'/sum/asdf/0/0/{MAX}/{MAX + 1}');
my_sum = float(requests.get(host + f'/get/float/asdf/0/{MAX + 1}').content)
print('sum is:', my_sum)
assert(my_sum == sum(float(x) for x in range(MAX)))
print('Ok')
