# curl -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" -X POST http://localhost:8000
import json

import requests

r = requests.post("http://localhost:5555",
                  json={"key1":"value1", "key2":"value2"}
                  # data=json.dumps({"key1":"value1", "key2":"value2"})
                  )
# r = requests.get("http://localhost:5555")

z = r.json()

print(z['key1'])
print(r.json())