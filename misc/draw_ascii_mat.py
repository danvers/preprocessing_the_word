#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

def draw_mat(cmap, filename):
    char_m = np.array([chr(c) for c in range(256)])
    char_m = np.reshape(char_m, (32, 8))
    char_m = np.hstack((char_m[:16,], char_m[16:,]))

    img_m = np.zeros((16, 16))
    img_m[:, 8:] = 1
    img_m[15, 15] = 2

    fig, ax = plt.subplots(figsize=(4, 4), dpi=150)
    ax.matshow(
        img_m,
        cmap = cmap,
        alpha = 0.65
    )
    for i in range(16):
        for j in range(16):
            c = char_m[j, i]
            ax.text(
                i, j,
                str(c),
                va = 'center',
                ha = 'center',
                fontsize = 'large',
            )
    ax.axis('off');
    fig.savefig(filename, bbox_inches='tight')

def main():
    outdir = "../data/img/"
    draw_mat(ListedColormap(['#DC267F', '#648FFF', '#FE6100'], 3), outdir+"ascii_matrix_highlighted.png")
    draw_mat(ListedColormap(['#DC267F', '#648FFF'], 2), outdir+"ascii_matrix.png")

if __name__ == '__main__':
    main()
