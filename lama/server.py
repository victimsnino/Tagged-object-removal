import os
from flask import Flask
import cv2
import numpy as np
from flask import request
from predictor import Predictor
from flask import send_file
import io

app = Flask(__name__)
predictor = Predictor(os.path.join(os.path.dirname(__file__), 'pretrained'))

pictures = dict()
masks = dict()

def data_to_img(data):
    nparr = np.fromstring(data, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    return img

@app.route("/sendPicture", methods = ["POST"])
def predict_image_picture():
    img = data_to_img(request.data)
    key = request.headers.get('TaskID')
    pictures[key] = img
    return "{key}_picture"

@app.route("/sendMask", methods = ["POST"])
def predict_image_mask():
    img = data_to_img(request.data)
    key = request.headers.get('TaskID')
    masks[key] = img
    return "{key}_mask"

@app.route("/predictImage", methods = ["GET"])
def predict_image():
    key = request.headers.get('TaskID')
    picture = pictures.get(key)
    mask = masks.get(key)
    if (picture is None) or (mask is None):
        return "Error"
    prediction = predictor.predict(picture, mask)
    _, prediction_encoded = cv2.imencode('.png', prediction)
    # prediction_encoded = picture_encoded.tostring()
    return send_file(
        io.BytesIO(prediction_encoded),
        mimetype='image/jpeg',
        as_attachment=True,
        attachment_filename='result.jpg')



if __name__ == "__main__":
    app.run(host = '127.0.0.1')