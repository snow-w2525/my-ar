# my-ar
このプログラムは、カメラから取り込んだ映像に、画像を重ねることを主目的として作成しました。
ARUCOマーカーを検出して、指定したリソースを表示させることができます。

## MyARの使い方
	- 設定はすべてcamera-config.jsonで行います。
	　このファイルの読み込みは、起動時だけなので、設定を変更して反映したい場合は、一度アプリを落とす必要があります。

## 開発環境
- Windows11
- Visual Studio 2022

### JSONファイルの設定

#### 共通設定
| パラメータ名 | 型の指定 | 説明 | 
|---|---|---|
| BACKGROUND_TYPE | string | windowの背景を何にするか。cameraかimageかを選択する| 
| CAMERA_TITLE | string | windowのタイトル。| 
| CAMERA_NO | int | BACKGROUND_TYPEがcameraのときに、どのカメラポートのカメラを使用するかを番号で選択する。|
| BACKGROUND_IMAGE_NAME | string | BACKGROUND_TYPEがimageのときに、どの画像を背景に挿入するかを設定する。|
| BACKGROUND_WIDTH | int | windowの幅を設定する。|
| BACKGROUND_HEIGHT | int | windowの高さを設定する。|

#### ARリソース設定
ARリソースのパラメータ。リスト形式になっているため、複数登録することが可能。

| パラメータ名 | 型の指定 | 説明 | 
|---|---|---|
| TYPE | string | ARリソースの種類。image、windowから選択。|
| NAME | string | ARリソースのファイル名、またはウィンドウ名。ウィンドウ名の場合は、このウィンドウ名に基づいてウィンドウハンドルを探す。|
| POSITION_X | float |ARリソースの表示位置のX座標を1以下の少数で指定。(画面の横幅いっぱいを1とする) |
| POSITION_Y | float | ARリソースの表示位置のY座標を1以下の少数で指定。(画面の縦幅いっぱいを1とする) |
| WIDTH | float | ARリソースの横サイズを1以下の少数で指定。(画面の横幅いっぱいを1とする) |
| HEIGHT | float | ARリソースの縦サイズを1以下の少数で指定。(画面の縦幅いっぱいを1とする) |
| MIN_R, MAX_R | float | (COMBINEがchromakeyまたはperspective + chromakeyのとき)クロマキー処理する色のR成分の幅を指定。|
| MIN_G, MAX_G | float | (COMBINEがchromakeyまたはperspective + chromakeyのとき)クロマキー処理する色のG成分の幅を指定。|
| MIN_B, MAX_B | float | (COMBINEがchromakeyまたはperspective + chromakeyのとき)クロマキー処理する色のB成分の幅を指定。|
| MORH | string | erode、dilateから選択。クロマキー処理後に残った画像の領域を膨張させるか、収縮させるかを選択する。|
| KERNEL_SIZE | int | MORHで設定した処理のカーネルサイズを指定。|
| TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT | [float, float] | (COMBINEがperspectiveまたはperspective + chromakeyのとき)透視変換後の配置座標を指定。|
| COMBINE | string |  windowに、ARリソースをどう配置するか。chromakey、perspective、perspective + chromakeyから選択する。|
| ARUCO | bool | BACKGROUND_TYPEがcameraのとき、ARUCOマーカー上にレンダリングするかを選択する。ARUCOマーカー上にレンダリングするので、COMBINEをperspective + chromakeyにすること。|
| ARUCO_VERTICAL | bool | ARUCOがtrueのとき、リソースをマーカー上に対して垂直に配置するか、水平に配置するかを選択する。|
| ARUCO_OFFSET | float | ARUCOマーカー上にリソースを表示する際の、ARリソースの拡大縮小倍率を設定。|
| ARUCO_OFFSET | float | ARUCOマーカー上にリソースを表示する際の、ARリソースの拡大縮小倍率を設定。(ARUCOマーカーの一辺を1とする) |
| ARUCO_SHIFT_X, ARUCO_SHIFT_Y, ARUCO_SHIFT_Z | float | ARUCOマーカーからどのくらいシフトした位置にARリソースを表示させるか。|

