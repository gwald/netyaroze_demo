/* $PSLibId: Run-time Library Release 4.0$ */
/*
 *	TOD functions (part2)
 :	TODアニメーション処理関数群（その２）
 *
 *
 *		Version 1.30	Apr, 17, 1996
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */


#include <libps.h>

#include "tod.h"

extern GsF_LIGHT pslt[3];		/* Light source: 光源×３個 */

/*
 *  Prototype definitions: プロトタイプ宣言
 */
u_long *TodSetFrame();
u_long *TodSetPacket();

/*
 *  TOD process for 1 frame
 :  １フレーム分のTODデータの処理
 */
u_long *TodSetFrame(currentFrame, todP, objTblP, tmdIdP, tmdTblP, mode)
int currentFrame;	/* Current time:現在の時刻（フレーム番号） */
u_long *todP;		/* Current TOD:再生中のTODデータへのポインタ */
TodOBJTABLE *objTblP;	/* Object table:オブジェクトテーブルへのポインタ */
int *tmdIdP;		/* TMD ID list:モデリングデータIDリスト */
u_long *tmdTblP;	/* TMD data addr:TMDデータへのポインタ */
int mode;		/* Playing mode:再生モード */
{
	u_long hdr;
	u_long nPacket;
	u_long frame;
	int i;

	/* Get frame info.:フレーム情報を得る */
	hdr = *todP;			/* :フレームヘッダを得る */
	nPacket = (hdr>>16)&0x0ffff;	/* Number of packet:パケット個数 */
	frame = *(todP+1);		/* :時刻（フレーム番号） */

	/* Wait until specified frame time
	 : 現在時刻のフレームでなければ何も処理しない
	 */
	if(frame > currentFrame) return todP;

	/* Execute each packet in a frame
	 : フレームの中の各パケットを処理する */
	todP += 2;
	for(i = 0; i < nPacket; i++) {
		todP = TodSetPacket(todP, objTblP, tmdIdP, tmdTblP, mode);
	}

	/* Return next frame addr.
	 : 次のフレームへのポインタを返す */
	return todP;	
}

/*
 *  Execute single packet
 :  １パケットの処理
 */
