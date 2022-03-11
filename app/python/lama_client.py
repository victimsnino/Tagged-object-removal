import cv2
import requests
import time
import numpy as np

def __send_image(picture, task_id, img_type):
    _, picture_encoded = cv2.imencode('.jpg', picture)
    picture_encoded = picture_encoded.tobytes()
    headers = {'TaskID' : task_id}
    response = requests.post(f'http://127.0.0.1:5000/send{img_type}', data=picture_encoded, headers = headers)

def send_image_to_lama(img, mask):
    task_id = str(time.time())
    __send_image(img, task_id, 'Picture')
    __send_image(mask, task_id, 'Mask')
    response = requests.get('http://127.0.0.1:5000/predictImage', headers = {'TaskID' : task_id})
    img_bytes = np.asarray(bytearray(response.content), dtype="uint8")
    return cv2.imdecode(img_bytes, cv2.IMREAD_COLOR)
