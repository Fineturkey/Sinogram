#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include "fft.h"

using namespace std;

// allocates an m x n two-dimensional array of integers
int **allocate(int m, int n);

Coeff **allocate_complex(int m, int n);

// deallocates a two-dimensional array of integers with m rows
void deallocate(int **A, int m);

void deallocate_complex(Coeff **A, int m);

// reads a PGM image from the given file name and return the dimensions
// and the image in a two dimensional array.  No error checking is done
// to see whether the image file is valid (other than whether it can
// be opened for reading).
bool read_image(const char *filename, int &m, int &n, Coeff **&img);

// writes a PGM image to the given file name
bool write_image(const char *filename, int m, int n, Coeff **img);

// compute the width needed for the sinogram
int compute_sinogram_width(int m, int n);

// computes the sinogram of a given pgm
void create_sinogram(Coeff **img, Coeff **sinogram, int numAngles, int m, int n, int sinogram_width);

// performs the FFT onto the sinogram to break it into its frequency domain. From there apply the filter.
void fft_sinogram(Coeff **sinogram, int m, int n, int sinogram_width);

// backproject the filtered image to obtain the final result
void back_projection(Coeff **sinogram, int numAngles, int sinogram_width, int m, int n, Coeff **output);

const double PI = acos(-1);
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input.pgm> <output.pgm>" << endl;
        return 1;
    }

    int m, n;
    int totalProjections = 180;
    Coeff **img; // Change img to a 2D array of Coeff objects

    if (!read_image(argv[1], m, n, img))
    {
        cerr << "Cannot open " << argv[1] << " for reading." << endl;
        return 2;
    }

    cout << "Entering create sinogram" << endl;

    int sinogram_width = compute_sinogram_width(m, n);

    Coeff **sinogram = allocate_complex(totalProjections, sinogram_width);

    Coeff **output = allocate_complex(m, n);

    create_sinogram(img, sinogram, totalProjections, m, n, sinogram_width);
    fft_sinogram(sinogram, m, n, sinogram_width);
    back_projection(sinogram, totalProjections, sinogram_width, m, n, output);

    cout << "Entering write image" << endl;

    if (!write_image(argv[2], m, n, output)) // Adjust parameters to write_image
    {
        cerr << "Cannot open " << argv[2] << " for writing." << endl;
        return 3;
    }

    deallocate_complex(img, m); // Use deallocate_complex instead of deallocate
    deallocate_complex(output, m);
    deallocate_complex(sinogram, totalProjections);

    return 0;
}
// allocates an m x n two-dimensional array of integers
int **allocate(int m, int n)
{
    int **A = new int *[m];
    for (int i = 0; i < m; i++)
    {
        A[i] = new int[n];
    }
    return A;
}
Coeff **allocate_complex(int m, int n)
{
    Coeff **A = new Coeff *[m];
    for (int i = 0; i < m; i++)
    {
        A[i] = new Coeff[n];
    }
    return A;
}

void deallocate(int **A, int m)
{
    for (int i = 0; i < m; i++)
    {
        delete[] A[i];
    }
    delete[] A;
}
// deallocates a two-dimensional array of integers with m rows

void deallocate_complex(Coeff **A, int m)
{
    for (int i = 0; i < m; i++)
    {
        delete[] A[i];
    }
    delete[] A;
}
// deallocates a complex two dimensional array

// reads a PGM image from the given file name and return the dimensions
// and the image in a two dimensional array.  No error checking is done
// to see whether the image file is valid (other than whether it can
// be opened for reading).
bool read_image(const char *filename, int &m, int &n, Coeff **&img)
{
    ifstream is(filename, ios::binary);
    if (!is)
    {
        return false;
    }

    string s;

    is >> s;
    is >> n >> m >> s;
    is.ignore(1024, '\n');
    img = allocate_complex(m, n);

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            img[i][j] = Coeff(is.get(), 0);
        }
    }
    return true;
}

