# distutils: language = c++
# distutils: sources = csrc/Snakes2D.cpp,csrc/Snakes3D.cpp

cimport cython
import numpy as np
cimport numpy as cnp
from libcpp.vector cimport vector

from Snakes cimport Snakes2D,Snakes3D

cdef extern from "csrc/Snakes3D.h":
    cdef cppclass Snakes3D:
        pass

cdef extern from "csrc/Snakes2D.h": 
    cdef cppclass Snakes3D:
        pass

def to_numpy(x):
    return np.asarray(x,dtype = np.int8)

cdef extern from "csrc/morphosnakes.h":
    void hello_openmp()
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

def _hello_openmp():
    return hello_openmp()


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

    cdef snakes_info conf = makeinfo(inside, outside,  lambda1, lambda2, <int>u.shape[1],<int>u.shape[0], 0)
    
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
    cdef snakes_info conf = makeinfo(inside, outside, lambda1, lambda2, <int>u.shape[2],<int>u.shape[1],<int>u.shape[0])
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
 

 

cdef class PySnakes3D:  
    cdef Snakes3D c_obj 
    cdef double[:, :, ::1] c_data
    cdef unsigned char[:, :, ::1] c_level_set

    def __cinit__(self): 
        pass

    def __init__(self, image,
                           u,
                            inside = 1,
                             outside = 0,lambda1 = 1, lambda2 = 1):
        if u.dtype != 'uint8':
            u = u.astype('uint8')
        if image.dtype != 'float64':
            image = image.astype('float64')        
            
        self.__init_c_obj(image, u, inside, outside ,lambda1 , lambda2 )

    cdef __init_c_obj(self, double[:, :, ::1] image,
                            unsigned char[:, :, ::1] u,
                            int inside = 1,
                            int outside = 0,lambda1 = 1, lambda2 = 1):
        cdef snakes_info conf = makeinfo(inside, outside, lambda1, lambda2, <int>u.shape[2],<int>u.shape[1],<int>u.shape[0])
        self.c_obj = Snakes3D(&image[0,0,0], &u[0,0,0], conf)
        self.c_obj.initialize()
        self.c_data = image
        self.c_level_set = u

    def iterate(self, iterations=1, smoothing = 1):
        for n in range(iterations):
            self.c_obj.evolve()
            for _ in range(smoothing):
                self.c_obj.smooth() 

    def smooth(self): 
        self.c_obj.smooth() 

    def evolve(self):
        self.c_obj.evolve() 

    @property
    def level_set(self):
        return np.asarray(self.c_level_set,dtype = np.int8)

    def set_data(self, double[:, :, ::1] image):
        self.c_obj.set_data(&image[0,0,0])



cdef class PySnakes2D:  
    cdef Snakes2D c_obj 
    cdef unsigned char[:,  ::1] c_level_set

    def __cinit__(self):
        pass
 
    def __init__(self, image,
                           u,
                            inside = 1,
                             outside = 0,lambda1 = 1, lambda2 = 1):
        if u.dtype != 'uint8':
            u = u.astype('uint8')
        if image.dtype != 'float64':
            image = image.astype('float64')        
        self.__init_c_obj(image, u, inside, outside ,lambda1 , lambda2 )

    cdef __init_c_obj(self, double[:,  ::1] image,
                            unsigned char[:,  ::1] u,
                            int inside = 1,
                            int outside = 0,lambda1 = 1, lambda2 = 1):
        cdef snakes_info conf = makeinfo(inside, outside, lambda1, lambda2, <int>u.shape[1],<int>u.shape[0], 0)
        self.c_obj = Snakes2D(&image[0,0], &u[0,0], conf)
        self.c_obj.initialize()
        self.c_level_set = u

    def iterate(self, iterations=1, smoothing = 1):
        for n in range(iterations):
            self.c_obj.evolve()
            for _ in range(smoothing):
                self.c_obj.smooth()  

    def smooth(self):
        self.c_obj.smooth()

    def evolve(self):
        self.c_obj.evolve()

    def set_data(self,  image):
        print('init image',image.shape, image.dtype)
        if image.dtype != 'float64':
            image = image.astype('float64')  
        self._set_data(image) 

    cdef _set_data(self, double[:, ::1] image):
        self.c_obj.set_data(&image[0,0])

    @property
    def level_set(self):
        return np.asarray(self.c_level_set,dtype = np.int8)





 



 