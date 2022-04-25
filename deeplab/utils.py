
# from deeplab import vis_segmentation as vis_segmentation
from PIL import Image
import numpy as np
import matplotlib as mplt
from matplotlib import pyplot as plt
import json

def run_visualization(url, MODEL):
  """Inferences DeepLab model and visualizes result."""
  try:
    original_im = Image.open(url)
  except IOError:
    print('Cannot retrieve image. Please check url: ' + url)
    return

  # print('running deeplab on image %s...' % url)
 
  resized_im, seg_map = MODEL.run(original_im)

  return seg_map, resized_im


def load_test():
  f = open("./images/test-dataset/tags.json")
  data = json.load(f)
  return data


def test_transform_image(seg_img, LABEL_NAMES, selected_tags, img_url, resized_im):
  image_name = img_url.split("/")[-1].split(".")[0]

  tag_idxs = [list(LABEL_NAMES).index(tag_) for tag_ in selected_tags]
  if(len(tag_idxs) != 0):
    transform_img = np.full(seg_img.shape, False)
    for tag_ in tag_idxs:
      transform_img = np.logical_or(transform_img, (seg_img == tag_).astype(np.uint8))

    transform_img = transform_img.astype(np.uint8)*255
    img = Image.open(img_url)

    mask_image = Image.fromarray(transform_img)
    mask_image = mask_image.resize(img.size)
    # mask_image.save("./images/test-dataset/segmented/" + image_name + "-segmentation.png")

    # plt.imshow(img)
    # plt.imshow(mask_image, alpha=0.7)
    # plt.axis('off')
    # plt.savefig("./images/test-dataset/segmented/" + image_name + "-overlay.png")


def transform_image(seg_img, LABEL_NAMES, selected_tag, img_url, resized_im):
  image_name = img_url.split("/")[-1].split(".")[0]
  plt.imshow(resized_im)
  plt.imshow(seg_img, alpha=0.7)
  plt.axis('off')
  plt.savefig("./images/segmented/" + image_name + "-overlay-all.png")

  tag_idx = list(LABEL_NAMES).index(selected_tag)
  if(tag_idx is not None):
    transform_img = (seg_img == tag_idx).astype(np.uint8)*255
    img = Image.open(img_url)

    mask_image = Image.fromarray(transform_img)
    mask_image = mask_image.resize(img.size)
    mask_image.save("./images/segmented/" + image_name + "-segmentation.png")
    
    
    plt.imshow(img)
    plt.imshow(mask_image, alpha=0.7)
    plt.axis('off')
    plt.savefig("./images/segmented/" + image_name + "-overlay.png")
    
