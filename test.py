#!/usr/bin/env python3

import requests

host = 'http://localhost:8000' 
path = '/set/int/asdf/0/{row}/{value}'

for row in range(0, 200):
    print('row = ', row)
    route = host + path.format(row=row, value=row)
    resp = requests.post(route)
    if resp.status_code != 200:
        raise Exception('bad status')
        
requests.post(host + '/sum/asdf/0/0/200/1003');
assert(requests.get(host + '/get/int/asdf/0/1003').content == b'19900')
