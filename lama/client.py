import cv2
import PIL.Image as Image
import requests
import io

def send_image(picture, task_id, img_type):
    _, picture_encoded = cv2.imencode('.png', picture)
    picture_encoded = picture_encoded.tobytes()
    headers = {'TaskID' : task_id}
    response = requests.post(f'http://127.0.0.1:5000/send{img_type}', data=picture_encoded, headers = headers)
    # try:
    #     data = response.json()
    #     print(data)
    # except Exception as e:
    #     print(e)
    #     print("Suck")

if __name__ == "__main__":
    task_id = '123'
    img_path = "000068.png"
    img = cv2.imread(img_path)
    # img = cv2.resize(img, (1000, 1504))
    send_image(img, task_id, 'Picture')
    mask_path = "000068_mask.png"
    mask = cv2.imread(mask_path)
    # mask = cv2.resize(mask, (1000, 1504))
    send_image(mask, task_id, 'Mask')
    response = requests.get('http://127.0.0.1:5000/predictImage', headers = {'TaskID' : task_id})
    f = io.BytesIO(response.content)
    img = Image.open(f)
    print(img.size)
    img.save('./client_img.png')