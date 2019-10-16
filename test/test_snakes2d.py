import unittest
import numpy as np
import cmorphosnakes as module

import logging

logging.basicConfig(level=logging.DEBUG)

SEED = 3


def ref_chan_vese(*args, **kwargs):
    """cycle runs as a module variable, 
    wrap reference method with a reset so it starts deterministically"""

    sup_inf = module.morphsnakes.sup_inf
    inf_sup = module.morphsnakes.inf_sup
    _fcycle = module.morphsnakes._fcycle
    module.morphsnakes._curvop = _fcycle(
        [lambda u: sup_inf(inf_sup(u)), lambda u: inf_sup(sup_inf(u))]
    )
    return module.morphsnakes.morphological_chan_vese(*args, **kwargs)


def random_set(shape, pad, th=0.1):
    if not pad:
        pad = min(shape) // 3
    u = np.zeros(shape, dtype="uint8")
    set_shape = [l - 2 * pad for l in shape]

    slices = [slice(pad, -pad) for _ in shape]
    np.random.seed(SEED)
    u[tuple(slices)] = np.random.random(set_shape) > th
    return u


class Test_Morphosnakes2D(unittest.TestCase):
    def setUp(self):
        pad = 0
        L = 8
        H, W = L, L - 1
        self.u_init = random_set((H, W), pad)
        self.image = np.random.random((H, W))
        self.n_iter = pad - 2
        self.smoothing = 1

    def test_evolve2d(self):
        # one step no smoothing
        r1 = module.morphological_chan_vese(
            self.image, 1, np.copy(self.u_init), smoothing=0
        )
        r2 = module.morphological_chan_vese_fm(
            self.image, 1, np.copy(self.u_init), smoothing=0
        )

        for el1, el2 in zip(r1.ravel(), r2.ravel()):
            self.assertEqual(el1, el2)

    def test_snakes2d(self):
        r1 = module.morphological_chan_vese(
            self.image, self.n_iter, np.copy(self.u_init), smoothing=self.smoothing
        )
        r2 = module.morphological_chan_vese_fm(
            self.image, self.n_iter, np.copy(self.u_init), smoothing=self.smoothing
        )
        for el1, el2 in zip(r1.ravel(), r2.ravel()):
            self.assertEqual(el1, el2)


if __name__ == "__main__":
    unittest.main()

