import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
import math

from natsort import natsorted

import cv2
import os

dataframe = pd.read_csv("evo-algo.csv")

def fitness(x):
    return (
        math.sin(2.0 * x)
        - math.cos(x)
        + math.sin(6.0 * x)
        + math.sin(10.0 * x)
    )

for generation in dataframe['generation'].unique():
    sns.scatterplot(
        data=dataframe[dataframe['generation'] == generation],
        x="genome",
        y="fitness",
    )

    sns.lineplot(
        x=np.linspace(dataframe['generation'].min(), 6, 100),
        y=list(map(fitness, np.linspace(0, 6, 100)))
    )

    if not os.path.exists("images"):
        os.makedirs("images")

    plt.savefig("images/{0:03d}.png".format(generation))
    plt.clf()

print(dataframe)

# adapted from https://stackoverflow.com/a/44948030
image_folder = 'images'
video_name = 'video.avi'

images = [img for img in os.listdir(image_folder)]

natsorted(images)

frame = cv2.imread(os.path.join(image_folder, images[0]))
height, width, layers = frame.shape

video = cv2.VideoWriter(video_name, 0, 5, (width,height))

for image in images:
    video.write(cv2.imread(os.path.join(image_folder, image)))

cv2.destroyAllWindows()
video.release()
