#ifndef SNAKES2D_H
#define SNAKES2D_H

#include "morphosnakes.h"

class Snakes2D
{
public:
    Snakes2D();
    Snakes2D(double *, uint8_t *, snakes_info);
    ~Snakes2D();

    void initialize();

    void evolve();
    void smooth();
    void iterate();

    std::vector<point2d> m_points; // Edge points
    double *m_image;
    uint8_t *m_levelset;
    snakes_info m_conf;

    void set_data(double * image);

private:
    int smooth_counter = 0;
    std::vector<long> m_counter;
};

#endif