import matplotlib.pyplot as plt
import imageio

import numpy as np
from scipy import ndimage as ndi
from cmorphosnakes import morphological_chan_vese_fm
from cmorphosnakes import Snakes
from skimage.morphology import disk
import time

class MaskedLevelset:
    def __init__(self, image, seed, pad=5):
        self.images = [image]
        self.pad = pad
        self.growth = 1
        self.abs = True

        self.levelsets = [self.add_dilated_mask(seed)]
        self.iterations = 3
        self.smoothing = 1

    def add_dilated_mask(self, levelset):
        mask = ndi.binary_dilation(levelset, structure=disk(self.pad)).astype("uint8")
        return mask + levelset

    def next_image(self):
        if not self.abs:
            return self.images[0]
        return np.abs(self.images[0] - np.mean(self.images[0][self.levelsets[-1] == 2]))

    def next_init(self):
        seed = ndi.binary_dilation(
            self.levelsets[-1] == 2, structure=disk(self.growth)
        ).astype("uint8")

        return self.add_dilated_mask(seed)

    def __getitem__(self, index):
        while len(self.images) <= index:


            times = []
            times.append(time.perf_counter())
            next_image = self.next_image()
            times.append(time.perf_counter())
            next_init = self.next_init()
            times.append(time.perf_counter())

            result = morphological_chan_vese_fm(
                next_image,
                self.iterations,
                next_init,
                smoothing=self.smoothing,
                inside=2,
                outside=1,
            )
            times.append(time.perf_counter())

            self.images.append(next_image)
            self.levelsets.append(result)

            dtimes = 1000*np.diff(times)
            # print(f'times [ms] image {dtimes[0]:.3f} level_set  {dtimes[1]:.3f} snakes  {dtimes[2]:.3f}')
            # I got about 2,30,5 in ms for the timing, so most of the ork is the dilation of the extended border
            # That should be done inside the snakes module instead when calculating the mean.
            # This is probably neccesary for any 3d task.

        return self.levelsets[index]


class Mouse:
    """
    Class for handling mouse position and events
    """
    def __init__(self):
        self.state = "UP"
        self.button = None
        self.x = -1
        self.y = -1
        self.click_x = -1
        self.click_y = -1
        self.xdata = None
        self.ydata = None


    def distance(self):
        """Distance since click
  
        Returns:
            float -- Distance travaled while pressed
        """
        if min(self.click_x,self.click_y)<0:
            # no event recorded
            return 1
        vec = [self.x - self.click_x, self.y - self.click_y]
        return np.linalg.norm(vec)

    def pos_string(self):
        return f"Event_pos ({self.click_x:.2f},{self.click_y:.2f}) Current ({self.x:.2f},{self.y:.2f})"

    def self_str(self):
        return f"State {self.state}"


    def __str__(self):
        return "\n".join([self.self_str(),  self.pos_string()])
    
class MorphoHandler():
    def __init__(self, fig, ax,data):
        self.fig = fig
        self.ax = ax
        self.data = data
        self.info = ax.text(0,0, "DEFAULT INFO", va="bottom", ha="left")
        self.mouse = Mouse()
        
        self._levelset = None
        self._contour = None
        self._contour_seed = None

        self.seed = None

        
    def set_contour(self, mask):
        if self._contour is not None:
            for c in self._contour.collections:
                c.remove()  # removes only the contours, leaves the rest intact
        if self._contour_seed is not None:
            for c in self._contour_seed.collections:
                c.remove()  # removes only the contours, leaves the rest intact

        self._contour = self.ax.contour(mask, [1.5], colors="r")
        self._contour_seed = self.ax.contour(self.seed, [0.5], colors="g")


        self.fig.canvas.draw()

    def init_morphosnakes(self, x, y):
        x, y = int(round(x)), int(round(y))
        seed = self.data * 0
        seed[y, x] = 1
        seed = ndi.binary_dilation(seed, structure = disk(5))
        self.seed = seed
        self._levelset = MaskedLevelset(self.data, seed, pad=5)
        # self._levelset = Levelset(self.data, seed)
        self.set_contour(seed)

    def update_morphosnakes(self, distance):
        n_iter = int(round(distance))
        ls_result = self._levelset[n_iter]
        self.set_contour(ls_result)
        self.info.set_text(f"Morhhosnakes: Dist {distance:.3g} to iter {n_iter}")


    def release_morphosnakes(self,distance):
        n_iter = int(round(distance))
        ls_result = self._levelset[n_iter]
        self.set_contour(ls_result)


    def onclick(self,event):

        self.mouse.state = "DOWN"
        self.mouse.button = event.button
        self.mouse.click_x = event.xdata
        self.mouse.click_y = event.ydata
        self.mouse.x = event.xdata
        self.mouse.y = event.ydata
        self.init_morphosnakes(event.xdata, event.ydata)
        # self.info.set_text(self.mouse)

    def onrelease(self,event):

        self.mouse.state = "UP"
        self.release_morphosnakes( self.mouse.distance())        
        self.info.set_text(self.mouse)

        # reset mouse event
        
        # reset event
        self.mouse.click_x = -1
        self.mouse.click_y = -1

        
    def motion(self, event):
        if self.mouse.state == "UP":
            return
        self.mouse.x = event.xdata
        self.mouse.y = event.ydata
        self.update_morphosnakes(self.mouse.distance())
        
    def setup(self):
        _ = self.fig.canvas.mpl_connect('button_press_event', self.onclick)
        _ = self.fig.canvas.mpl_connect('button_release_event', self.onrelease)        
        _ = self.fig.canvas.mpl_connect("motion_notify_event", self.motion)

    def draw(self):
        self.fig.canvas.draw()




if __name__ == '__main__':
    PATH_IMG_CAMERA = 'images/camera.png'
    img = imageio.v3.imread(PATH_IMG_CAMERA)/255.0

    fig, ax = plt.subplots()
    ax.imshow(img)


    handler = MorphoHandler(fig, ax, data = img)
    handler.setup()
    print('exit')

    handler.info.set_text('Click and drag mouse to initialize and grow a levelset')
    plt.show()

