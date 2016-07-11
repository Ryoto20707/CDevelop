/*
 * map.cのbelt[]のインデックス
 */
#define X_FROM    0 // x座標（から）
#define X_TO      1 // x座標（まで）
#define Y_FROM    2 // y座標 (から)
#define Y_TO      3 // y座標（まで）
#define SPEED     4 // 速さ（DIRECTION軸に対して正の向き）
#define DIRECTION 5 // 方向

/*
 * DIRECTIONの取りうる値
 */
#define X_DIR 1.0 // x軸方向
#define Y_DIR 2.0 // y軸方向


//ベルトコンベア描写関数のプロトタイプ宣言
void drawBelt(void);