#pragma once

#include "snakes2d_operators.h"
#include "snakes2d_operators_bidir.h"
#include "snakes3d_operators.h"
#include "snakes3d_operators_bidir.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num() { return 0; }
inline omp_int_t omp_get_num_threads() { return 1; }
#endif

typedef struct snakes_info
{
  int inside_label, outside_label;
  double lambda1, lambda2;
  int nx, ny, nz;
} snakes_info;

// function that I can call from cython as a pseudoconstructor
snakes_info makeinfo(int inside_label, int outside_label, double lambda1,
                     double lambda2, int nx, int ny, int nz)
{
  snakes_info const conf = {inside_label, outside_label, lambda1, lambda2, nx, ny, nz};
  return conf;
}

typedef struct point3d
{
  int x, y, z;
} point3d;

typedef struct point2d
{
  int x, y;
} point2d;

bool is_inside(int const t_xi, int const t_L) { return (t_xi >= 0) && (t_xi < t_L); }

bool is_central(int const t_xi, int const t_L)
{
  return (t_xi > 0) && (t_xi < t_L - 1);
}

bool SId_2d_borders(int const xi, int const yi, uint8_t const *levelset,
                    snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val = conf.inside_label;
  // All diagonal elements for the borders are zero
  // only non-diagonal masks are relevant
  int const stride_x = 1;
  int const stride_y = nx;
  int const index = xi + stride_y * yi;

  if (is_central(xi, nx))
  {
    // mask along x is valid
    return SId_2d_1(levelset, index, stride_x, stride_y, val);
  }
  if (is_central(yi, ny))
  {
    // mask along y is valid
    return SId_2d_3(levelset, index, stride_x, stride_y, val);
  }
  return false;
}

bool SId_3d_borders(int const xi, int const yi, int const zi, uint8_t const *levelset,
                    snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val = conf.inside_label;
  // All diagonal elements for the borders are zero
  // only non-diagonal masks are relevant
  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;
  int const index = xi + stride_y * yi + stride_z * zi;

  if (is_central(xi, nx) && is_central(yi, ny))
  {
    // mask along x is valid
    return SId_3d_2(levelset, index, stride_x, stride_y, stride_z, val);
  }
  if (is_central(yi, ny) && is_central(zi, nz))
  {
    // mask along y is valid
    return SId_3d_0(levelset, index, stride_x, stride_y, stride_z, val);
  }
  if (is_central(zi, nz) && is_central(xi, nx))
  {
    // mask along z is valid
    return SId_3d_1(levelset, index, stride_x, stride_y, stride_z, val);
  }
  return false;
}

bool ISd_2d_borders(int const xi, int yi, uint8_t *levelset, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val = conf.inside_label;
  // I defined a bidirectional function for the dilations,
  // outside coordinates are mapped back to INDEX, i.e. constant
  // boundary conditions with value = levelset[index]
  int nstride_x = (xi == 0) ? 0 : 1;
  int nstride_y = (yi == 0) ? 0 : nx;

  int stride_x = (xi == nx - 1) ? 0 : 1;
  int stride_y = (yi == ny - 1) ? 0 : nx;

  int const index = xi + nx * yi;
  return ISd_2d_any_bidir(levelset, index, nstride_x, stride_x, nstride_y, stride_y,
                          val);
}

bool ISd_3d_borders(int const xi, int yi, int zi, uint8_t *levelset,
                    snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val = conf.inside_label;
  // I defined a bidirectional function for the dilations,
  // outside coordinates are mapped back to INDEX, i.e. constant
  // boundary conditions with value = levelset[index]
  int nstride_x = (xi == 0) ? 0 : 1;
  int nstride_y = (yi == 0) ? 0 : nx;
  int nstride_z = (zi == 0) ? 0 : nx * ny;

  int stride_x = (xi == nx - 1) ? 0 : 1;
  int stride_y = (yi == ny - 1) ? 0 : nx;
  int stride_z = (zi == nz - 1) ? 0 : nx * ny;

  int const index = xi + nx * yi + nx * ny * zi;

  return ISd_3d_any_bidir(levelset, index, nstride_x, stride_x, nstride_y, stride_y,
                          nstride_z, stride_z, val);
}

