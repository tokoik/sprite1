# sprite1 - Point Sprite を使ってみる サンプルプログラム

## 1. 概要

このプログラムは、OpenGL における「Point Sprite」機能を用いて点にテクスチャをマッピングし、球のパーティクルを描画する学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って作成したものです。

- [Point Sprite を使ってみる](https://tokoik.github.io/blog/2006-02-27.html)

このプログラムでは、`GL_POINT_SPRITE` を有効化して点（正方形）に球のテクスチャ (`ball.raw`) をマッピングし、アルファテストによる輪郭の抜き処理、および `glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, ...)` を用いた距離に応じた点の大きさの減衰計算（遠近感の付与）を実装しています。

## 2. ビルド方法

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各OSとも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

### 2.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の sprite1.sln を Visual Studio で開きます。
4. ソリューションエクスプローラーで **sprite1** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 2.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された build/sprite1.xcodeproj を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **sprite1** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 2.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 3. 使い方

### 3.1 プログラムの起動方法

- **Windows**: `build\Debug\sprite1.exe`
- **macOS**: `open build/Debug/sprite1.app` または Xcode 上で Run
- **Ubuntu Linux**: `cd build && ./sprite1`

### 3.2 操作方法

- **マウスの左ボタンでドラッグ**: 視点を 3 次元的に回転
- **キーボードの q, Q または ESC キー**: プログラムを終了

## 4. 解説

このプログラムは、[sprite0](https://github.com/tokoik/sprite0) の点によるパーティクル描画に、テクスチャマッピングと Point Sprite を追加したものです。

### 4.1 テクスチャの読み込みとテクスチャ環境 (`init()` 関数)

64 × 64 画素の RGBA の生画像 `ball.raw` を `std::ifstream` で読み込み、テクスチャに割り当てます。下地の色の影響を受けないよう、テクスチャ環境のモードには `GL_REPLACE` を指定します。また、`GL_GENERATE_MIPMAP` を `GL_TRUE` にしてミップマップを自動生成し、縮小フィルタに `GL_LINEAR_MIPMAP_LINEAR` を指定しています。

```cpp
/* テクスチャ環境 */
glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
```

`glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE)` により、点を描画する際に各フラグメントのテクスチャ座標を 0.0〜1.0 の範囲で自動生成させ、点全体にテクスチャ画像がマッピングされるようにします。

### 4.2 アルファテストによる輪郭の抜き処理

テクスチャ画像には球の形に切り抜くためのアルファ値が設定されているので、その透明部分を描画しないようにアルファテストを使います。判別関数は `init()` で設定し、描画時に有効にします。

```cpp
/* アルファテストの判別関数 */
glAlphaFunc(GL_GREATER, 0.5);
```

### 4.3 描画時の設定 (`scene()` 関数)

パーティクルを描く前にテクスチャマッピング・Point Sprite・アルファテストを有効にし、描き終わったら無効に戻します。点の大きさは `glPointSize()` で指定します。

```cpp
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
...
```

### 4.4 点の大きさの制御

- **ウィンドウの大きさに合わせる**: `resize()` で、点の大きさを保持する変数 `psize` をウィンドウの幅に比例させます（`psize = w * 0.1f`）。
- **視点からの距離に応じて変える**: `glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance)` により、実際に描かれる点の大きさが

  $$pointSize=\mathrm{clampSize}\left(size * \sqrt{\frac{1}{a + b * d + c * d^2}}\right)$$

  となるようにします。配列 `distance` の要素が $a$, $b$, $c$ に相当するので、`{ 0.0f, 0.0f, 1.0f }` を指定すれば、点の大きさが視点からの距離 $d$ に反比例するようになります。
- なお `glPointParameterfv()` は OpenGL 1.4 以降の機能なので、Windows では `wglGetProcAddress()` で関数のエントリポイントを取得しています。

  ```cpp
  #if defined(WIN32)
    glPointParameterfv =
      (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfv");
  #endif
  ```
