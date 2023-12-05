from sys import argv
import requests
import json

url = 'http://127.0.0.1:6767/report'
headers = {'Content-type': 'application/json',  # Определение типа данных
           'Accept': 'text/plain',
           'Content-Encoding': 'utf-8'}
data = {"dimensions": [argv[1], argv[2], argv[3]]}
answer = requests.post(url, data=json.dumps(data), headers=headers)
print(answer)
response = answer.json()
print(json.dumps(response, indent=4))
