# Pico2 向けのFMP3ベンチマークプログラム

## コンフィギュレーション
- シングルプロセッサ向けのベンチマークに関してはFMP3のコンフィギュレーションをマルチコアとシングルコアから選択可能
- トップの ConfigSingle.cmake から選択可能
- 1行目を有効にするとシングルコア，2行目を有効にするとマルチコアのコンフィギュレーションでFMP3をビルドする．
  
```
set(TNUM_PRCID 1)
# set(TNUM_PRCID 2)
```

## 計測方法
- 2種類の計測方法をサポートしている
  - 内蔵タイマーによる計測
    - 1us精度による計測
  - GPIOによる計測
    - GP2/GP3/GP4による計測
    - 使用するポートは./fmp3/target/rp2350-arm-s_pico_sdk/taget_test.h のマクロで指定する

```
#define M_GPIO_NO1  2
#define M_GPIO_NO2  3
#define M_GPIO_NO3  4
```

- 切り替え方法
  - ./perf/perf.h でマクロの定義で切り替える
  - 内蔵タイマーによる計測
    - USE_HISTOGRAM を定義する
  - GPIOによる計測
    - USE_HISTOGRAM を未定義にする

## 使用方法
- Pico拡張をインストールしたVSCodeで各フォルダを開いてビルド