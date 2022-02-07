
# from deeplab import vis_segmentation as vis_segmentation
from PIL import Image
import numpy as np
import matplotlib as mplt
from matplotlib import pyplot as plt

def run_visualization(url, MODEL):
  """Inferences DeepLab model and visualizes result."""
  try:
    original_im = Image.open(url)
  except IOError:
    print('Cannot retrieve image. Please check url: ' + url)
    return

  print('running deeplab on image %s...' % url)
 
  resized_im, seg_map = MODEL.run(original_im)

  return seg_map

def transform_image(seg_img, LABEL_NAMES, selected_tag, img_url):
  tag_idx = list(LABEL_NAMES).index(selected_tag)
  if(tag_idx is not None):
    transform_img = (seg_img == tag_idx).astype(np.uint8)*255

    mask_image = Image.fromarray(transform_img)
    mask_image = mask_image.resize((1499, 1083))
    image_name = img_url.split("/")[-1].split(".")[0]
    mask_image.save("./images/segmented/" + image_name + "-segmentation.png")
    
    img = Image.open(img_url)
    plt.imshow(img)
    plt.imshow(mask_image, alpha=0.7)
    plt.axis('off')
    plt.savefig("./images/segmented/" + image_name + "-overlay.png")
    
