import os
import shutil
import sys
import time
import albumentations as A
import cv2

cv2.setNumThreads(0)
num_augmentations = int(sys.argv[1])
folder_in = 'input_images_100/'
folder_out = 'output_images/'
try:
    print("Cleaning workspace...")
    shutil.rmtree(folder_out)
    shutil.os.mkdir(folder_out)
    print("Done")
except OSError as e:
        print(f"ERROR: {folder_out} - {e.strerror}")
        shutil.os.mkdir(folder_out)
        print(folder_out + " created!")
try:
    images_from_folder = os.listdir(folder_in)
except OSError as e:
    print(f"ERROR: {folder_in} - {e.strerror}")
    print("ERROR: import jpg dataset images into " + folder_in)
    sys.exit(1)

images = [cv2.imread(folder_in + image) for image in images_from_folder]

transform = A.Compose([
        A.HorizontalFlip(p=1),
        A.VerticalFlip(p=1),
        A.Rotate(limit=360, p=1),
        A.RandomBrightnessContrast(p=1),
        A.GaussianBlur(blur_limit=(3, 9), p=1),
        A.ColorJitter(brightness=0.6, contrast=0.6, saturation=0.6, hue=0.6, p=0.5),
        A.RGBShift(r_shift_limit=40, g_shift_limit=40, b_shift_limit=40, p=0.5),
        A.ChannelShuffle(p=1),
        A.RandomGamma(p=1),
        A.Blur(p=1),
        A.ToGray(p=0.5),
    ])

print('Augmenting images...\n')

j = 0
for image in images:
    i = 0
    for i in range(num_augmentations):
        # Augmenting image
        augmented_image = transform(image=image)['image']
        # Writing augmented image on disk
        cv2.imwrite('./' + folder_out + '/augmented_image' + str(j) + '_' + str(i) + '.jpg', augmented_image)
    j = j + 1
print("Done")