#ifndef SOBEL_HPP
#define SOBEL_HPP

/*
 * sobel.cpp
 *
 * Created on: Sep 9, 2013
 * 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
 */
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <fstream>
#include "utilities.hpp"

template<typename T>
class Pixel {
public:
    Pixel (T r, T g, T b)
    {
        this->r = r ;
        this->g = g ;
        this->b = b ;
    }
    T r ;
    T g ;
    T b ;
} ;

template<typename T>
class Image {
public:
    int 			width ;
    int 			height ;
    std::vector 	<std::vector<std::shared_ptr<Pixel<T>> > > pixels ;
    std::string 	meta ;

    // Constructor
    Image()
    {
        this->width  = 0 ;
        this->height = 0 ;
    }

    int loadRgbImage (const std::string& filename)
    {
        std::ifstream imageFile ;

        imageFile.open(filename.c_str()) ;
        if(! imageFile.is_open())
        {
            std::cerr << "# Error opening the file!" << std::endl ;
            return 0 ;
        }

        // Read first line and split based on the , and any spaces before or after
        imageFile >> this->width;
        imageFile.ignore();
        imageFile >> this->height;
        imageFile.ignore();

        for (int h = 0 ; h < this->height ; h++)
        {
            std::vector<std::shared_ptr<Pixel<T>> > currRow ;

            for(int w = 0 ; w < this->width ; w++)
            {
                T r, g, b;
                readFromStream(imageFile, r);
                imageFile.ignore();
                readFromStream(imageFile, g);
                imageFile.ignore();
                readFromStream(imageFile, b);
                imageFile.ignore();

                // Add pixel to the current row
                std::shared_ptr<Pixel<T>> currPixel(new Pixel<T>(r, g, b)) ;
                currRow.push_back(currPixel) ;
            }
            this->pixels.push_back(currRow) ;
        }

        std::string line;
        std::getline(imageFile, line) ;
        this->meta = line ;
        return 1 ;
    }
    std::vector<int> exportRgbImage (T scale)
    {
        std::vector<int> output;

        for(int h = 0 ; h < this->height ; h++)
        {
            for(int w = 0 ; w < this->width; w++)
            {
                int red   = (int)(this->pixels[h][w]->r * scale) ;
                int green = (int)(this->pixels[h][w]->g * scale) ;
                int blue  = (int)(this->pixels[h][w]->b * scale) ;

                output.push_back(red);
                output.push_back(green);
                output.push_back(blue);
            }
        }
        return output ;
    }
    void makeGrayscale()
    {
        T rC(0.30 / 256.0);
        T gC(0.59 / 256.0);
        T bC(0.11 / 256.0);

        for(int h = 0 ; h < this->height ; h++)
        {
            for(int w = 0 ; w < this->width ; w++)
            {
                T luminance (( rC * this->pixels[h][w]->r ) +
                            ( gC * this->pixels[h][w]->g ) +
                            ( bC * this->pixels[h][w]->b )) ;

                this->pixels[h][w]->r = luminance ;
                this->pixels[h][w]->g = luminance ;
                this->pixels[h][w]->b = luminance ;
            }
        }
    }
} ;

template<typename T>
void window(const std::shared_ptr<Image<T>>& imagePtr, int x, int y, T window[][3])
{
    window[0][0] = imagePtr->pixels[ y - 1 ][ x - 1 ]->r ;
 	window[0][1] = imagePtr->pixels[ y - 1 ][ x ]->r ;
 	window[0][2] = imagePtr->pixels[ y - 1 ][ x + 1 ]->r ;

	window[1][0] = imagePtr->pixels[ y ][ x - 1 ]->r ;
 	window[1][1] = imagePtr->pixels[ y ][ x ]->r ;
 	window[1][2] = imagePtr->pixels[ y ][ x + 1 ]->r ;

	window[2][0] = imagePtr->pixels[ y + 1 ][ x - 1 ]->r ;
 	window[2][1] = imagePtr->pixels[ y + 1 ][ x ]->r ;
 	window[2][2] = imagePtr->pixels[ y + 1 ][ x + 1 ]->r ;
}

