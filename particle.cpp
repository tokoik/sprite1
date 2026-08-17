/*
** 重力の影響を受けるパーティクル
*/
#include <stdlib.h>

#include "particle.h"

double particle::g = -0.0001;
double particle::h = -1.0;
double particle::a = 0.5;

particle::particle()
{
  p[0] = 0.0;
  p[1] = 1.0;
  p[2] = 0.0;
  v[0] = ((double)rand() / (double)RAND_MAX - 0.5) * 0.005;
  v[1] = ((double)rand() / (double)RAND_MAX) * 0.01;
  v[2] = ((double)rand() / (double)RAND_MAX - 0.5) * 0.005;
}

particle::particle(double p[3], double v[3])
{
  this->p[0] = p[0];
  this->p[1] = p[1];
  this->p[2] = p[2];
  this->v[0] = v[0];
  this->v[1] = v[1];
  this->v[2] = v[2];
};

void particle::gravity(double g)
{
  particle::g = g;
}

void particle::height(double h)
{
  particle::h = h;
}

void particle::attenuation(double a)
{
  particle::a = a;
}

void particle::update(void)
{
  p[0] += v[0];
  v[1] += g;
  p[1] += v[1];
  if (p[1] < h) {
    p[1] = h;
    v[1] = -a * v[1];
  }
  p[2] += v[2];
}

const double *particle::getPosition(void)
{
  return p;
}
