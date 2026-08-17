#ifndef PARTICLE_H
#define PARTICLE_H

class particle {
  double p[3]; // 位置
  double v[3]; // 速度
  static double g; // 重力加速度
  static double h; // 落下面の高さ
  static double a; // 減衰率
 public:
  particle();
  particle(double p[3], double v[3]);
  ~particle() {};
  static void gravity(double);
  static void height(double);
  static void attenuation(double);
  void update(void);
  const double *getPosition(void);
};

#endif
