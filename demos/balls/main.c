/*
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 */


       
#include <libps.h>

#include "pad.h"

#include "vid_mode.h"


#define KANJI				
#define OT_LENGTH	1	   
#define MAXOBJ		800		
#define MVOL		127	   
#define SVOL		127	   
#define DFILE		3	  



#define VH_ADDR       0x80090000
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000

	// screen constants depend on video mode used
#if PAL_VIDEO_MODE
	#define SCREEN_WIDTH		320		
	#define SCREEN_HEIGHT		256
	#define SCREEN_X_OFFSET 0
	#define SCREEN_Y_OFFSET 20
	#define WALL_X		(SCREEN_WIDTH-16)	
	#define WALL_Y		(SCREEN_HEIGHT-58)		
#elif NTSC_VIDEO_MODE
	#define SCREEN_WIDTH		320		
	#define SCREEN_HEIGHT		240		
	#define SCREEN_X_OFFSET 0
	#define SCREEN_Y_OFFSET 16
	#define WALL_X		(SCREEN_WIDTH-16)	
	#define WALL_Y		(SCREEN_HEIGHT-40)
#else
	#error "shouldn't be here"     // must specify one mode or the other
#endif



	// walls always 16 in from screen edges






#define limitRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))





GsOT		WorldOT[2];
GsOT_TAG	OTTags[2][1<<OT_LENGTH];

PACKET		GpuPacketArea[2][MAXOBJ*(20+4)];
GsSPRITE	sprt[MAXOBJ];


typedef struct {
	u_short x, y;			
	u_short dx, dy;		 
} POS;


volatile u_char *bb0, *bb1;


typedef struct {
	char	*fname;
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, (0)},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, (0)},
	{"\\DATA\\SOUND\\SAMPLE1.SEQ;1", (void *)SEQ_ADDR, (0)},
};


short vab, seq;


static void init_prim();		
static void init_point(POS *pos);  
static long pad_read(long n);	   
static u_long PadRead(long id);	 
static void datafile_search();	   
static void datafile_read();	  
static void init_sound();	  
static void play_sound();	   
static void stop_sound();	   



extern DISPENV GsDISPENV;

    // NOTE: cannot use __attribute__((section("cachedata"))) specifier
    // with the blackpsx
int nobj = 1;   



int main(void)
{
	int	i, cnt, x, y;	/* 作業変数 */
	int	activeBuff;
	GsSPRITE	*sp;
	POS	pos[MAXOBJ];
	POS	*pp;

	GetPadBuf(&bb0, &bb1);	/* コントローラ受信バッファの取得 */
	datafile_search();	/* データファイルの検索 */
	datafile_read();	/* データファイルの読み込み */

    GsInitGraph(320, 256, 4, 0, 0); /* 描画・表示環境の設定 */
	GsDefDispBuff(0, 0, 0, 256);	/* 同上 */


	// setting the video mode for PAL
#if PAL_VIDEO_MODE
		GsDISPENV.pad0 = 1;
#endif

		// Here: setting screen offset
	GsDISPENV.screen.x = SCREEN_X_OFFSET; 
	GsDISPENV.screen.y = SCREEN_Y_OFFSET;
     
		// setting the screen width and height
	// NEED? GsDISPENV.screen.w = SCREEN_WIDTH;
	// NEED? GsDISPENV.screen.h = SCREEN_HEIGHT;
	





	for (i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}

	/* フォントの設定 */
#ifdef KANJI    /* 漢字表示の場合 */
	KanjiFntOpen(160, 16, 256, 240, 704, 0, 768, 256, 0, 512);
#endif
	FntLoad(960, 256);	/* 基本フォントパターンをフレームバッファにロード */
	FntOpen(16, 16, 256, 200, 0, 512);	/* フォントの表示位置の設定 */

	init_prim();		/* プリミティブバッファの初期設定 */
	init_point(pos);	/* ボールパタンの動きの初期設定 */
	init_sound();		/* サウンドの初期設定 */
	play_sound();		/* サウンド再生の開始 */

	/* メインループ */
	while ((nobj = pad_read(nobj)) > 0) {
		/* ダブルバッファの切り替え */
		activeBuff = GsGetActiveBuff();
		GsSetWorkBase((PACKET *)GpuPacketArea[activeBuff]);

		/* オーダリングテーブルのクリア */
		GsClearOt(0, 0, &WorldOT[activeBuff]);

		/* ボールパタン位置の更新とＯＴへの登録 */
		sp = sprt;	pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* 横方向座標値の更新 */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			/* 縦方向座標値の更新 */
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* スプライトプリミティブに新しい座標値をセット */
			sp->x = x;	sp->y = y;
			/* スプライトプリミティブのオーダリングテーブルへの登録 */
			GsSortFastSprite(sp, &WorldOT[activeBuff], 0);
		}

		DrawSync(0);			/* 描画の終了待ち */
		cnt = VSync(0);			/* 垂直同期割り込み待ち */
		GsSwapDispBuff();		/* ダブルバッファの切替え */

		/* 画面クリアプリミティブのオーダリングテーブルへの登録 */
		GsSortClear(60, 120, 120, &WorldOT[activeBuff]);

		/* ＯＴに登録されたプリミティブの描画 */
		GsDrawOt(&WorldOT[activeBuff]);

		/* ボールの数と経過時間のプリント */
#ifdef KANJI
		KanjiFntPrint("玉の数＝%d\n", nobj);
		KanjiFntPrint("時間=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
             
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n\n\n", cnt);
		FntPrint("UP    : INCREASE\n");
		FntPrint("DOWN  : DECREASE\n");
		FntPrint("L1    : PAUSE\n");
		FntPrint("SELECT: END\n");
		FntFlush(-1);
	}	/* メインループの終端 */

	/* selectボタン押し下げ検出でここを実行 */
	stop_sound();		/* サウンド再生の終了 */
	return(0);		/* プログラムの終了 */
}


