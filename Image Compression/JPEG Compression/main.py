import re
import cv2
import numpy as np
from skimage import io


def showImage(img):
    io.imshow(img)
    io.show()


# Quantization Arrays
inp = input("Enter image name:")
fnum = int(input("Enter filter:"))
img = cv2.imread(inp, 0)
showImage(img)

# one column of image
height = len(img)
# one row of image
width = len(img[0])
block = 3
res = np.zeros((height - block + 1, width - block + 1), dtype=int)

filter = [
    np.array([[1, 0, -1], [1, 0, -1], [1, 0, -1]]),
    np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]]),
    np.array([[3, 0, -3], [10, 0, -10], [3, 0, -3]]),
]

# dividing block x block parts
for i in range(block, height + 1):
    for j in range(block, width + 1):
        sliced = np.array(img[i - block : i, j - block : j])
        res[i - block][j - block] = np.sum(np.multiply(filter[fnum], sliced))

name = re.findall("(.*)[.]", inp)[0] + "Edge" + re.findall("[.].*", inp)[0]

cv2.imwrite(name, res)

showImage(cv2.imread(name, 0))
