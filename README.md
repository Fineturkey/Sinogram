# Recreation of an image through projections

In order to run the code, drag and drop a PGM file. 
You will need a software to open the file as well. In my case it was gimp. Run it through your powershell so that it complies together
From there you will get an output.pgm image which will be the output of the image passed through the code and reconstructed using projections

CT scans, otherwise known as X-ray computed tomography will be explained shortly. To begin we will discuss the issue in a less technical sense in order to lay out an arbitrary and easy to understand definition. For the sake of this example we will be reconstructing the image in a 2-D plane.

The image subject to reconstruction will be that of a circle, say that we want to reconstruct it using CT scans. What a CT scan will do is send multiple projections at different angles to graph or measure each intensity value. We will describe what a single projection will do in this case. When the projection “detects” or passes through a subject it will collect and graph an intensity value. That projection now holds a value, which will then go through another process known as backprojection.

Backprojection is the act of sending back the projection, and its value which smears the image back to where the projection originated from. By sending multiple projections over many different angles one can begin to generalize the shape of the image being recreated. The overall intensity surrounding the object is greater than that which does not cover the subject at hand. What you will notice about the bottom right image is how blurry and convoluted the image itself is. 

What is known as the halo effect, is evidence that even if you can recreate the general shape of the subject there is still unwanted blurriness created by the projection. This is in theory how a CT is done with a human. In practice the projection beam itself is an X-ray being sent left to right, with an X-ray absorption or detector on the other side to absorb and send back the data to reconstruct the image. In this readme we will look at many different techniques one may apply in order to simulate and re-create the CT scan. 

The projection of a parallel-ray beam can be modeled by its normal representation $(xcos θ + ysin θ)$ which is considereed because slope-intercept form cannot represent a straight vertical line.

consider the arbitrary point ($p_j$, $θ_k$), which is given by the raysum along the line $xcos θ_k + y sin θ_k = p_k$
Working with continous projections (multiple different raysums) you take the raysum line integral represented by this equation: ![Equation](https://latex.codecogs.com/svg.image?g(p_j,%20\theta_k)%20=%20\int_{-\infty}^{\infty}%20\int_{-\infty}^{\infty}%20f(x,y)\,\delta(x\cos\theta_k%20+%20y\sin\theta_k%20-%20p_j)\,dx\,dy)

This is the _raysum_ line integral which is referring to the total intensity obtained from summing the intensity values across the opposite point of the projection.

Which in turn is obtained through the following transformation

![Equation](https://latex.codecogs.com/svg.image?g(p,%20\theta)%20=%20\sum_{x=0}^{M-1}%20\sum_{y=0}^{N-1}%20f(x,%20y)\,\delta(x\cos\theta%20+%20y\sin\theta%20-%20p))

By incrementing through all values p, you can complete the span of $m x n$ where m is the width of the image and n is the length. Keep in note you need to span through multiple values of $θ$ (the degrees of the projection), the tighter the increments of $θ$ the more precise the recreation will be.

# Sinogram

The sinogram is an image created with the Radon transform ($g(p, θ)$). A sinogram is a visual representation of the Radon transform, displaying the data gathered through the transform. This will be useful later on, its essentially an image created through the values of ($(p, θ)$) where $p$ and $θ$ are its respective coordinates.

# Backprojections


