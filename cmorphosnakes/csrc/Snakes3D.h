#ifndef SNAKES3D_H
#define SNAKES3D_H

#include "morphosnakes.h"

class Snakes3D
{
public:
    Snakes3D();
    Snakes3D(double const *, uint8_t *, snakes_info);
    ~Snakes3D();

    void initialize();

    void evolve();
    void smooth();
    void iterate();

    std::vector<point3d> m_points; // Edge points
    double const *m_image;
    uint8_t *m_levelset;
    snakes_info m_conf;

private:
    int smooth_counter = 0;
    std::vector<long> m_counter;
};

#endif