double masked_average(double const *image, uint8_t const *mask, int const size,
                      int const value)
{
  double cumsum = 0;
  int counter = 0;
#pragma omp parallel for shared(image, mask) reduction(+                     \
                                                       : cumsum) reduction(+ \
                                                                           : counter)
  for (long i = 0; i < size; ++i)
  {
    if (mask[i] == value)
    {
      cumsum += image[i];
      counter++;
    }
  }
  if (counter == 0)
  {
    return {0};
  }
  return cumsum / counter;
}

// The parallel processing of new points based on
// https://stackoverflow.com/a/36356923
template <class T>
void reduce(std::vector<T> *v1, int const begin, int const end)
{
  if (end - begin == 1)
    return;
  int pivot = (begin + end) / 2;
  //#pragma omp task
  reduce(v1, begin, pivot);
  //#pragma omp task
  reduce(v1, pivot, end);
  //#pragma omp taskwait
  v1[begin].insert(v1[begin].end(), v1[pivot].begin(), v1[pivot].end());
}

namespace pysnakes2d
{

bool is_edge_to_val(uint8_t const *levelset, point2d const point, int const nx,
                    int const ny, int const val)
{
  int const xi = point.x;
  int const yi = point.y;
  int const stride_x = 1;
  int const stride_y = nx;
  // Edges are only inside image
  if (xi < 0 || yi < 0 || xi > nx - 1 || yi > ny - 1)
  {
    return false;
  }
  int const index = xi + stride_y * yi;
  // edge pixels are defined as the neighbouring pixels
  if (levelset[index] == val)
  {
    return false;
  }
  int const index_left = index - (xi > 0) * stride_x;
  int const index_right = index + (xi < (nx - 1)) * stride_x;
  int const index_down = index - (yi > 0) * stride_y;
  int const index_up = index + (yi < (ny - 1)) * stride_y;
  if (levelset[index_left] == val || levelset[index_right] == val ||
      levelset[index_down] == val || levelset[index_up] == val)
  {
    return true;
  }
  return false;
}

bool is_edge(uint8_t const *levelset, point2d const point, snakes_info const conf)
{
  return is_edge_to_val(levelset, point, conf.nx, conf.ny, conf.inside_label);
}

bool is_edge_to_outside(uint8_t const *levelset, point2d const point,
                        snakes_info const conf)
{
  return is_edge_to_val(levelset, point, conf.nx, conf.ny, conf.outside_label);
}

void update_edge(uint8_t const *levelset, long *counter,
                 std::vector<point2d> &edge_points, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val = conf.inside_label;

  counter[nx * ny] += 1;
  int const current_iteration = counter[nx * ny];

  int const stride_x = 1;
  int const stride_y = nx;

  std::vector<point2d> new_edge;
  for (const point2d &point : edge_points)
  {
    int const index = point.x + stride_y * point.y;
    if (counter[index] != current_iteration)
    {
      counter[index] = current_iteration;
      if (is_edge(levelset, point, conf))
      {
        new_edge.push_back(point);
      }
    }
  }
  edge_points = new_edge;
}

void check_and_add_edges(std::vector<point2d> &edge_points,
                         std::vector<point2d> const &unchecked_points,
                         uint8_t const *levelset, long *counter,
                         snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val_in = conf.inside_label;
  int const val_out = conf.outside_label;

  int const current_iteration = counter[nx * ny];

  int const stride_x = 1;
  int const stride_y = nx;

  for (const point2d &p : unchecked_points)
  {
    for (int yi = p.y - 1; yi < p.y + 2; yi++)
    {
      if (is_inside(yi, ny))
      {
        for (int xi = p.x - 1; xi < p.x + 2; xi++)
        {
          if (is_inside(xi, nx))
          {
            int index = xi + stride_y * yi;
            if (counter[index] != current_iteration)
            {
              counter[index] = current_iteration;

              int index_left = index - (xi > 0) * stride_x;
              int index_right = index + (xi < (nx - 1)) * stride_x;
              int index_down = index - (yi > 0) * stride_y;
              int index_up = index + (yi < (ny - 1)) * stride_y;

              if (levelset[index] == val_out &&
                  (levelset[index_left] == val_in || levelset[index_right] == val_in ||
                   levelset[index_down] == val_in || levelset[index_up] == val_in))
              {
                point2d edge = {xi, yi};
                edge_points.push_back(edge);
              }
            }
          }
        }
      }
    }
  }
  return;
}
} // namespace pysnakes2d

