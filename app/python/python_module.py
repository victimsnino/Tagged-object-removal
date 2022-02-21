import cv2

def process_image(image):
    start_point = (5, 5)
    end_point = (220, 220)
    color = (255, 0, 0)
    thickness = 2
    image = cv2.rectangle(image, start_point, end_point, color, thickness)
    return image
    