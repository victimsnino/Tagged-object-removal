from deeplab import DeepLabModel, label_to_color_image
import os
from io import BytesIO
import tarfile
import tempfile
from six.moves import urllib
import tensorflow as tf
from utils import run_visualization, transform_image, load_test, test_transform_image
import numpy as np
import time

# To activate env in VS Code .\.venv\Scripts\activate
#To run python main.py

LABEL_NAMES = np.asarray([
    'background', 'aeroplane', 'bicycle', 'bird', 'boat', 'bottle', 'bus',
    'car', 'cat', 'chair', 'cow', 'diningtable', 'dog', 'horse', 'motorbike',
    'person', 'pottedplant', 'sheep', 'sofa', 'train', 'tv'
])

FULL_LABEL_MAP = np.arange(len(LABEL_NAMES)).reshape(len(LABEL_NAMES), 1)
FULL_COLOR_MAP = label_to_color_image(FULL_LABEL_MAP)

MODEL_NAME = 'xception_coco_voctrainaug'
_DOWNLOAD_URL_PREFIX = 'http://download.tensorflow.org/models/'
_MODEL_URLS = {
    'mobilenetv2_coco_voctrainaug':
        'deeplabv3_mnv2_pascal_train_aug_2018_01_29.tar.gz',
    'mobilenetv2_coco_voctrainval':
        'deeplabv3_mnv2_pascal_trainval_2018_01_29.tar.gz',
    'xception_coco_voctrainaug':
        'deeplabv3_pascal_train_aug_2018_01_04.tar.gz',
    'xception_coco_voctrainval':
        'deeplabv3_pascal_trainval_2018_01_04.tar.gz',
}
_TARBALL_NAME = 'deeplab_model.tar.gz'

model_dir = tempfile.mkdtemp()
tf.gfile.MakeDirs(model_dir)

download_path = os.path.join(model_dir, _TARBALL_NAME)
print('downloading model, this might take a while...')
urllib.request.urlretrieve(_DOWNLOAD_URL_PREFIX + _MODEL_URLS[MODEL_NAME],
                   download_path)
print('download completed! loading DeepLab model...')

MODEL = DeepLabModel(download_path)
print('model loaded successfully!')

def run_experiments():
    URL ="./images/test-dataset/"
    test_data = load_test()
    start = time.time()
    for file_ in test_data.keys():
        SEG_IMG, resized_im = run_visualization(URL + file_, MODEL)
        tags = test_data[file_]
        test_transform_image(SEG_IMG, LABEL_NAMES, tags, URL + file_, resized_im)
    end = time.time()
    print("Time", end-start)

run_experiments()

# IMAGE_URL = './images/8399166846_f6fb4e4b8e_k.png'

# SEG_IMG, resized_im = run_visualization(IMAGE_URL, MODEL)

# TAG_TO_DELETE = "person"
# transform_image(SEG_IMG, LABEL_NAMES, TAG_TO_DELETE, IMAGE_URL, resized_im)
# load_test()