void evolve_edge_2d(double const *image, uint8_t *levelset, long *counter,
                    std::vector<point2d> &edge_points, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;

  double lambda1 = conf.lambda1;
  double lambda2 = conf.lambda2;

  int const stride_x = 1;
  int const stride_y = nx;

  double c0 = masked_average(image, levelset, nx * ny, conf.outside_label);
  double c1 = masked_average(image, levelset, nx * ny, conf.inside_label);

  counter[nx * ny] += 1;
  int const current_iteration = counter[nx * ny];

  std::vector<point2d> changed_add, changed_remove;
  std::vector<point2d> *changed_add_p, *changed_remove_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_add_p = new std::vector<point2d>[omp_get_num_threads()];
      changed_remove_p = new std::vector<point2d>[omp_get_num_threads()];
    }

    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);
#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point2d p = edge_points[pi];
      for (int yi = p.y - 1; yi < p.y + 2; yi++)
      {
        if (is_central(yi, ny))
        {
          for (int xi = p.x - 1; xi < p.x + 2; xi++)
          {
            if (is_central(xi, nx))
            {
              int const index = xi + stride_y * yi;

              if (counter[index] != current_iteration)
              {
                // possible race, cleared up at check
                counter[index] = current_iteration;

                bool gx = levelset[index + stride_x] != levelset[index - stride_x];
                bool gy = levelset[index + stride_y] != levelset[index - stride_y];

                bool abs_grad = (gx || gy);
                double value = image[index];
                double aux =
                    abs_grad
                        ? (lambda1 * pow(value - c1, 2) - lambda2 * pow(value - c0, 2))
                        : 0.0;

                if (aux < 0 && levelset[index] == conf.outside_label)
                {
                  point2d point = {xi, yi};
                  changed_add_p[omp_get_thread_num()].push_back(point);
                }
                if (aux > 0 && levelset[index] == conf.inside_label)
                {
                  point2d point = {xi, yi};
                  changed_remove_p[omp_get_thread_num()].push_back(point);
                }
              }
            }
          }
        }
      }
    }
#pragma omp single
    {
      reduce(changed_add_p, 0, omp_get_num_threads());
      reduce(changed_remove_p, 0, omp_get_num_threads());
    }
  }
  changed_add = changed_add_p[0], changed_remove = changed_remove_p[0];
  delete[] changed_add_p;
  delete[] changed_remove_p;

  for (const point2d &point : changed_add)
  {
    levelset[point.x + stride_y * point.y] = conf.inside_label;
  }
  for (const point2d &point : changed_remove)
  {
    levelset[point.x + stride_y * point.y] = conf.outside_label;
  }

  pysnakes2d::update_edge(levelset, counter, edge_points, conf);
  pysnakes2d::check_and_add_edges(edge_points, changed_remove, levelset, counter, conf);
  pysnakes2d::check_and_add_edges(edge_points, changed_add, levelset, counter, conf);
  return;
}

