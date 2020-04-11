# distutils: language = c++
cimport cython
from libcpp.vector cimport vector
cimport numpy as cnp
import numpy as np


def to_numpy(x):
    return np.asarray(x,dtype = np.int8)

cdef extern from "_morphosnakes.h":
    ctypedef struct point2d
    ctypedef struct point3d

    ctypedef struct snakes_info:
        int inside_label
        int outside_label
        double lambda1
        double lambda2
        int nx
        int ny
        int nz

    snakes_info makeinfo(int inside_label, int outside_label,
                     double lambda1, double lambda2,
                     int nx, int ny, int nz)

    vector[point2d] get_edge_list_2d(unsigned char* levelset,
                                     snakes_info conf)
    void evolve_edge_2d(double *image, 
                        unsigned char* levelset, 
                        long *counter,
                        vector[point2d] &edge_points, 
                        snakes_info conf)
    void fast_marching_dilation_2d(vector[point2d] &edge, 
                                   unsigned char* levelset, 
                                   long *counter,
                                   snakes_info conf) 
    void fast_marching_erosion_2d(vector[point2d] &edge, 
                                  unsigned char* levelset, 
                                  long *counter,
                                  snakes_info conf) 
    void sort_edge2d(vector[point2d] &edge)  
    void sort_edge3d(vector[point3d] &edge)      


    vector[point3d] get_edge_list_3d(unsigned char* levelset, 
                                     snakes_info conf)
    void evolve_edge_3d(double *image, 
                        unsigned char* levelset, 
                        long *counter,
                        vector[point3d] &edge_points, 
                        snakes_info conf)
    void fast_marching_dilation_3d(vector[point3d] &edge, unsigned char* levelset, long *counter,
                          snakes_info conf)
    void fast_marching_erosion_3d(vector[point3d] &edge, unsigned char* levelset, long *counter,
                          snakes_info conf)


@cython.boundscheck(False)
@cython.wraparound(False)
def _morphological_chan_vese_2d(double[:, ::1] image,
                            unsigned char[:, ::1] u,
                            long[::1] counter,
                            int iterations, 
                            int smoothing=1, 
                            double lambda1=1, 
                            double lambda2=1,
                            int inside = 1,
                            int outside = 0,
                            iter_callback=lambda x: None):

    cdef int smooth_counter = 0

    cdef snakes_info conf = makeinfo(inside, outside,  lambda1, lambda2, u.shape[1],u.shape[0], 0)
    # cdef snakes_info conf
    
    cdef vector[point2d] edge_points = get_edge_list_2d(&u[0, 0],
                  conf)
    iter_callback(to_numpy(u))
    for i in range(iterations):
        evolve_edge_2d(&image[0,0],
                    &u[0,0], 
                    &counter[0], 
                    edge_points, conf) 

                 
        # Smoothing
        for _ in range(smoothing):
            if smooth_counter % 2 == 0:
                fast_marching_dilation_2d(edge_points, &u[ 0,0], &counter[ 0], conf)
                fast_marching_erosion_2d(edge_points, &u[ 0,0], &counter[ 0], conf)

            else:
                fast_marching_erosion_2d(edge_points, &u[ 0,0], &counter[ 0], conf)
                fast_marching_dilation_2d(edge_points, &u[ 0,0], &counter[ 0], conf)

            smooth_counter=smooth_counter+1
        sort_edge2d(edge_points)   

        iter_callback(to_numpy(u))

    return to_numpy(u)



@cython.boundscheck(False)
@cython.wraparound(False)
def _morphological_chan_vese_3d(double[:, :, ::1] image,
                            unsigned char[:, :, ::1] u,
                            long[::1] counter,
                            int iterations, 
                            int smoothing=1, 
                            double lambda1=1, 
                            double lambda2=1,
                            int inside = 1,
                            int outside = 0,
                            iter_callback=lambda x: None):

    cdef int smooth_counter = 0
    cdef snakes_info conf = makeinfo(inside, outside, lambda1, lambda2, u.shape[2],u.shape[1],u.shape[0])
    # cdef snakes_info conf

    cdef vector[point3d] edge_points = get_edge_list_3d(&u[0, 0,0],
                 conf)    
       
    iter_callback(to_numpy(u))
    for _ in range(iterations):
        evolve_edge_3d(&image[0,0,0],
                    &u[0,0,0], 
                    &counter[0],  
                    edge_points,conf) 


        # Smoothing
        for _ in range(smoothing):        
            if smooth_counter % 2 == 0:
                fast_marching_dilation_3d(edge_points, &u[0, 0,0], &counter[0],conf)
                fast_marching_erosion_3d(edge_points, &u[0, 0,0], &counter[0],conf)
            else:
                fast_marching_erosion_3d(edge_points, &u[0, 0,0], &counter[0],conf)
                fast_marching_dilation_3d(edge_points, &u[0, 0,0], &counter[0],conf)
            smooth_counter+=1
        
        sort_edge3d(edge_points)        
        iter_callback(to_numpy(u))

    return to_numpy(u)
