#include "Snakes3D.h"

// Default constructor
Snakes3D::Snakes3D() {}

Snakes3D::Snakes3D(double const *image, uint8_t *levelset, snakes_info conf)
{
    this->m_image = image;
    this->m_levelset = levelset;
    this->m_conf = conf;
}

// Destructor
Snakes3D::~Snakes3D() {}

void Snakes3D::initialize()
{
    m_points = get_edge_list_3d(m_levelset, m_conf);
    m_counter = std::vector<long>((__int64)m_conf.nx * m_conf.ny * m_conf.nz + 1, {0});
}

void Snakes3D::evolve()
{
    evolve_edge_3d(m_image,
                   m_levelset,
                   m_counter.data(),
                   m_points, m_conf);
}
void Snakes3D::smooth()
{

    if (smooth_counter % 2 == 0)
    {
        fast_marching_dilation_3d(m_points, m_levelset, m_counter.data(), m_conf);
        fast_marching_erosion_3d(m_points, m_levelset, m_counter.data(), m_conf);
    }
    else
    {
        fast_marching_erosion_3d(m_points, m_levelset, m_counter.data(), m_conf);
        fast_marching_dilation_3d(m_points, m_levelset, m_counter.data(), m_conf);
    }
    smooth_counter++;
}