template<typename T>
void half_window(const std::shared_ptr<Image<T>>& imagePtr, int x, int y, T window[][3])
{
    window[0][0] = (x == 0 || y == 0) ? T(0) : imagePtr->pixels[y - 1][x - 1]->r;
	window[0][1] = (y == 0) ? T(0) : imagePtr->pixels[y - 1][x]->r;
	window[0][2] = (x == imagePtr->width -1 || y == 0) ? T(0) : imagePtr->pixels[y - 1][x + 1]->r;

	window[1][0] = (x == 0) ? T(0) : imagePtr->pixels[y][x - 1]->r;
	window[1][1] = imagePtr->pixels[y][x]->r;
	window[1][2] = (x == imagePtr->width -1) ? T(0) : imagePtr->pixels[y][x + 1]->r;

	window[2][0] = (x == 0 || y == imagePtr->height - 1) ? T(0) : imagePtr->pixels[y + 1][x - 1]->r;
	window[2][1] = (y == imagePtr->height - 1) ? T(0) : imagePtr->pixels[y + 1][x]->r;
	window[2][2] = (x == imagePtr->width -1 || y == imagePtr->height - 1) ? T(0) : imagePtr->pixels[y + 1][x + 1]->r;
}

template<typename T>
T convolve(T w[][3], T k[][3])
{
    T r(0.0);
    for( int j = 0 ; j < 3 ; j++ )
        for ( int i = 0 ; i < 3 ; i++ )
        {
            r += w[i][j] * k[j][i] ;
        }
    return r ;
}

template<typename T>
T sobelW(T w[][3])
{
    static T kx[][3] =
            {
                    { T(-1), T(-2), T(-1) },
                    { T(0), T(0), T(0) },
                    { T(1), T(2), T(1) }
            } ;

    static T ky[][3] =
            {
                    { T(-1), T(0), T(1) },
                    { T(-2), T(0), T(2) },
                    { T(-1), T(0), T(1) }
            } ;

    static T val = squareRoot(T(256 * 256 + 256 * 256));

    T sx = convolve(w, ky) ;
    T sy = convolve(w, kx) ;
    T s = squareRoot(sx * sx + sy * sy) ;


    if (s >= (T(256) / val))
        s = T(255) / val;

    return s ;
}

template<typename T>
std::vector<int> sobel(const std::string& inputFileName)
{
    int x, y;
    T s(0);

    // Source and destination image
    std::shared_ptr<Image<T>> srcImagePtr(new Image<T>());
    std::shared_ptr<Image<T>> dstImagePtr(new Image<T>());

    T w[][3] = {
            {T(0), T(0), T(0)},
            {T(0), T(0), T(0)},
            {T(0), T(0), T(0)}
    };


    srcImagePtr->loadRgbImage( inputFileName ); // source image
    dstImagePtr->loadRgbImage( inputFileName ); // destination image

    srcImagePtr->makeGrayscale( ); // convert the source file to grayscale

    y = 0 ;

    // Start performing Sobel operation
    for( x = 0 ; x < srcImagePtr->width ; x++ ) {
        half_window(srcImagePtr, x, y, w) ;


        s = sobelW(w);


        dstImagePtr->pixels[y][x]->r = s ;
        dstImagePtr->pixels[y][x]->g = s ;
        dstImagePtr->pixels[y][x]->b = s ;
    }

    for (y = 1 ; y < (srcImagePtr->height - 1) ; y++) {
        x = 0 ;
        half_window(srcImagePtr, x, y, w);

        s = sobelW(w);


        dstImagePtr->pixels[y][x]->r = s ;
        dstImagePtr->pixels[y][x]->g = s ;
        dstImagePtr->pixels[y][x]->b = s ;


        for( x = 1 ; x < srcImagePtr->width - 1 ; x++ ) {
            window(srcImagePtr, x, y, w) ;

            s = sobelW(w);

            dstImagePtr->pixels[y][x]->r = s ;
            dstImagePtr->pixels[y][x]->g = s ;
            dstImagePtr->pixels[y][x]->b = s ;

        }

        x = srcImagePtr->width - 1 ;
        half_window(srcImagePtr, x, y, w) ;


        s = sobelW(w);

        dstImagePtr->pixels[y][x]->r = s ;
        dstImagePtr->pixels[y][x]->g = s ;
        dstImagePtr->pixels[y][x]->b = s ;
    }

    y = srcImagePtr->height - 1;

    for(x = 0 ; x < srcImagePtr->width ; x++) {
        half_window(srcImagePtr, x, y, w) ;

        s = sobelW(w);

        dstImagePtr->pixels[y][x]->r = s ;
        dstImagePtr->pixels[y][x]->g = s ;
        dstImagePtr->pixels[y][x]->b = s ;

    }

    std::vector<int> output = dstImagePtr->exportRgbImage(squareRoot(T(256 * 256 + 256 * 256))) ;
    return output ;
}

#endif