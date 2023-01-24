import unittest
import numpy as np
import cmorphosnakes as module

import logging

logging.basicConfig(level=logging.DEBUG)

SEED = 3



def random_set(shape, pad, th=0.1):
    if not pad:
        pad = min(shape) // 3
    u = np.zeros(shape, dtype="uint8")
    set_shape = [l - 2 * pad for l in shape]

    slices = [slice(pad, -pad) for _ in shape]
    np.random.seed(SEED)
    u[tuple(slices)] = np.random.random(set_shape) > th
    return u


class Test_Class3D(unittest.TestCase):
    def setUp(self):
        pad = 0
        L = 8
        # shape = (L, L - 1, L - 2)
        shape = (L, L + 1, L + 2)
        self.u_init = random_set(shape, pad)
        self.image = np.random.random(shape)
        self.n_iter = 5
        self.smoothing = 1

    # @unittest.skip("Skip")
    def test_class3d(self):
        # one step no smoothing
        r1 = module.morphological_chan_vese(
            self.image, self.n_iter, np.copy(self.u_init), smoothing=self.smoothing
        )
        snakes = module.Snakes(self.image,np.copy(self.u_init))
        snakes.iterate(self.n_iter,smoothing = self.smoothing )
        r2 = snakes.level_set

        for el1, el2 in zip(r1.ravel(), r2.ravel()):
            self.assertEqual(el1, el2) 


if __name__ == "__main__":
    unittest.main()

