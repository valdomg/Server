from flask import Flask, request
from flask import render_template as rd
from flask_socketio import SocketIO, emit
import logging
'''
    SERVIDOR COM PYTHON UTILIZANDO O FRAMEWORK FLASK 
'''
app = Flask(__name__)

logging.basicConfig(level=logging.DEBUG)
socketio = SocketIO(app)

coordenadas = (0,0)

@app.route("/")
def index():
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)

    #print("X: ", x, "y: ", y)

    return rd("index.html")


@app.route("/coordenadas")
def receber_coordenadas():
    global coordenada
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)

    
    coordenada = (x, y)
    #print(f"Recebido: x = {x}, y = {y}")
    socketio.emit('nova_coordenada', {'x': x, 'y': y})
    return "OK"


if __name__ == '__main__':
     socketio.run(app, host='0.0.0.0', port=5000, debug=True)