import cv2
import os
from lama_client import send_image_to_lama
from deeplab import get_available_tags, create_model, get_mask_for_image


model = create_model()
print('deeplab model loaded successfully!')

def get_list_of_tags():
    return get_available_tags()

def process_image(list_of_tags, image):
    if len(list_of_tags) == 0:
        return image

    mask = get_mask_for_image(model, list_of_tags, image)
    cv2.imwrite(os.path.join(os.path.dirname(os.path.abspath(__file__)), "mask.jpg") ,mask)
    return send_image_to_lama(image, mask)    


if __name__ == "__main__":
    img = cv2.imread("C:/Coding/Study/Tagged-object-removal/lama/000068.png")
    cv2.imwrite("temp.jpg",process_image(["person", "dog"], img))
    