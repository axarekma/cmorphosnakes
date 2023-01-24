import numpy as np
import unittest
from skimage._shared import testing
from skimage._shared.testing import assert_array_equal

import cmorphosnakes as module
from cmorphosnakes import circle_level_set


def gaussian_blob():
    coords = np.mgrid[-5:6, -5:6]
    sqrdistances = (coords ** 2).sum(0)
    return np.exp(-sqrdistances / 10)


class Test_skimage(unittest.TestCase):
    def test_morphsnakes_incorrect_image_shape(self):
        img = np.zeros((10, 10, 3))
        ls = np.zeros((10, 9))

        with testing.raises(ValueError):
            module.morphological_chan_vese_fm(img, iterations=1, init_level_set=ls)

    def test_morphsnakes_incorrect_ndim(self):
        img = np.zeros((4, 4, 4, 4))
        ls = np.zeros((4, 4, 4, 4))

        with testing.raises(ValueError):
            module.morphological_chan_vese_fm(img, iterations=1, init_level_set=ls)

    def test_morphsnakes_black(self):
        img = np.zeros((11, 11))
        ls = circle_level_set(img.shape, (5, 5), 3)

        ref_zeros = np.zeros(img.shape, dtype=np.int8)
        ref_ones = np.ones(img.shape, dtype=np.int8)

        acwe_ls = module.morphological_chan_vese_fm(
            img, iterations=6, init_level_set=ls
        )
        assert_array_equal(acwe_ls, ref_zeros)
        assert acwe_ls.dtype == np.int8

    # @unittest.skip("this fails untill edges are fixed")
    def test_morphsnakes_simple_shape_chan_vese(self):
        img = gaussian_blob()
        ls1 = circle_level_set(img.shape, (5, 5), 3)
        ls2 = circle_level_set(img.shape, (5, 5), 6)

        acwe_ls1 = module.morphological_chan_vese_fm(
            img, iterations=10, init_level_set=ls1
        )
        acwe_ls2 = module.morphological_chan_vese_fm(
            img, iterations=10, init_level_set=ls2
        )
        assert_array_equal(acwe_ls1, acwe_ls2)

        assert acwe_ls1.dtype == acwe_ls2.dtype == np.int8

    def test_init_level_sets(self):
        image = np.zeros((6, 6))
        checkerboard_ls = module.morphological_chan_vese_fm(image, 0, "checkerboard")
        checkerboard_ref = np.array(
            [
                [0, 0, 0, 0, 0, 1],
                [0, 0, 0, 0, 0, 1],
                [0, 0, 0, 0, 0, 1],
                [0, 0, 0, 0, 0, 1],
                [0, 0, 0, 0, 0, 1],
                [1, 1, 1, 1, 1, 0],
            ],
            dtype=np.int8,
        )

        assert_array_equal(checkerboard_ls, checkerboard_ref)

    # @unittest.skip("this fails until edges are fixed")
    def test_morphsnakes_3d(self):
        image = np.zeros((7, 7, 7))

        evolution = []

        def callback(x):
            evolution.append(x.sum())

        ls = module.morphological_chan_vese_fm(
            image, 5, "circle", iter_callback=callback
        )
        # print(ls)

        # Check that the initial circle level set is correct
        assert evolution[0] == 81

        # Check that the final level set is correct
        assert ls.sum() == 0

        # Check that the contour is shrinking at every iteration
        for v1, v2 in zip(evolution[:-1], evolution[1:]):
            assert v1 >= v2


if __name__ == "__main__":
    unittest.main()
    # suite = unittest.TestSuite()
    # suite.addTest(Test_skimage("test_morphsnakes_simple_shape_chan_vese"))
    # runner = unittest.TextTestRunner()
    # runner.run(suite)
