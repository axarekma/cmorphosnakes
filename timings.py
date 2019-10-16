import numpy as np
from time import time

import cmorphosnakes


from cmorphosnakes import morphological_chan_vese
from cmorphosnakes import morphological_chan_vese_fm
from cmorphosnakes import checkerboard_level_set, circle_level_set

# from cpysnakes.snakes import c_morphological_chan_vese

try:  # python2 & python3 compatibility
    xrange
except NameError:
    xrange = range


def bench(msg, iterable, max_elapse=20):
    n = 0
    t0 = time()
    for _ in iterable:
        n += 1
        if (time() - t0) > max_elapse:
            break
    t1 = time()
    print(
        "{:<20s} calls: {:<10d} elapsed: {:.3f} s       ms/call: {:.1f} ms".format(
            msg + ":", n, t1 - t0, 1e3 * (t1 - t0) / n
        )
    )


def random_set(shape, pad=None, th=0.3):
    if not pad:
        pad = min(shape) // 3
    u = np.zeros(shape)
    set_shape = [l - 2 * pad for l in shape]

    slices = [slice(pad, -pad) for _ in shape]
    u[tuple(slices)] = np.random.random(set_shape) > th
    return u


def benchmark_2d(n_calls=5):
    from skimage import data, img_as_float

    image = img_as_float(data.camera())
    init_ls = checkerboard_level_set(image.shape, 6)
    iterations = 35
    smoothing = 3
    print(f"\n === 2d case === shape {image.shape} nit {iterations}")

    bench(
        "Reference",
        (
            morphological_chan_vese(image, iterations, init_ls, smoothing == smoothing)
            for _ in range(n_calls)
        ),
    )
    bench(
        "Edge propagation",
        (
            morphological_chan_vese_fm(
                image, iterations, init_ls, smoothing == smoothing
            )
            for _ in range(n_calls)
        ),
    )

    print("    Overhead (nit==0)")
    iterations = 0
    bench(
        "Reference",
        (morphological_chan_vese(image, iterations, init_ls) for _ in range(n_calls)),
    )
    bench(
        "Edge propagation",
        (
            morphological_chan_vese_fm(image, iterations, init_ls)
            for _ in range(n_calls)
        ),
    )


def benchmark_3d(n_calls=5):
    image = np.load("images/confocal.npy").astype("float")
    init_ls = circle_level_set(image.shape, (30, 50, 80), 25)
    iterations = 15
    print(f"\n === 3d case === shape {image.shape} nit {iterations}")
    bench(
        "Reference",
        (morphological_chan_vese(image, iterations, init_ls) for _ in range(n_calls)),
    )
    bench(
        "Edge propagation",
        (
            morphological_chan_vese_fm(image, iterations, init_ls)
            for _ in range(n_calls)
        ),
    )

    print("    Overhead (nit==0)")
    iterations = 0
    bench(
        "Reference",
        (morphological_chan_vese(image, iterations, init_ls) for _ in range(n_calls)),
    )
    bench(
        "Edge propagation",
        (
            morphological_chan_vese_fm(image, iterations, init_ls)
            for _ in range(n_calls)
        ),
    )


if __name__ == "__main__":
    # benchmark_cash()
    benchmark_2d()
    benchmark_3d()
