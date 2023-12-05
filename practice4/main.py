import time
import json
import socket

from flask import Flask, request

HOST = "127.0.0.1"
PORT = "6767"


class _Socket():
    def __init__(self):
        self.host = '127.0.0.1'
        self.port = 6379

    def send_request(self, message: str) -> str:
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((self.host, self.port))
        self.client.send(message.encode(encoding='utf-8', errors='ignore'))
        response = self.client.recv(4096).decode(encoding='utf-8', errors='ignore').split('\n')
        self.client.close()
        return response[0]


class RemoteRepository():
    def __init__(self):
        self.socket = _Socket()
        self.key = "data"
        self.data = []
        self.id = 0
        self.get_id()
        self.load()

    def get_id(self):
        self.id = int(self.socket.send_request("--file stats --query HGET ID id").split(':')[1])

    def save_id(self):
        self.socket.send_request(f"--file stats --query HSET ID id {self.id}")

    def save(self, statistic: dict):
        data = f"{statistic['ip']}_{statistic['url']}_{statistic['time']}"
        self.data.append(data.split("_"))
        self.socket.send_request(f"--file stats --query HSET {self.id} {self.key} {data}")
        self.id += 1
        self.save_id()

    def load(self):
        for i in range(0, self.id):
            resp = self.socket.send_request(f"--file stats --query HGET {i} {self.key}").split(":")[1].split('_')
            self.data.append(resp)


class ReportGenerator():
    def __init__(self, repository: RemoteRepository):
        self.repository = repository

    def generate(self, dimensions: list):
        base = [{
            "URL": el[1],
            "SourceIP": el[0],
            "TimeInterval": el[2].replace('-', ':'),
            } for el in self.repository.data]
        d1, d2, d3 = dimensions
        data = dict()
        for d in base:
            p1, p2, p3 = d[d1], d[d2], d[d3]
            if p1 not in data.keys():
                data[p1] = {"Count": 1, p2: {"Count": 1, p3: 1}}
                continue
            if p2 not in data[p1].keys():
                data[p1][p2] = {"Count": 1, p3: 1}
                data[p1]["Count"] += 1
            elif p3 not in data[p1][p2].keys():
                data[p1]["Count"] += 1
                data[p1][p2]["Count"] += 1
                data[p1][p2][p3] = 1
            else:
                data[p1]["Count"] += 1
                data[p1][p2]["Count"] += 1
                data[p1][p2][p3] += 1
        return json.dumps(data, indent=2)


app = Flask(__name__)

repository = RemoteRepository()
report_generator = ReportGenerator(repository)


@app.route("/", methods=["POST"])
def save_statistic():
    statistic = {
        'ip': request.json["ip"],
        'url': request.json["url"],
        'time': time.strftime("%H-%M", time.gmtime(time.time()))
    }
    repository.save(statistic)
    return ""


@app.route("/report", methods=["POST"])
def generate_report():
    dimensions = request.json["dimensions"]
    report = report_generator.generate(dimensions)
    return report


if __name__ == "__main__":
    app.run(debug=True, host=HOST, port=PORT)
