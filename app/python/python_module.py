import cv2
import os
import tempfile
from six.moves import urllib
import tensorflow as tf
import requests
import numpy as np
import time
from deeplab import DeepLabModel
from PIL import Image


def init_model():
    global DEEPLAB_MODEL
    model_url = 'deeplabv3_pascal_train_aug_2018_01_04.tar.gz'
    download_url = 'http://download.tensorflow.org/models/'
    
    model_folder_name = 'deeplab_model.tar.gz'

    model_dir = tempfile.mkdtemp()
    tf.gfile.MakeDirs(model_dir)

    download_path = os.path.join(model_dir, model_folder_name)
    urllib.request.urlretrieve(download_url + model_url, download_path)

    DEEPLAB_MODEL = DeepLabModel(download_path)
    print('model loaded successfully!')



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


def process_image(list_of_tags, image):
    print(list_of_tags)
    global DEEPLAB_MODEL
    # convert cv2 image to PIL
    color_coverted = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    pil_img = Image.fromarray(color_coverted)
    # segmentation model do its work
    resized_im, seg_map = DEEPLAB_MODEL.run(pil_img)
    # mask segmentation map by selected tags
    label_names = np.asarray(['background', 'aeroplane', 'bicycle', 'bird', 'boat', 'bottle', 'bus',
    'car', 'cat', 'chair', 'cow', 'diningtable', 'dog', 'horse', 'motorbike',
    'person', 'pottedplant', 'sheep', 'sofa', 'train', 'tv'])
    tag_idxs = [list(label_names).index(tag_) for tag_ in list_of_tags]
    if(len(tag_idxs) != 0):
        transform_img = np.full(seg_map.shape, False)
        for tag_ in tag_idxs:
            transform_img = np.logical_or(transform_img, (seg_map == tag_).astype(np.uint8))

        transform_img = transform_img.astype(np.uint8)*255

        # resize mask to original size, because model gives smaller map
        mask_image = Image.fromarray(transform_img)
        mask_image = mask_image.resize(pil_img.size)
        # convert to cv2 image
        mask = cv2.cvtColor(np.array(mask_image), cv2.COLOR_RGB2BGR)
        cv2.imshow('image',mask)
        cv2.waitKey(0)

    # temp solution to generate mask
    # mask = 0 * np.ones(shape=image.shape[:2], dtype=np.uint8)
    # cv2.rectangle(mask, pt1=(int(image.shape[0]),int(image.shape[1]/8)), 
    #                     pt2=(int(image.shape[0]/3),int(image.shape[1]/2)), 
    #                     color=(255,255,255), 
    #                     thickness=-1)

    # cv2.imwrite("mask.jpg",mask)
    # return send_image_to_lama(image, mask)

# check how it works
# init_model()
# img = cv2.imread("IMG_URL")
# process_image(["person", "dog"], img)

def get_list_of_tags():
    return ["some_tag", "some_other_tag"]

# if __name__ == "__main__":
#     img = cv2.imread("C:/Coding/Study/Tagged-object-removal/lama/000068.png")
#     cv2.imwrite("temp.jpg",process_image(img))
    