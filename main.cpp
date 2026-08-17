#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
//#  pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#  include "glut.h"
#  include "glext.h"
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

/*
** 光源
*/
static const GLfloat lightpos[] = { 0.0, 0.0, 1.0, 0.0 }; /* 位置　　　 */
static const GLfloat lightcol[] = { 1.0, 1.0, 1.0, 1.0 }; /* 直接光強度 */
static const GLfloat lightamb[] = { 0.1, 0.1, 0.1, 1.0 }; /* 環境光強度 */

/*
** パーティクル
*/
#include "particle.h"
#include <deque>
#define MAX_PARTICLES 800
static std::deque<particle> particles;
static GLfloat psize;
static GLfloat distance[] = { 0.0, 0.0, 1.0 };

/*
** 地面の高さ
*/
#define HEIGHT -1.0

/*
** テクスチャ
*/
#define TEXWIDTH  64                       /* テクスチャの幅　　　 */
#define TEXHEIGHT 64                       /* テクスチャの高さ　　 */
static const char texture[] = "ball.raw";  /* テクスチャファイル名 */

/*
** 初期化
*/
static void init(void)
{
#if defined(WIN32)
  glPointParameterfv =
    (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfv");
#endif

  /* テクスチャの読み込みに使う配列 */
  GLubyte image[TEXHEIGHT][TEXWIDTH][4];
  
  /* テクスチャ画像の読み込み */
  std::ifstream file(texture, std::ios::in | std::ios::binary);
  if (file) {
    file.read((char *)image, sizeof image);
    file.close();
  }
  else {
    std::cerr << texture << " が開けません" << std::endl;
  }
  
  /* テクスチャ画像はバイト単位に詰め込まれている */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  
  /* テクスチャを拡大・縮小する方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  
  /* テクスチャの繰り返し方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
  /* テクスチャ環境 */
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  
  /* テクスチャの割り当て */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, TEXHEIGHT, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, image);

  /* アルファテストの判別関数 */
  glAlphaFunc(GL_GREATER, 0.5);

  /* 距離に対する点の大きさの制御 */
  glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);

  /* 初期設定 */
  glClearColor(0.3, 0.3, 1.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  /* 陰影付けを無効にする */
  glDisable(GL_LIGHTING);

  /* 地面の高さ */
  particle::height(HEIGHT);
}

/*
** シーンの描画
*/
static void scene(void)
{
  /* パーティクルを生成する */
  if (particles.size() >= MAX_PARTICLES) {
    particles.pop_back();
  }
  particle p;
  particles.push_front(p);

  /* テクスチャマッピング開始 */
  glEnable(GL_TEXTURE_2D);

  /* Point Sprite を有効にする */
  glEnable(GL_POINT_SPRITE);

  /* アルファテストを有効にする */
  glEnable(GL_ALPHA_TEST);

  /* パーティクルを描く */
  glColor3d(1.0, 1.0, 1.0);
  glPointSize(psize);
  glBegin(GL_POINTS);
  for (std::deque<particle>::iterator ip = particles.begin();
       ip != particles.end(); ++ip) {
    glVertex3dv(ip->getPosition());
    ip->update();
  }
  glEnd();

  /* アルファテストを無効にする */
  glDisable(GL_ALPHA_TEST);

  /* Point Sprite を無効にする */
  glDisable(GL_POINT_SPRITE);

  /* テクスチャマッピング終了 */
  glDisable(GL_TEXTURE_2D);
}


/****************************
** GLUT のコールバック関数 **
****************************/

#include "trackball.h" /* トラックボール処理用関数の宣言 */

static void display(void)
{
  /* モデルビュー変換行列の初期化 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* 光源の位置を設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  
  /* 視点の移動（物体の方を奥に移動）*/
  glTranslated(0.0, 0.0, -3.0);
  
  /* トラックボール処理による回転 */
  glMultMatrixd(trackballRotation());
  
  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* シーンの描画 */
  scene();
  
  /* ダブルバッファリング */
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  /* 点の大きさを設定する */
  psize = w * 0.1;

  /* トラックボールする範囲 */
  trackballRegion(w, h);
  
  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);
  
  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);
  
  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(60.0, (double)w / (double)h, 1.0, 100.0);
}

static void idle(void)
{
  /* 画面の描き替え */
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state) {
    case GLUT_DOWN:
      /* トラックボール開始 */
      trackballStart(x, y);
      break;
    case GLUT_UP:
      /* トラックボール停止 */
      trackballStop(x, y);
      break;
    default:
      break;
    }
    break;
    default:
      break;
  }
}

static void motion(int x, int y)
{
  /* トラックボール移動 */
  trackballMotion(x, y);
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    /* ESC か q か Q をタイプしたら終了 */
    exit(0);
  default:
    break;
  }
}

/*
** メインプログラム
*/
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
  return 0;
}