/*----------------------------------------------------------------------
 ボールパタンの初期化
----------------------------------------------------------------------*/
#include "balltex.h"		/* ボールパタンのテクスチャパターン */

/* ボールパタンのグラフィック関連初期化 */
static void init_prim()
{
	GsSPRITE *sp;
	u_short	tpage;
	RECT	rect;
	int	i;
	
	rect.x = 640;	rect.y = 0;
	rect.w = 16/4;	rect.h = 16;
	LoadImage(&rect, ball16x16);
	tpage = GetTPage(0, 0, 640, 0);

	for (i = 0; i < 32; i++) {
		rect.x = 320;	rect.y = 0+i;
		rect.w = 256;	rect.h = 1;
		LoadImage(&rect, ballcolor[i]);
	}

	/* スプライトの初期化 */
	for (sp = sprt, i = 0; i < MAXOBJ; i++, sp++) {
		sp->attribute = 0;
		sp->x = 0;
		sp->y = 0;
		sp->w = 16;
		sp->h = 16;
		sp->tpage = tpage;
		sp->u = 0;
		sp->v = 0;
		sp->cx = 320;
		sp->cy = 0+(i%32);
		sp->r = sp->g = sp->b = 0x80;
		sp->mx = 0;
		sp->my = 0;
		sp->scalex = ONE;
		sp->scaley = ONE;
		sp->rotate = 0;
	}
}

/* ボールパタンの動き関連初期化 */
static void init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* スタート座標 Ｘ */
		pos->y  = rand();		/* スタート座標 Ｙ */
		pos->dx = (rand() % 4) + 1;	/* 移動距離 Ｘ (1<=x<=4) */
		pos->dy = (rand() % 4) + 1;	/* 移動距離 Ｙ (1<=y<=4) */
		pos++;
	}
}

/*----------------------------------------------------------------------
 コントローラ状態の読み込みと解析
----------------------------------------------------------------------*/
/* コントローラ状態の解析 */
/*	返り値：
		-1：selectボタン押し下げ検出時
		第１引数＋４：上方向ボタン押し下げ検出時
		第１引数－４：左方向ボタン押し下げ検出時
	L1キー押し下げ中は関数内部でポーズする。
*/
static long pad_read(long n)
{
	u_long padd = PadRead(1);		/* コントローラの読み込み */

	if (padd & PADLup)	n += 4;		/* 左の十字キーアップ */
	if (padd & PADLdown)	n -= 4;		/* 左の十字キーダウン */

	if (padd & PADL1)
		while (PadRead(1)&PADL1);	/* ポーズ */

	if(padd & PADselect)
		return(-1);			/* プログラムの終了 */

	limitRange(n, 1, MAXOBJ-1);		/* nを1<=n<=(MAXOBJ-1)の値にする */
	return(n);
}

/* コントローラ状態の読み込み */
static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

/*----------------------------------------------------------------------
 CD-ROM上のファイル（DFILE個）の読み込み
----------------------------------------------------------------------*/
/* CD-ROM上のファイルの検索 */
static void datafile_search()
{
	int i, j;

	for (i = 0; i < DFILE; i++) {		/* DFILE個のファイルを扱う */
		for (j = 0; j < 10; j++) {	/* リトライループ */
			if (CdSearchFile(&(dfile[i].finfo), dfile[i].fname) != 0)
				break;		/* 正常終了ならばリトライループ中断 */
			else
				printf("%s not found.\n", dfile[i].fname);
		}
	}
}

/* CD-ROM上のファイルの読み込み */
static void datafile_read()
{
	int i, j;
	int cnt;

	for (i = 0; i < DFILE; i++) {		/* DFILE個のファイルを扱う */
		for (j = 0; j < 10; j++) {	/* リトライループ */
			CdReadFile(dfile[i].fname, dfile[i].addr,dfile[i].finfo.size);

			/* リードの裏で通常の処理は実行できる。*/
			/* ここでは、Read が終了するまで残りのセクタ数を監視 */
			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	/* 垂直同期割り込み待ち（時間調整用） */

			if (cnt == 0)
				break;	/* 正常終了ならばリトライループ中断 */
		}
	}
}


/*----------------------------------------------------------------------
 サウンド関連
----------------------------------------------------------------------*/
/* メモリ上のサウンドデータの再生準備 */
static void init_sound()
{
	/* VAB のオープンおよびサウンドバッファへの転送 */
	vab = SsVabTransfer( (u_char*)VH_ADDR, (u_char*)VB_ADDR, -1, 1 );
	if (vab < 0) {
		printf("SsVabTransfer failed (%d)\n", vab);
		return;
	}

	/* SEQ のオープン */
	seq = SsSeqOpen((u_long *)SEQ_ADDR, vab);
	if (seq < 0)
		printf("SsSeqOpen failed (%d)\n", seq);
}

/* サウンド再生の開始 */
static void play_sound()
{
	SsSetMVol(MVOL, MVOL);			/* メインボリュームの設定 */
	SsSeqSetVol(seq, SVOL, SVOL);		/* SEQ毎のボリュームの設定 */
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);	/* 再生スイッチＯＮ */
}

/* サウンド再生の終了 */
static void stop_sound()
{
	SsSeqStop(seq);				/* 再生スイッチＯＦＦ */
	VSync(0);
	VSync(0);
	SsSeqClose(seq);			/* SEQのクロ－ズ */
	SsVabClose(vab);			/* VABのクローズ */
}

/* ソースコードの終わり */
