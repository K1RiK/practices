import os
import socket
from flask import Flask, redirect, request


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


class UrlShortener:

    def __init__(self, database_filename: str = 'links'):
        self.db_filename = database_filename
        self.id = 0
        self.client = _Socket()

    def shorten_url(self, full_url):
        short_url = self.client.send_request(f"--file {self.db_filename} --query HGET furl {full_url}")[5::]
        if short_url != '(null)' and 'link' in short_url:
            return short_url
        short_url = f'link{self.id}'
        self.id += 1
        self.save_data(full_url, short_url)
        return short_url

    def get_full_url(self, short_url):
        response = self.client.send_request(f"--file {self.db_filename} --query HGET links {short_url}")
        print(response)
        return response[5::]

    def save_data(self, full_url, short_url):
        self.client.send_request(f"--file {self.db_filename} --query HSET links {short_url} {full_url}")
        self.client.send_request(f"--file {self.db_filename} --query HSET furl {full_url} {short_url}")

    # def read_data(self):
    #     response = self.client.send_request(f"--file {self.db_filename} --query HGET links {short_url}")
    #     print(response)


app = Flask(__name__)

shortener = UrlShortener()


@app.route('/', methods=['POST'])
def shorten_url_endpoint():
    full_url = request.get_data().decode(encoding='utf-8', errors='ignore')
    return shortener.shorten_url(full_url)


@app.route("/", methods=["GET"])
def index():
    html = '<html style="font-size: 200%; font-family: Verdana, Arial, sans-serif;">'
    return f'{html}<h3>Сервис для сокращения ссылок<br>Для сокращения ссылки пользуйтесь POST запросом</h3>'


@app.route('/<short_url>', methods=['GET'])
def redirect_endpoint(short_url):
    full_url = shortener.get_full_url(short_url).replace('\n', '').strip()
    print(f'{full_url=}')

    if full_url == '(null)':
        return redirect('/')

    if len(full_url.split('://')) == 1:
        return redirect(f'https://{full_url}')
    return redirect(full_url)


if __name__ == "__main__":
    app.run(debug=True, host="127.0.0.1", port="5000")