std::vector<point2d> get_edge_list_2d(uint8_t const *levelset, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val_in = conf.inside_label;
  int const val_out = conf.outside_label;

  int const stride_x = 1;
  int const stride_y = nx;

  std::vector<point2d> retval;
  for (int yi = 0; yi < ny; yi++)
  {
    for (int xi = 0; xi < nx; xi++)
    {

      int index = xi + stride_y * yi;
      int index_left = index - (xi > 0) * stride_x;
      int index_right = index + (xi < (nx - 1)) * stride_x;
      int index_down = index - (yi > 0) * stride_y;
      int index_up = index + (yi < (ny - 1)) * stride_y;
      if (levelset[index] == val_out &&
          (levelset[index_left] == val_in || levelset[index_right] == val_in ||
           levelset[index_down] == val_in || levelset[index_up] == val_in))
      {

        point2d edge = {xi, yi};
        retval.push_back(edge);
      }
    }
  }

  return retval;
}

void fast_marching_erosion_2d(std::vector<point2d> &edge_points, uint8_t *levelset,
                              long *counter, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const val = conf.inside_label;
  int const stride_x = 1;
  int const stride_y = nx;

  counter[nx * ny] += 1;
  int current_iteration = counter[nx * ny];

  std::vector<point2d> changed;
  std::vector<point2d> *changed_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_p = new std::vector<point2d>[omp_get_num_threads()];
    }

    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);

#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point2d p = edge_points[pi];
      for (int yi = p.y - 1; yi < p.y + 2; yi++)
      {
        if (is_inside(yi, ny))
        {
          for (int xi = p.x - 1; xi < p.x + 2; xi++)
          {
            if (is_inside(xi, nx))
            {
              int const index = xi + stride_y * yi;
              if (counter[index] != current_iteration)
              {
                counter[index] = current_iteration;
                // // apply erosion only on 1
                if (levelset[index] == val)
                {

                  if (is_central(xi, nx) && is_central(yi, ny))
                  {
                    // normal case
                    if (SId_2d_any(levelset, index, stride_x, stride_y, val) == false)
                    {
                      point2d point = {xi, yi};
                      changed_p[omp_get_thread_num()].push_back(point);
                    }
                  }
                  else if (SId_2d_borders(xi, yi, levelset, conf) == false)
                  {
                    point2d point = {xi, yi};
                    changed_p[omp_get_thread_num()].push_back(point);
                  }
                }
              }
            }
          }
        }
      }
    }

#pragma omp single
    {
      reduce(changed_p, 0, omp_get_num_threads());
    }
  }
  changed = changed_p[0];
  delete[] changed_p;

  if (changed.size() == 0)
    return;

  for (const point2d &point : changed)
  {
    // true erosion points are borders of current outside label
    if (pysnakes2d::is_edge_to_outside(levelset, point, conf))
    {
      levelset[point.x + stride_y * point.y] = conf.outside_label;
    }
  }

  pysnakes2d::update_edge(levelset, counter, edge_points, conf);
  pysnakes2d::check_and_add_edges(edge_points, changed, levelset, counter, conf);
  return;
}

void fast_marching_dilation_2d(std::vector<point2d> &edge_points, uint8_t *levelset,
                               long *counter, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val_in = conf.inside_label;
  int const val_out = conf.outside_label;

  int const stride_x = 1;
  int const stride_y = nx;

  counter[nx * ny] += 1;
  int current_iteration = counter[nx * ny];

  std::vector<point2d> changed;
  std::vector<point2d> *changed_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_p = new std::vector<point2d>[omp_get_num_threads()];
    }

    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);
#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point2d p = edge_points[pi];
      for (int yi = p.y - 1; yi < p.y + 2; yi++)
      {
        if (is_inside(yi, ny))
        {
          for (int xi = p.x - 1; xi < p.x + 2; xi++)
          {
            if (is_inside(xi, nx))
            {
              int const index = xi + stride_y * yi;
              if (counter[index] != current_iteration)
              {
                counter[index] = current_iteration;
                // apply dilation only on 0
                if (levelset[index] == val_out)
                {
                  if (is_central(xi, nx) && is_central(yi, ny))
                  {
                    // normal case
                    if (ISd_2d_any(levelset, index, stride_x, stride_y, val_in) ==
                        true)
                    {
                      point2d point = {xi, yi};
                      changed_p[omp_get_thread_num()].push_back(point);
                    }
                  }
                  else if (ISd_2d_borders(xi, yi, levelset, conf) == true)
                  {
                    point2d point = {xi, yi};
                    changed_p[omp_get_thread_num()].push_back(point);
                  }
                }
              }
            }
          }
        }
      }
    }

