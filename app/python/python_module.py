import cv2
import requests
import numpy as np
import time

def send_image(picture, task_id, img_type):
    _, picture_encoded = cv2.imencode('.png', picture)
    picture_encoded = picture_encoded.tobytes()
    headers = {'TaskID' : task_id}
    response = requests.post(f'http://127.0.0.1:5000/send{img_type}', data=picture_encoded, headers = headers)

def send_image_to_lama(img, mask):
    task_id = str(time.time())
    send_image(img, task_id, 'Picture')
    send_image(mask, task_id, 'Mask')
    response = requests.get('http://127.0.0.1:5000/predictImage', headers = {'TaskID' : task_id})
    img_bytes = np.asarray(bytearray(response.content), dtype="uint8")
    return cv2.imdecode(img_bytes, cv2.IMREAD_COLOR)


def process_image(image):

    # temp solution to generate mask
    mask = 0 * np.ones(shape=image.shape[:2], dtype=np.uint8)
    cv2.rectangle(mask, pt1=(int(image.shape[0]),int(image.shape[1]/8)), 
                        pt2=(int(image.shape[0]/3),int(image.shape[1]/2)), 
                        color=(255,255,255), 
                        thickness=-1)

    cv2.imwrite("mask.jpg",mask)
    return send_image_to_lama(image, mask)


# if __name__ == "__main__":
#     img = cv2.imread("C:/Coding/Study/Tagged-object-removal/lama/000068.png")
#     cv2.imwrite("temp.jpg",process_image(img))
    