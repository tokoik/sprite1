# sprite1 - Point Sprite を使ってみる サンプルプログラム

## 1. 概要

このプログラムは、OpenGL における「Point Sprite」機能を用いて点にテクスチャをマッピングし、球のパーティクルを描画する学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って学習を進めるための雛形として提供されています。

- [Point Sprite を使ってみる](https://tokoik.github.io/blog/opengl/2006/02/27/texture.html)

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

### 4.1 Point Sprite の有効化とテクスチャ環境

```cpp
glEnable(GL_POINT_SPRITE);
glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
```

点を描画する際に各フラグメントのテクスチャ座標を自動生成（0.0〜1.0）させ、点全体にテクスチャ画像をマッピングします。

### 4.2 アルファテストによる輪郭の抜き処理

テクスチャ画像の背景（アルファ値 0）を描画しないように、アルファテストを有効化します。

```cpp
glEnable(GL_ALPHA_TEST);
glAlphaFunc(GL_GREATER, 0.5);
```

### 4.3 距離減衰による点の大きさの制御

`glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance)` により、視点からの距離 $d$ に反比例して点の描画サイズをスケーリングします。