#pragma omp single
    {
      reduce(changed_p, 0, omp_get_num_threads());
    }
  }
  changed = changed_p[0];
  delete[] changed_p;

  if (changed.size() == 0)
    return;

  for (const point2d &point : changed)
  {
    levelset[point.x + stride_y * point.y] = conf.inside_label;
  }

  pysnakes2d::update_edge(levelset, counter, edge_points, conf);
  pysnakes2d::check_and_add_edges(edge_points, changed, levelset, counter, conf);
  return;
}

namespace pysnakes3d
{

bool is_edge_to_val(uint8_t const *levelset, point3d const point, int const nx,
                    int const ny, int const nz, int const val)
{
  int const xi = point.x;
  int const yi = point.y;
  int const zi = point.z;
  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;
  int const index = xi + stride_y * yi + stride_z * zi;
  // Edges are only inside image
  if (xi < 0 || yi < 0 || zi < 0 || xi > nx - 1 || yi > ny - 1 || zi > nz - 1)
  {
    return false;
  }
  // edge pixels are defined as the neighbouring pixels
  if (levelset[index] == val)
  {
    return false;
  }
  int const index_x0 = index - (xi > 0) * stride_x;
  int const index_x1 = index + (xi < (nx - 1)) * stride_x;
  int const index_y0 = index - (yi > 0) * stride_y;
  int const index_y1 = index + (yi < (ny - 1)) * stride_y;
  int const index_z0 = index - (zi > 0) * stride_z;
  int const index_z1 = index + (zi < (nz - 1)) * stride_z;
  if ((levelset[index_x0] == val) || (levelset[index_x1] == val) ||
      (levelset[index_y0] == val) || (levelset[index_y1] == val) ||
      (levelset[index_z0] == val) || (levelset[index_z1] == val))
  {
    return true;
  }

  return false;
}
bool is_edge(uint8_t const *levelset, point3d const point, snakes_info const conf)
{
  return is_edge_to_val(levelset, point, conf.nx, conf.ny, conf.nz, conf.inside_label);
}

bool is_edge_to_outside(uint8_t const *levelset, point3d const point,
                        snakes_info const conf)
{
  return is_edge_to_val(levelset, point, conf.nx, conf.ny, conf.nz, conf.outside_label);
}

void update_edge(uint8_t const *levelset, long *counter,
                 std::vector<point3d> &edge_points, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  counter[nx * ny * nz] += 1;
  int current_iteration = counter[nx * ny * nz];

  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;

  std::vector<point3d> new_edge;
  new_edge.reserve(edge_points.size());

  for (const point3d &point : edge_points)
  {
    int const index = point.x + stride_y * point.y + stride_z * point.z;
    if (counter[index] != current_iteration)
    {
      counter[index] = current_iteration;
      if (is_edge(levelset, point, conf))
      {
        new_edge.push_back(point);
      }
    }
  }
  edge_points = new_edge;
}

void check_and_add_edges(std::vector<point3d> &edge_points,
                         std::vector<point3d> const &unchecked_points,
                         uint8_t const *levelset, long *counter,
                         snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val_in = conf.inside_label;
  int const val_out = conf.outside_label;
  counter[nx * ny * nz] += 1;
  int current_iteration = counter[nx * ny * nz];

  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;

  if (edge_points.capacity() < edge_points.size() + unchecked_points.size())
  {
    edge_points.reserve(edge_points.size() + unchecked_points.size());
  }

  for (const point3d &p : unchecked_points)
  {
    for (int zi = p.z - 1; zi < p.z + 2; zi++)
    {
      if (is_inside(zi, nz))
      {
        for (int yi = p.y - 1; yi < p.y + 2; yi++)
        {
          if (is_inside(yi, ny))
          {
            for (int xi = p.x - 1; xi < p.x + 2; xi++)
            {
              if (is_inside(xi, nx))
              {
                int const index = xi + stride_y * yi + stride_z * zi;
                if (counter[index] != current_iteration)
                {
                  counter[index] = current_iteration;

                  int const index_x0 = index - (xi > 0) * stride_x;
                  int const index_x1 = index + (xi < (nx - 1)) * stride_x;
                  int const index_y0 = index - (yi > 0) * stride_y;
                  int const index_y1 = index + (yi < (ny - 1)) * stride_y;
                  int const index_z0 = index - (zi > 0) * stride_z;
                  int const index_z1 = index + (zi < (nz - 1)) * stride_z;
                  if (levelset[index] == val_out &&
                      (levelset[index_x0] == val_in || levelset[index_x1] == val_in ||
                       levelset[index_y0] == val_in || levelset[index_y1] == val_in ||
                       levelset[index_z0] == val_in || levelset[index_z1] == val_in))
                  {

                    point3d edge = {xi, yi, zi};
                    edge_points.push_back(edge);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return;
}

} // namespace pysnakes3d

void evolve_edge_3d(double const *image, uint8_t *levelset, long *counter,
                    std::vector<point3d> &edge_points, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;

  double lambda1 = conf.lambda1;
  double lambda2 = conf.lambda2;

  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;

  double c0 = masked_average(image, levelset, nx * ny * nz, conf.outside_label);
  double c1 = masked_average(image, levelset, nx * ny * nz, conf.inside_label);

  counter[nx * ny * nz] += 1;
  int current_iteration = counter[nx * ny * nz];

  std::vector<point3d> changed_add, changed_remove;
  std::vector<point3d> *changed_add_p, *changed_remove_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_add_p = new std::vector<point3d>[omp_get_num_threads()];
      changed_remove_p = new std::vector<point3d>[omp_get_num_threads()];
    }

    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);
#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point3d p = edge_points[pi];
      for (int zi = p.z - 1; zi < p.z + 2; zi++)
      {
        if (is_central(zi, nz))
        {
          for (int yi = p.y - 1; yi < p.y + 2; yi++)
          {
            if (is_central(yi, ny))
            {
              for (int xi = p.x - 1; xi < p.x + 2; xi++)
              {
                if (is_central(xi, nx))
                {
                  int const index = xi + stride_y * yi + stride_z * zi;

                  if (counter[index] != current_iteration)
                  {
                    // possible race gets cleared up at at check_and_add_edges
                    counter[index] = current_iteration;

                    bool gx = levelset[index + stride_x] != levelset[index - stride_x];
                    bool gy = levelset[index + stride_y] != levelset[index - stride_y];
                    bool gz = levelset[index + stride_z] != levelset[index - stride_z];

                    double abs_grad = (gx || gy || gz);
                    double value = image[index];
                    double aux = abs_grad ? (lambda1 * pow(value - c1, 2) -
                                             lambda2 * pow(value - c0, 2))
                                          : 0.0;

                    if (aux < 0 && levelset[index] == conf.outside_label)
                    {
                      point3d point = {xi, yi, zi};
                      changed_add_p[omp_get_thread_num()].push_back(point);
                    }
                    if (aux > 0 && levelset[index] == conf.inside_label)
                    {
                      point3d point = {xi, yi, zi};
                      changed_remove_p[omp_get_thread_num()].push_back(point);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
#pragma omp single
    {
      reduce(changed_add_p, 0, omp_get_num_threads());
      reduce(changed_remove_p, 0, omp_get_num_threads());
    }
  }
  changed_add = changed_add_p[0], changed_remove = changed_remove_p[0];
  delete[] changed_add_p;
  delete[] changed_remove_p;

  // std::cout << "evolve_edge_3d  \n";
  // std::cout << "   changed_add " << changed_add.size() << '\n';
  // std::cout << "   changed_remove " << changed_remove.size() << '\n';

  for (const point3d &point : changed_add)
  {
    levelset[point.x + stride_y * point.y + stride_z * point.z] = conf.inside_label;
  }
  for (const point3d &point : changed_remove)
  {
    levelset[point.x + stride_y * point.y + stride_z * point.z] = conf.outside_label;
  }

  pysnakes3d::update_edge(levelset, counter, edge_points, conf);
  pysnakes3d::check_and_add_edges(edge_points, changed_remove, levelset, counter, conf);
  pysnakes3d::check_and_add_edges(edge_points, changed_add, levelset, counter, conf);
  return;
}

void fast_marching_erosion_3d(std::vector<point3d> &edge_points, uint8_t *levelset,
                              long *counter, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val = conf.inside_label;
  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;

  counter[nx * ny * nz] += 1;
  int current_iteration = counter[nx * ny * nz];

  std::vector<point3d> changed;
  std::vector<point3d> *changed_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_p = new std::vector<point3d>[omp_get_num_threads()];
    }

    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);
#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point3d point = edge_points[pi];
      for (int k = -1; k < 2; k++)
      {
        int zi = point.z + k;
        if (is_inside(zi, nz))
        {
          for (int j = -1; j < 2; j++)
          {
            int yi = point.y + j;
            if (is_inside(yi, ny))
            {
              for (int i = -1; i < 2; i++)
              {
                int xi = point.x + i;
                if (is_inside(xi, nx))
                {
                  int const index = xi + stride_y * yi + stride_z * zi;
                  if (counter[index] != current_iteration)
                  {
                    counter[index] = current_iteration;
                    // // apply erosion only on inside
                    if (levelset[index] == val)
                    {
                      if (is_central(xi, nx) && is_central(yi, ny) &&
                          is_central(zi, nz))
                      {
                        // normal case
                        if (SId_3d_any(levelset, index, stride_x, stride_y, stride_z,
                                       val) == false)
                        {
                          point3d point = {xi, yi, zi};
                          changed_p[omp_get_thread_num()].push_back(point);
                        }
                      }
                      else if (SId_3d_borders(xi, yi, zi, levelset, conf) == false)
                      {
                        point3d point = {xi, yi, zi};
                        changed_p[omp_get_thread_num()].push_back(point);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

#pragma omp single
    {
      reduce(changed_p, 0, omp_get_num_threads());
    }
  }
  changed = changed_p[0];
  delete[] changed_p;

  if (changed.size() == 0)
    return;

  for (const point3d &point : changed)
  {
    // true erosion points are borders of current outside label
    if (pysnakes3d::is_edge_to_outside(levelset, point, conf))
    {
      levelset[point.x + stride_y * point.y + stride_z * point.z] = conf.outside_label;
    }
  }

  pysnakes3d::update_edge(levelset, counter, edge_points, conf);
  pysnakes3d::check_and_add_edges(edge_points, changed, levelset, counter, conf);
  return;
}

void fast_marching_dilation_3d(std::vector<point3d> &edge_points, uint8_t *levelset,
                               long *counter, snakes_info const conf)
{
  int const nx = conf.nx;
  int const ny = conf.ny;
  int const nz = conf.nz;
  int const val_in = conf.inside_label;
  int const val_out = conf.outside_label;

  int const stride_x = 1;
  int const stride_y = nx;
  int const stride_z = nx * ny;

  counter[nx * ny * nz] += 1;
  int current_iteration = counter[nx * ny * nz];

  std::vector<point3d> changed;
  std::vector<point3d> *changed_p;

#pragma omp parallel
  {
#pragma omp single
    {
      changed_p = new std::vector<point3d>[omp_get_num_threads()];
    }
    // cast to int to be compatible with OpenMP 2.0
    assert(edge_points.size() < INT_MAX / 2);
#pragma omp for
    for (int pi = 0; pi < static_cast<int>(edge_points.size()); pi++)
    {
      point3d p = edge_points[pi];
      for (int zi = p.z - 1; zi < p.z + 2; zi++)
      {
        if (is_inside(zi, nz))
        {
          for (int yi = p.y - 1; yi < p.y + 2; yi++)
          {
            if (is_inside(yi, ny))
            {
              for (int xi = p.x - 1; xi < p.x + 2; xi++)
              {
                if (is_inside(xi, nx))
                {

                  int const index = xi + stride_y * yi + stride_z * zi;
                  if (counter[index] != current_iteration)
                  {
                    counter[index] = current_iteration;

                    // apply dilation only on outside
                    if (levelset[index] == val_out)
                    {
                      if (is_central(xi, nx) && is_central(yi, ny) &&
                          is_central(zi, nz))
                      {

                        // normal case
                        if (ISd_3d_any(levelset, index, stride_x, stride_y, stride_z,
                                       val_in) == true)
                        {
                          point3d point = {xi, yi, zi};
                          changed_p[omp_get_thread_num()].push_back(point);
                        }
                      }
                      else if (ISd_3d_borders(xi, yi, zi, levelset, conf) == true)
                      {
                        point3d point = {xi, yi, zi};
                        changed_p[omp_get_thread_num()].push_back(point);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
#pragma omp single
    {
      reduce(changed_p, 0, omp_get_num_threads());
    }
  }
  changed = changed_p[0];
  delete[] changed_p;

  if (changed.size() == 0)
    return;

  for (const point3d &point : changed)
  {
    levelset[point.x + stride_y * point.y + stride_z * point.z] = conf.inside_label;
  }

  pysnakes3d::update_edge(levelset, counter, edge_points, conf);
  pysnakes3d::check_and_add_edges(edge_points, changed, levelset, counter, conf);
  return;
}

std::vector<point3d> get_edge_list_3d(uint8_t const *levelset, snakes_info const conf)
{

  int const stride_x = 1;
  int const stride_y = conf.nx;
  int const stride_z = conf.nx * conf.ny;

  std::vector<point3d> retval;
  for (int zi = 0; zi < conf.nz; zi++)
  {
    for (int yi = 0; yi < conf.ny; yi++)
    {
      for (int xi = 0; xi < conf.nx; xi++)
      {
        int index = xi + stride_y * yi + stride_z * zi;
        int const index_x0 = index - (xi > 0) * stride_x;
        int const index_x1 = index + (xi < (conf.nx - 1)) * stride_x;
        int const index_y0 = index - (yi > 0) * stride_y;
        int const index_y1 = index + (yi < (conf.ny - 1)) * stride_y;
        int const index_z0 = index - (zi > 0) * stride_z;
        int const index_z1 = index + (zi < (conf.nz - 1)) * stride_z;
        if (levelset[index] == conf.outside_label &&
            (levelset[index_x0] == conf.inside_label ||
             levelset[index_x1] == conf.inside_label ||
             levelset[index_y0] == conf.inside_label ||
             levelset[index_y1] == conf.inside_label ||
             levelset[index_z0] == conf.inside_label ||
             levelset[index_z1] == conf.inside_label))
        {
          point3d edge = {xi, yi, zi};
          retval.push_back(edge);
        }
      }
    }
  }
  return retval;
}

struct sortfunc
{
  bool operator()(point2d const &left, point2d const &right) const
  {
    if (left.y != right.y)
    {
      return left.y < right.y;
    }
    return left.x < right.x;
  }
  bool operator()(point3d const &left, point3d const &right) const
  {
    if (left.z != right.z)
    {
      return left.z < right.z;
    }
    if (left.y != right.y)
    {
      return left.y < right.y;
    }
    return left.x < right.x;
  }
};

void sort_edge2d(std::vector<point2d> &edge)
{
  std::sort(edge.begin(), edge.end(), sortfunc());

  return;
}
void sort_edge3d(std::vector<point3d> &edge)
{
  std::sort(edge.begin(), edge.end(), sortfunc());

  return;
}
