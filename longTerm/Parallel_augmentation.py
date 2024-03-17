import math
import os
import shutil
import sys
import albumentations as A
import cv2
from joblib import Parallel, delayed


'''
    TODO:
     Dataset 720p o 1080p  
     prove:  100 foto, 200 foto
    con -> 10, 50, 150 augment.
'''


'''
    Struttura programma:

    Per ogni immagine del dataset:
        - Carica immagine
        Ripeti 10 volte:
            Processa immagine con modifiche random
            Salva immagine su disco

    Quindi se nel dataset ci sono 20 immagini il numero finale di immagini modificate sarà 20 * 10 
'''
def imgAugmentation(imagesPath, num_augmentations):
    cv2.setNumThreads(0)
    folder_in = 'input_images_100/'
    folder_out = 'output_images/'
    images = []
    for path in imagesPath:
        images.append(cv2.imread(folder_in + path))

    transform = A.Compose([
            A.HorizontalFlip(p=1),
            A.VerticalFlip(p=1),
            A.Rotate(limit=360, p=1),
            A.RandomBrightnessContrast(p=1),
            A.GaussianBlur(blur_limit=(3, 9), p=1),
            #A.RandomCrop(height=250, width=300, p=1),
            A.ColorJitter(brightness=0.6, contrast=0.6, saturation=0.6, hue=0.6, p=1),
            A.RGBShift(r_shift_limit=40, g_shift_limit=40, b_shift_limit=40, p=1),
            A.ChannelShuffle(p=1),
            A.RandomGamma(p=1),
            A.Blur(p=1),
            A.ToGray(p=1),
        ])

    print(f'Augmenting images by thread number: {os.getpid()}')
    j = 0
    for image in images:
        i = 0
        for i in range(num_augmentations):
            # Esegue l'augmentation dell'immagine
            augmented_image = transform(image=image)['image']
            #augmented_image = transform(image=np.array(image))['image']
            # Salva l'immagine augmentata
            #augmented_image = Image.fromarray(augmented_image)
            #augmented_image.save('./' + folder_out + '/augmented_image' + str(j) + '_' + str(i) + '.jpg')
            cv2.imwrite('./' + folder_out + '/augmented_image' + str(j) + '_' + str(i) + str(os.getpid()) + '.jpg', augmented_image)
        j = j + 1

    print(f'Finished thread number: {os.getpid()}')


if __name__ == '__main__':
    folder_in = 'input_images_100/'
    folder_out = 'output_images/'
    num_augmentations = int(sys.argv[1])
    num_thread = int(sys.argv[2])

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

    #print("Lunghezza lista immagini: " + str(len(images_from_folder)))

    imageBatchSize = math.ceil(len(images_from_folder) / num_thread)
    batches_for_process = [images_from_folder[i:i + imageBatchSize] for i in range(0, len(images_from_folder), imageBatchSize)]
    print(f'batches_for_process {batches_for_process}')

    #image_paths = next(os.walk('in_images'))[2]    ISTRUZIONI DA VEDERE DA MACHINS
    #num_images = len(image_paths)
    #batch_size = round(num_images // num_thread)
    #path_batches = [image_paths[i:i + batch_size] for i in range(0, num_images, batch_size)]

    #tasks = [asyncio.create_task(imgAugmentation(batch, num_augmentations)) for batch in batches_for_process]

    Parallel(n_jobs=num_thread)(delayed(imgAugmentation)(batch, num_augmentations) for batch in batches_for_process)

    print("Finito tutto dhn")