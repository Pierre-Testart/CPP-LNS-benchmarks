#ifndef RGBIMAGE_HPP
#define RGBIMAGE_HPP

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

template<typename T>
struct RgbPixel {
    T r;
    T g;
    T b;
    int cluster;
    T distance;
};

template<typename T>
struct RgbImage {
    int w;
    int h;
    RgbPixel<T>** pixels;
    char* meta;
};

template<typename T>
void initRgbImage(RgbImage<T>* image) {
    image->w = 0;
    image->h = 0;
    image->pixels = nullptr;
    image->meta = nullptr;
}

int readCell(FILE *fp, char* w) {
    int c;
    int i;

    w[0] = 0;
    for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) {
        if (c == ' ' || c == '\t') {
            if (w[0] != '\"')
                continue;
        }

        if (c == ',' || c == '\n') {
            if (w[0] != '\"')
                break;
        }

        w[i] = c;
        i++;
    }
    w[i] = 0;

    return c;
}

template<typename T>
int loadRgbImage(const char* fileName, RgbImage<T>* image, T scale) {
    int c;
    int i;
    int j;
    char w[256];
    RgbPixel<T>** pixels;
    FILE *fp;

    //printf("Loading %s ...\n", fileName);

    fp = fopen(fileName, "r");
    if (!fp) {
        printf("Warning: Oops! Cannot open %s!\n", fileName);
        return 0;
    }

    c = readCell(fp, w);
    image->w = atoi(w);
    c = readCell(fp, w);
    image->h = atoi(w);

    //printf("%d x %d\n", image->w, image->h);

    pixels = (RgbPixel<T>**)malloc(image->h * sizeof(RgbPixel<T>*));

    if (pixels == nullptr) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        fclose(fp);

        return 0;
    }

    c = 0;
    for(i = 0; i < image->h; i++) {
        pixels[i] = (RgbPixel<T>*)malloc(image->w * sizeof(RgbPixel<T>));
        if (pixels[i] == nullptr) {
            c = 1;
            break;
        }
    }

    if (c == 1) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        for (i--; i >= 0; i--)
            free(pixels[i]);
        free(pixels);

        fclose(fp);

        return 0;
    }

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++) {
            c = readCell(fp, w);
            pixels[i][j].r = T(atoi(w)) / scale;

            c = readCell(fp, w);
            pixels[i][j].g = T(atoi(w)) / scale;

            c = readCell(fp, w);
            pixels[i][j].b = T(atoi(w)) / scale;

            pixels[i][j].cluster = 0;
            pixels[i][j].distance = T(0);
        }
    }
    image->pixels = pixels;

    c = readCell(fp, w);
    image->meta = (char*)malloc(strlen(w) * sizeof(char));
    if(image->meta == nullptr) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        for (i = 0; i < image->h; i++)
            free(pixels[i]);
        free(pixels);

        fclose(fp);

        return 0;

    }
    strcpy(image->meta, w);

    //printf("%s\n", image->meta);

    //printf("w=%d x h=%d\n", image->w, image->h);

    return 1;
}

template<typename T>
std::vector<int> exportRgbImage(RgbImage<T>* image, T scale) {
    int i;
    int j;

    std::vector<int> output;

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++) {
            output.push_back((int)(image->pixels[i][j].r * scale));
            output.push_back((int)(image->pixels[i][j].g * scale));
            output.push_back((int)(image->pixels[i][j].b * scale));
        }
    }

    return output;
}

template<typename T>
void freeRgbImage(RgbImage<T>* image) {
    int i;

    if (image->meta != nullptr)
        free(image->meta);

    if (image->pixels == nullptr)
        return;

    for (i = 0; i < image->h; i++)
        if (image->pixels != nullptr)
            free(image->pixels[i]);

    free(image->pixels);
}

#endif
