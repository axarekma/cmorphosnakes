#include "Snakes2D.h"
#include <iostream>

// Default constructor
Snakes2D::Snakes2D() {}

Snakes2D::Snakes2D(double const *image, uint8_t *levelset, snakes_info conf)
{
    this->m_image = image;
    this->m_levelset = levelset;
    this->m_conf = conf;
}

// Destructor
Snakes2D::~Snakes2D() {}

void Snakes2D::initialize()
{
    m_points = get_edge_list_2d(m_levelset, m_conf);
    m_counter = std::vector<long>((__int64)m_conf.nx * m_conf.ny + 1, {0});
}

void Snakes2D::evolve()
{
    evolve_edge_2d(m_image,
                   m_levelset,
                   m_counter.data(),
                   m_points, m_conf);
}
void Snakes2D::smooth()
{
    if (smooth_counter % 2 == 0)
    {
        fast_marching_dilation_2d(m_points, m_levelset, m_counter.data(), m_conf);
        fast_marching_erosion_2d(m_points, m_levelset, m_counter.data(), m_conf);
    }
    else
    {
        fast_marching_erosion_2d(m_points, m_levelset, m_counter.data(), m_conf);
        fast_marching_dilation_2d(m_points, m_levelset, m_counter.data(), m_conf);
    }
    smooth_counter++;
}
