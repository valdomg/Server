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

'''URL principal, onde terá todas informações'''
@app.route("/")
def index():
    return rd("index.html")

'''Endpoint que o raspberry enviará as coordenadas do joystick'''
@app.route("/coordenadas")
def receber_coordenadas():
    global coordenada
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)

    
    coordenada = (x, y)
    #print(f"Recebido: x = {x}, y = {y}")
    socketio.emit('nova_coordenada', {'x': x, 'y': y})
    return "OK"

'''Endpoint para ler o estado dos botões'''
@app.route("/btn")
def btn_direito():

    a = request.args.get('a', type=int)
    b = request.args.get('b', type=int)
    socketio.emit('btn', {'a': a, 'b': b})

    return "Ok"
   

if __name__ == '__main__':
     socketio.run(app, host='0.0.0.0', port=5000, debug=True)