u_long *TodSetPacket(packetP, tblP, tmdIdP, tmdTblP, mode)
u_long *packetP;
TodOBJTABLE *tblP;
int *tmdIdP;
u_long *tmdTblP;
int mode;
{
	u_long *dP;		/* Current TOD addr:処理中のTODへのポインタ */

	u_long hdr;
	u_long id;
	u_long flag;
	u_long type;
	u_long len;

	/* Current object: 処理中オブジェクト */
	GsDOBJ2 *objP;		/* オブジェクトへのポインタ */
	GsCOORD2PARAM *cparam;	/* Coordinate value(RST): 座標パラメータ(R,S,T) */
	MATRIX *coordp;		/* Coordinate matrix: 座標マトリクス */
	GsDOBJ2 *parentP;	/* Parent object: 親オブジェクト */
	VECTOR v;
	SVECTOR sv;

	/* Dummy object: ダミーオブジェクト変数 */
	GsDOBJ2 dummyObj;
	MATRIX dummyObjCoord;
	GsCOORD2PARAM dummyObjParam;


	/* Get Packet Info: パケット情報を得る */
	dP = packetP;
	hdr = *dP++;
	id = hdr&0x0ffff;	/* ID number */
	type = (hdr>>16)&0x0f;	/* Packet type (TOD_???) */
	flag = (hdr>>20)&0x0f;	/* Flags */
	len = (hdr>>24)&0x0ff;	/* Word lengthfor packet: パケットのワード長 */

	/* Search specified object from ID
	 : ID番号で該当するオブジェクトをサーチ */
	objP = TodSearchObjByID(tblP, id);
	if(objP == NULL) {
		/* Use dummy object when not found
		 : ない場合はダミーオブジェクトを使う */
		objP = &dummyObj;
		coordp = &dummyObjCoord;
		cparam = &dummyObjParam;
	}
	else {
		coordp = &(objP->coord2->coord);
		cparam = (objP->coord2->param);
		objP->coord2->flg = 0;
	}

	/* パケットのタイプ別に処理 */
	switch(type) {
	    case TOD_ATTR:/* Update attribute: アトリビュート変更 */
		objP->attribute = (objP->attribute&*dP)|*(dP+1);	
		dP += 2;
		break;

	    case TOD_COORD:/* Update Coordinate value
			    : 座標変更（移動／回転／拡大） */

		/* Update 'coordinate' member of current object
		   using 'cparam' member.
		 : 該当オブジェクトのcoordinateの内容を更新する
		   更新するパラメータはcparamメンバに保管してある */

		if(flag&0x01) {

			/* Differencial value: 前フレームからの差分の場合 */

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx += (*(((long *)dP)+0))/360;
				cparam->rotate.vy += (*(((long *)dP)+1))/360;
				cparam->rotate.vz += (*(((long *)dP)+2))/360;
				dP += 3;
			}
			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx
				 = (cparam->scale.vx**(((short *)dP)+0))/4096;
				cparam->scale.vy
				 = (cparam->scale.vy**(((short *)dP)+1))/4096;
				cparam->scale.vz
				 = (cparam->scale.vz**(((short *)dP)+2))/4096;
				dP += 2;
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx += *(((long *)dP)+0);
				cparam->trans.vy += *(((long *)dP)+1);
				cparam->trans.vz += *(((long *)dP)+2);
				dP += 3;
			}

			RotMatrix(&(cparam->rotate), coordp);
			ScaleMatrix(coordp, &(cparam->scale));
			TransMatrix(coordp, &(cparam->trans));
		}
		else {
			/* Immediate value: 絶対値の場合 */

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx = (*(((long *)dP)+0))/360;
				cparam->rotate.vy = (*(((long *)dP)+1))/360;
				cparam->rotate.vz = (*(((long *)dP)+2))/360;
				dP += 3;
				RotMatrix(&(cparam->rotate), coordp);
			}

			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx = *(((short *)dP)+0);
				cparam->scale.vy = *(((short *)dP)+1);
				cparam->scale.vz = *(((short *)dP)+2);
				dP += 2;
				if(!(flag&0x02))
					RotMatrix(&(cparam->rotate), coordp);
				ScaleMatrix(coordp, &(cparam->scale));
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx = *(((long *)dP)+0);
				cparam->trans.vy = *(((long *)dP)+1);
				cparam->trans.vz = *(((long *)dP)+2);
				dP += 3;
				TransMatrix(coordp, &(cparam->trans));
			}
		}
		break;

	    case TOD_TMDID:	/* Link to TMD: TMDとのリンクを設定 */
		if(tmdTblP != NULL) {
			GsLinkObject4((u_long)TodSearchTMDByID(tmdTblP, tmdIdP,
					(unsigned long)(*dP&0xffff)), objP, 0);

		}
		break;

	    case TOD_PARENT:	/* Set parent: 親オブジェクトの設定 */
		if(mode != TOD_COORDONLY) {
			if((*dP == NULL)||(*dP == 0xffff)) {
				objP->coord2->super = NULL;
				dP++;
			}
			else {
				parentP = TodSearchObjByID(tblP, *dP++);
				objP->coord2->super = parentP->coord2;
			}
		}
		break;

	    case TOD_OBJCTL:
		/* in the objtable: テーブル内オブジェクトの管理 */
		if(tblP != NULL) {
			switch(flag) {
			    case 0:
				/* Create new object: 新規オブジェクトの生成 */
				TodCreateNewObj(tblP, id);
				break;
			    case 1:
				/* Delete object: オブジェクトの削除 */
				TodRemoveObj(tblP, id);
				break;
			}
		}
		break;

	    case TOD_LIGHT:
		/* Light source control: 光源の設定 */
		if(flag&0x02) {
			if(flag&0x01) {
				pslt[id].vx += *(((long *)dP)+0);
				pslt[id].vy += *(((long *)dP)+1);
				pslt[id].vz += *(((long *)dP)+2);
			}
			else {
				pslt[id].vx = *(((long *)dP)+0);
				pslt[id].vy = *(((long *)dP)+1);
				pslt[id].vz = *(((long *)dP)+2);
			}
			dP += 3;
		}
		if(flag&0x04) {
			if(flag&0x01) {
				pslt[id].r += *(((u_char *)dP)+0);
				pslt[id].g += *(((u_char *)dP)+1);
				pslt[id].b += *(((u_char *)dP)+2);
			}
			else {
				pslt[id].r = *(((u_char *)dP)+0);
				pslt[id].g = *(((u_char *)dP)+1);
				pslt[id].b = *(((u_char *)dP)+2);
			}
			       dP++;
		}
		GsSetFlatLight(id, &pslt[id]);
		break;

	    case TOD_USER0:
		/* Describe user process here
		 : ここにユーザ定義パケットの処理を記述する  */
		break;
	}
	/* Return next packet addr: 次のパケットへのポインタを返す */
	return packetP+len;
}

