
# from deeplab import vis_segmentation as vis_segmentation
from PIL import Image
import numpy as np
import matplotlib as mplt

def run_visualization(url, MODEL):
  """Inferences DeepLab model and visualizes result."""
  try:
    original_im = Image.open(url)
  except IOError:
    print('Cannot retrieve image. Please check url: ' + url)
    return

  print('running deeplab on image %s...' % url)
  resized_im, seg_map = MODEL.run(original_im)
  print(seg_map.shape)

  # vis_segmentation(resized_im, seg_map)
  mplt.image.imsave('./images/segmentation.png', seg_map.astype(np.uint8))
  return seg_map

def transform_image(seg_img, LABEL_NAMES, selected_tag):
  tag_idx = list(LABEL_NAMES).index(selected_tag)
  if(tag_idx is not None):
    transform_img = (seg_img == tag_idx).astype(np.uint8)*255
    img_data = Image.fromarray(transform_img)
    img_data.save('./images/transform.png')

