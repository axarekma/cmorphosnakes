# distutils: sources = Snakes3D.cpp

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

    
cdef extern from "csrc/Snakes3D.cpp":
    pass
cdef extern from "csrc/Snakes2D.cpp":
    pass

cdef extern from "csrc/Snakes3D.h":

    cdef cppclass Snakes3D:
        Snakes3D() except +
        Snakes3D(double*, unsigned char*, snakes_info) except +

        void initialize()
        void set_data(double*)
        void evolve()
        void smooth()

cdef extern from "csrc/Snakes2D.h":

    cdef cppclass Snakes2D:
        Snakes2D() except +
        Snakes2D(double*, unsigned char*, snakes_info) except +

        void initialize()
        void set_data(double*)
        void evolve()
        void smooth()

      