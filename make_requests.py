# curl -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" -X POST http://localhost:8000

import requests
import sys


if sys.argv[1] == "POST":
    r = requests.post("http://localhost:" + sys.argv[2],
                      json={"key1": "value1", "key2": "value" + sys.argv[3]}
                      )
    z = r.json()
    print(z['key1'])
    print(r.json())
else:
    r = requests.get("http://localhost:" + sys.argv[2])
    print(r.text)

