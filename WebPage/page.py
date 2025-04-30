from flask import Flask, request
from flask import render_template as rd
import logging
'''
    SERVIDOR COM PYTHON UTILIZANDO O FRAMEWORK FLASK 
'''
app = Flask(__name__)

logging.basicConfig(level=logging.DEBUG)
coordenadas = []

@app.route("/")
def index():
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)
    coordenadas.append((x,y))

    print("X: ", x, "y: ", y)

    #if x is not None and y is not None:
     #   coordenadas.append(("Valor não encontrado", "Valor não encontrado")) 

    ultima = coordenadas[-2]
    print(ultima)

    return rd("index.html", ultima = ultima)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True, threaded= True)