// writes a PGM image to the given file name
bool write_image(const char *filename, int m, int n, Coeff **img)
{
    cout << "In write image" << endl;
    string s = filename;

    // Find the maximum value in the image
    double maxValue = 0.0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            maxValue = std::max(maxValue, img[i][j].real());
        }
    }

    if (maxValue > 0.0)
    {
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                img[i][j] = img[i][j].real() * (255.0 / maxValue);
            }
        }
    }

    // Allocate and copy image data to an integer array
    int **intArray = allocate(m, n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // Ensure the value is within 0-255 range (clamp)
            intArray[i][j] = std::min(255, std::max(0, (int)round(img[i][j].real())));
        }
    }

    // Write the image data to the file
    ofstream os(s.c_str(), ios::binary);
    if (!os)
    {
        deallocate(intArray, m);
        return false;
    }

    os << "P5" << endl;
    os << n << " " << m << endl;
    os << 255 << endl;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            os.put(static_cast<unsigned char>(intArray[i][j]));
        }
    }

    deallocate(intArray, m);
    return os.good();
}

int compute_sinogram_width(int m, int n)
{
    int ans = 1;
    while (ans < ceil(sqrt(m * m + n * n)))
    {
        ans *= 2;
    }
    return ans;
}

void create_sinogram(Coeff **img, Coeff **sinogram, int numAngles, int m, int n, int sinogram_width)
{
    cout << "Entered" << endl;
    // attempted at incrementing it based off the amount of angles/projections to be done.
    // numAngles has to be set to 180
    // increment according to the total amount of angles wished to be checked
    for (int theta = 0; theta < numAngles; theta++)
    {
        double angle = static_cast<double>(theta) * PI / (numAngles); // Normalize theta (0 to pi), tried normalizing in terms of the numAngles
        // this was set in place so the amount of projections done will be limited to 180 as you only need to go 180 degrees before you compute the entire sinogram
        for (int r = 0; r < sinogram_width; r++)
        {
            Coeff sum = 0.0;
            for (int x = 0; x < m; x++)
            {
                double y_double = (r - x * cos(angle)) / sin(angle);
                int y = round(y_double);
                if (y < 0 || y >= n)
                {
                    continue;
                }
                sum += img[x][y];
            }
            int int_sum = round((sum).real());
            sinogram[theta][r] = int_sum;
        }
    }
    cout << "Exit" << endl;
}

// filter
double hamming_window(double n, int j)
{
    const double PI = acos(-1.0);
    if (j >= 0 && j <= n - 1)
    {
        return 0.54 - 0.46 * cos(2.0 * PI * j / (n - 1));
    }
    else
    {
        // Handle out-of-range case
        return 0.0; // Return a default value or handle the error as appropriate
    }
}

void fft_sinogram(Coeff **sinogram, int m, int n, int sinogram_width)
{

    Coeff *buffer = new Coeff[sinogram_width];

    for (int i = 0; i < 180; i++)
    {
        FFT(sinogram[i], sinogram_width, buffer);
    }

    for (int i = 0; i < 180; i++)
    {
        for (int j = 0; j < sinogram_width; j++)
        {
            sinogram[i][j] *= hamming_window(sinogram_width, j);
        }
    }

    for (int i = 0; i < 180; i++)
    {
        inverseFFT(sinogram[i], sinogram_width, buffer);
    }

    delete[] buffer;
}

void back_projection(Coeff **sinogram, int numAngles, int sinogram_width, int m, int n, Coeff **output)
{

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            output[i][j] = 0;
        }
    }
    for (int theta = 0; theta < numAngles; theta++)
    {
        double angle = static_cast<double>(theta) * PI / (numAngles - 180);
        for (int x = 0; x < m; x++)
        {
            for (int y = 0; y < n; y++)
            {
                double r_prime = x * cos(angle) + y * sin(angle);
                int r_floor = floor(r_prime);
                int r_ceil = ceil(r_prime);
                if (r_floor < 0 || r_ceil >= sinogram_width)
                {
                    continue;
                }
                double t = r_prime - r_floor;
                // linear interpolation
                Coeff sinogram_val = (1 - t) * sinogram[theta][r_floor] + t * sinogram[theta][r_ceil];
                output[x][y] += sinogram_val;
            }
        }
    }
}