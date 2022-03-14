import cv2
import os
import tarfile
import numpy as np
from PIL import Image

import tensorflow as tf
class DeepLabModel(object):
  """Class to load deeplab model and run inference."""

  INPUT_TENSOR_NAME = 'ImageTensor:0'
  OUTPUT_TENSOR_NAME = 'SemanticPredictions:0'
  INPUT_SIZE = 513
  FROZEN_GRAPH_NAME = 'frozen_inference_graph'

  def __init__(self, tarball_path):
    self.graph = tf.Graph()

    graph_def = None
    # Extract frozen graph from tar archive.
    tar_file = tarfile.open(tarball_path)
    for tar_info in tar_file.getmembers():
      if self.FROZEN_GRAPH_NAME in os.path.basename(tar_info.name):
        file_handle = tar_file.extractfile(tar_info)
        graph_def = tf.compat.v1.GraphDef().FromString(file_handle.read())
        break

    tar_file.close()

    if graph_def is None:
      raise RuntimeError('Cannot find inference graph in tar archive.')

    with self.graph.as_default():
      tf.import_graph_def(graph_def, name='')

    self.sess = tf.compat.v1.Session(graph=self.graph)

  def run(self, image):
    width, height = image.size
    resize_ratio = 1.0 * self.INPUT_SIZE / max(width, height)
    target_size = (int(resize_ratio * width), int(resize_ratio * height))
    resized_image = image.convert('RGB').resize(target_size, Image.ANTIALIAS)
    batch_seg_map = self.sess.run(
        self.OUTPUT_TENSOR_NAME,
        feed_dict={self.INPUT_TENSOR_NAME: [np.asarray(resized_image)]})
    seg_map = batch_seg_map[0]
    return resized_image, seg_map

def get_available_tags():
  return ['aeroplane', 'bicycle', 'bird', 'boat', 'bottle', 'bus',
  'car', 'cat', 'chair', 'cow', 'diningtable', 'dog', 'horse', 'motorbike',
  'person', 'pottedplant', 'sheep', 'sofa', 'train', 'tv']

def create_model():
  path_to_weights = os.path.join(os.path.dirname(os.path.abspath(__file__)), "deeplab_model", "model.gz")
  if not os.path.exists(path_to_weights):
    raise FileNotFoundError(f"Can't find file with model weights by path {path_to_weights}")
  return DeepLabModel(path_to_weights)

def get_mask_for_image(model, list_of_tags, image):
  pil_img = Image.fromarray(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))

  # segmentation model do its work
  _, seg_map = model.run(pil_img)
  # mask segmentation map by selected tags
  label_names = ['background'] + get_available_tags()
  tag_idxs = [label_names.index(tag_) for tag_ in list_of_tags]
  transform_img = np.full(seg_map.shape, False)
  for tag_ in tag_idxs:
      transform_img = np.logical_or(transform_img, (seg_map == tag_).astype(np.uint8))

  transform_img = transform_img.astype(np.uint8)*255

  # resize mask to original size, because model gives smaller map
  mask_image = Image.fromarray(transform_img)
  mask_image = mask_image.resize(pil_img.size)
  # convert to cv2 image
  res = cv2.cvtColor(np.array(mask_image), cv2.COLOR_RGB2BGR)
  return cv2.dilate(res, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (40, 40)))