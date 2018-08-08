#ifndef SEGMENTATION_HPP
#define SEGMENTATION_HPP

#include "rgbimage.hpp"
#include "utilities.hpp"

template<typename T>
struct Centroid {
    T r;
    T g;
    T b;
    int n;
};

template<typename T>
struct Clusters {
    int k;
    Centroid<T>* centroids;
};

template<typename T>
T euclideanDistance(RgbPixel<T>* p, Centroid<T>* c1) {
    T r(0);

    r += (p->r - c1->r) * (p->r - c1->r);
    r += (p->g - c1->g) * (p->g - c1->g);
    r += (p->b - c1->b) * (p->b - c1->b);

    return squareRoot(r);
}

template<typename T>
int pickCluster(RgbPixel<T>* p, Centroid<T>* c1) {
    T d1(euclideanDistance(p, c1));

    if (p->distance <= d1)
        return 0;

    return 1;
}

template<typename T>
void assignCluster(RgbPixel<T>* p, Clusters<T>* clusters) {
    int i = 0;

    T d(euclideanDistance(p, &clusters->centroids[i]));
    p->distance = d;

    for(i = 1; i < clusters->k; ++i) {
        d = euclideanDistance(p, &clusters->centroids[i]);
        if (d < p->distance) {
            p->cluster = i;
            p->distance = d;
        }
    }
}

template<typename T>
int initClusters(Clusters<T>* clusters, int k, T scale) {
    int i;

    clusters->centroids = (Centroid<T>*)malloc(k * sizeof(Centroid<T>));

    if (clusters->centroids == nullptr) {
        printf("Warning: Oops! Cannot allocate memory for the clusters!\n");

        return 0;
    }

    clusters->k = k;
    const T randMax = T(RAND_MAX);

    for (i = 0; i < clusters->k; i ++) {
        T x (((T(rand()))/randMax) * scale);
        clusters->centroids[i].r = x;

        x = ((T(rand()))/randMax) * scale;
        clusters->centroids[i].g = x;

        x = ((T(rand()))/randMax) * scale;
        clusters->centroids[i].b = x;

        clusters->centroids[i].n = 0;
    }
    return 1;
}

template<typename T>
void freeClusters(Clusters<T>* clusters) {
    if (clusters != NULL)
        free(clusters->centroids);
}

template<typename T>
void segmentImage(RgbImage<T>* image, Clusters<T>* clusters, int n) {
    int i;
    int x, y;
    int c;

    for (i = 0; i < n; ++i) {
        for (y = 0; y < image->h; y++) {
            for (x = 0; x < image->w; x++) {
                assignCluster(&image->pixels[y][x], clusters);
            }
        }

        /** Recenter */
        for (c  = 0; c < clusters->k; ++c) {
            clusters->centroids[c].r = T(0);
            clusters->centroids[c].g = T(0);
            clusters->centroids[c].b = T(0);
            clusters->centroids[c].n = 0;
        }
        for (y = 0; y < image->h; y++) {
            for (x = 0; x < image->w; x++) {
                clusters->centroids[image->pixels[y][x].cluster].r += image->pixels[y][x].r;
                clusters->centroids[image->pixels[y][x].cluster].g += image->pixels[y][x].g;
                clusters->centroids[image->pixels[y][x].cluster].b += image->pixels[y][x].b;
                clusters->centroids[image->pixels[y][x].cluster].n += 1;
            }
        }
        for (c  = 0; c < clusters->k; ++c) {
            if (clusters->centroids[c].n != 0) {
                T nVal(clusters->centroids[c].n);
                clusters->centroids[c].r = clusters->centroids[c].r / nVal;
                clusters->centroids[c].g = clusters->centroids[c].g / nVal;
                clusters->centroids[c].b = clusters->centroids[c].b / nVal;
            }
        }
    }

    for (y = 0; y < image->h; y++) {
        for (x = 0; x < image->w; x++) {
            image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
            image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
            image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
        }
    }
}

#endif