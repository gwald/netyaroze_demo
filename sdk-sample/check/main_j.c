/*
 *	��ʓ����o�E���h����{�[���p�^����`�悷��B
 *	�㉺�����L�[�ɂ��{�[���p�^�����𑝌�����B
 *	�N���f�B�X�N����T�E���h�f�[�^��ǂݍ���BGM�Ƃ��čĐ�����B
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
*/

/* �W���C���N���[�h�w�b�_ */
#include <libps.h>
#include "pad.h"

/*----------------------------------------------------------------------
	�}�N��
----------------------------------------------------------------------*/
/* ��{�ݒ� */
#define KANJI				/* ������\������X�C�b�` */
#define OT_LENGTH	1		/* �I�[�_�����O�e�[�u���̐� */
#define MAXOBJ		1500		/* �X�v���C�g�i�{�[���j���̏�� */
#define MVOL		127		/* ���C���{�����[�����x�� */
#define SVOL		127		/* SEQ�{�����[�����x�� */
#define DFILE		3		/* �t�@�C���� */

/* �f�[�^�z�u�֘A�}�N�� */
#define VH_ADDR		0x80090000
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000

/* �\���̈�֘A�}�N�� */
#define FRAME_X		320		/* �\���̈�T�C�Y(���j */
#define FRAME_Y		240		/* �\���̈�T�C�Y(�c�j */
#define WALL_X		(FRAME_X-16)	/* �{�[���p�^�����̈�T�C�Y�i���j */
#define WALL_Y		(FRAME_Y-16)	/* �{�[���p�^�����̈�T�C�Y�i�c�j */

/* �͈̓`�F�b�N�}�N�� */
#define limitRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))


/*----------------------------------------------------------------------
	�O���ϐ�
----------------------------------------------------------------------*/
/* �I�[�_�����O�e�[�u���֘A�ϐ� */
GsOT		WorldOT[2];
GsOT_TAG	OTTags[2][1<<OT_LENGTH];

/* �v���~�e�B�u�֘A�ϐ� */
PACKET		GpuPacketArea[2][MAXOBJ*(20+4)];
GsSPRITE	sprt[MAXOBJ];

/* �{�[���p�^���̓����֘A�ϐ� */
typedef struct {
	u_short x, y;			/* �ʒu */
	u_short dx, dy;			/* ���x */
} POS;

/* �R���g���[���֘A�ϐ� */
volatile u_char *bb0, *bb1;

/* �t�@�C���ǂݍ��݊֘A�ϐ� */
typedef struct {
	char	*fname;
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\SAMPLE1.SEQ;1", (void *)SEQ_ADDR, 0},
};

/* �T�E���h�֘A�ϐ� */
short vab, seq;

/*----------------------------------------------------------------------
 �ÓI�֐��e���v���[�g
----------------------------------------------------------------------*/
static void init_prim();		/* �{�[���p�^���̃O���t�B�b�N�֘A������ */
static void init_point(POS *pos);	/* �{�[���p�^���̓����֘A������ */
static long pad_read(long n);		/* �R���g���[����Ԃ̉�� */
static u_long PadRead(long id);		/* �R���g���[����Ԃ̓ǂݎ�� */
static void datafile_search();		/* CD-ROM��̃t�@�C���̌��� */
static void datafile_read();		/* CD-ROM��̃t�@�C���̓ǂݍ��� */
static void init_sound();		/* ��������̃T�E���h�f�[�^�̍Đ����� */
static void play_sound();		/* �T�E���h�Đ��̊J�n */
static void stop_sound();		/* �T�E���h�Đ��̏I�� */


/*----------------------------------------------------------------------
 ���C���֐�
----------------------------------------------------------------------*/
main()
{
	int	nobj = 1;	/* �\������{�[���p�^���̐��i�ŏ��͂P����j*/
	GsOT	*ot;		/* �`��Ώۂn�s�ւ̃|�C���^ */
	int	i, cnt, x, y;	/* ��ƕϐ� */
	int	activeBuff;
	GsSPRITE	*sp;
	POS	pos[MAXOBJ];
	POS	*pp;

        SetVideoMode(MODE_NTSC);        /* NTSC Mode */
/*      SetVideoMode(MODE_PAL);         /* PAL  Mode */

	GetPadBuf(&bb0, &bb1);	/* �R���g���[����M�o�b�t�@�̎擾 */
	datafile_search();	/* �f�[�^�t�@�C���̌��� */
	datafile_read();	/* �f�[�^�t�@�C���̓ǂݍ��� */

	GsInitGraph(320, 240, 4, 0, 0);	/* �`��E�\�����̐ݒ� */
	GsDefDispBuff(0, 0, 0, 240);	/* ���� */
	/* 
	  (0,0)-(320,240)�ɕ`�悵�Ă���Ƃ���(0,240)-(320,480)��\��(db[0])
	  (0,240)-(320,480)�ɕ`�悵�Ă���Ƃ���(0,  0)-(320,240)��\��(db[1])
	*/

	/* �I�[�_�����O�e�[�u�����̐ݒ� */
	for (i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}

	/* �t�H���g�̐ݒ� */
#ifdef KANJI    /* �����\���̏ꍇ */
	KanjiFntOpen(160, 16, 256, 240, 704, 0, 768, 256, 0, 512);
#endif
	FntLoad(960, 256);	/* ��{�t�H���g�p�^�[�����t���[���o�b�t�@�Ƀ��[�h */
	FntOpen(16, 16, 256, 200, 0, 512);	/* �t�H���g�̕\���ʒu�̐ݒ� */

	init_prim();		/* �v���~�e�B�u�o�b�t�@�̏����ݒ� */
	init_point(pos);	/* �{�[���p�^���̓����̏����ݒ� */
	init_sound();		/* �T�E���h�̏����ݒ� */
	play_sound();		/* �T�E���h�Đ��̊J�n */

	/* ���C�����[�v */
	while ((nobj = pad_read(nobj)) > 0) {
		/* �_�u���o�b�t�@�̐؂�ւ� */
		activeBuff = GsGetActiveBuff();
		GsSetWorkBase((PACKET *)GpuPacketArea[activeBuff]);

		/* �I�[�_�����O�e�[�u���̃N���A */
		GsClearOt(0, 0, &WorldOT[activeBuff]);

		/* �{�[���p�^���ʒu�̍X�V�Ƃn�s�ւ̓o�^ */
		sp = sprt;	pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* ���������W�l�̍X�V */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			/* �c�������W�l�̍X�V */
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* �X�v���C�g�v���~�e�B�u�ɐV�������W�l���Z�b�g */
			sp->x = x;	sp->y = y;
			/* �X�v���C�g�v���~�e�B�u�̃I�[�_�����O�e�[�u���ւ̓o�^ */
			GsSortFastSprite(sp, &WorldOT[activeBuff], 0);
		}

		DrawSync(0);			/* �`��̏I���҂� */
		cnt = VSync(0);			/* �����������荞�ݑ҂� */
		GsSwapDispBuff();		/* �_�u���o�b�t�@�̐ؑւ� */

		/* ��ʃN���A�v���~�e�B�u�̃I�[�_�����O�e�[�u���ւ̓o�^ */
		GsSortClear(60, 120, 120, &WorldOT[activeBuff]);

		/* �n�s�ɓo�^���ꂽ�v���~�e�B�u�̕`�� */
		GsDrawOt(&WorldOT[activeBuff]);

		/* �{�[���̐��ƌo�ߎ��Ԃ̃v�����g */
#ifdef KANJI
		KanjiFntPrint("�ʂ̐���%d\n", nobj);
		KanjiFntPrint("����=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n\n\n", cnt);
		FntPrint("UP    : INCREASE\n");
		FntPrint("DOWN  : DECREASE\n");
		FntPrint("L1    : PAUSE\n");
		FntPrint("SELECT: END\n");
		FntFlush(-1);
	}	/* ���C�����[�v�̏I�[ */

	/* select�{�^�������������o�ł��������s */
	stop_sound();		/* �T�E���h�Đ��̏I�� */
	KanjiFntClose();
	return(0);		/* �v���O�����̏I�� */
}


/*----------------------------------------------------------------------
 �{�[���p�^���̏�����
----------------------------------------------------------------------*/
#include "balltex.h"		/* �{�[���p�^���̃e�N�X�`���p�^�[�� */

/* �{�[���p�^���̃O���t�B�b�N�֘A������ */
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
		rect.x = 0;	rect.y = 480+i;
		rect.w = 256;	rect.h = 1;
		LoadImage(&rect, ballcolor[i]);
	}

	/* �X�v���C�g�̏����� */
	for (sp = sprt, i = 0; i < MAXOBJ; i++, sp++) {
		sp->attribute = 0;
		sp->x = 0;
		sp->y = 0;
		sp->w = 16;
		sp->h = 16;
		sp->tpage = tpage;
		sp->u = 0;
		sp->v = 0;
		sp->cx = 0;
		sp->cy = 480+(i%32);
		sp->r = sp->g = sp->b = 0x80;
		sp->mx = 0;
		sp->my = 0;
		sp->scalex = ONE;
		sp->scaley = ONE;
		sp->rotate = 0;
	}
}

/* �{�[���p�^���̓����֘A������ */
static void init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* �X�^�[�g���W �w */
		pos->y  = rand();		/* �X�^�[�g���W �x */
		pos->dx = (rand() % 4) + 1;	/* �ړ����� �w (1<=x<=4) */
		pos->dy = (rand() % 4) + 1;	/* �ړ����� �x (1<=y<=4) */
		pos++;
	}
}

/*----------------------------------------------------------------------
 �R���g���[����Ԃ̓ǂݍ��݂Ɖ��
----------------------------------------------------------------------*/
/* �R���g���[����Ԃ̉�� */
/*	�Ԃ�l�F
		-1�Fselect�{�^�������������o��
		��P�����{�S�F������{�^�������������o��
		��P�����|�S�F�������{�^�������������o��
	L1�L�[�����������͊֐������Ń|�[�Y����B
*/
static long pad_read(long n)
{
	u_long padd = PadRead(1);		/* �R���g���[���̓ǂݍ��� */

	if (padd & PADLup)	n += 4;		/* ���̏\���L�[�A�b�v */
	if (padd & PADLdown)	n -= 4;		/* ���̏\���L�[�_�E�� */

	if (padd & PADL1)
		while (PadRead(1)&PADL1);	/* �|�[�Y */

	if(padd & PADselect)
		return(-1);			/* �v���O�����̏I�� */

	limitRange(n, 1, MAXOBJ-1);		/* n��1<=n<=(MAXOBJ-1)�̒l�ɂ��� */
	return(n);
}

/* �R���g���[����Ԃ̓ǂݍ��� */
static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

/*----------------------------------------------------------------------
 CD-ROM��̃t�@�C���iDFILE�j�̓ǂݍ���
----------------------------------------------------------------------*/
/* CD-ROM��̃t�@�C���̌��� */
static void datafile_search()
{
	int i, j;

	for (i = 0; i < DFILE; i++) {		/* DFILE�̃t�@�C�������� */
		for (j = 0; j < 10; j++) {	/* ���g���C���[�v */
			if (CdSearchFile(&(dfile[i].finfo), dfile[i].fname) != 0)
				break;		/* ����I���Ȃ�΃��g���C���[�v���f */
			else
				printf("%s not found.\n", dfile[i].fname);
		}
	}
}

/* CD-ROM��̃t�@�C���̓ǂݍ��� */
static void datafile_read()
{
	int i, j;
	int cnt;

	for (i = 0; i < DFILE; i++) {		/* DFILE�̃t�@�C�������� */
		for (j = 0; j < 10; j++) {	/* ���g���C���[�v */
			CdReadFile(dfile[i].fname, dfile[i].addr,dfile[i].finfo.size);

			/* ���[�h�̗��Œʏ�̏����͎��s�ł���B*/
			/* �����ł́ARead ���I������܂Ŏc��̃Z�N�^�����Ď� */
			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	/* �����������荞�ݑ҂��i���Ԓ����p�j */

			if (cnt == 0)
				break;	/* ����I���Ȃ�΃��g���C���[�v���f */
		}
	}
}


/*----------------------------------------------------------------------
 �T�E���h�֘A
----------------------------------------------------------------------*/
/* ��������̃T�E���h�f�[�^�̍Đ����� */
static void init_sound()
{
	/* VAB �̃I�[�v������уT�E���h�o�b�t�@�ւ̓]�� */
	vab = SsVabTransfer( (u_char*)VH_ADDR, (u_char*)VB_ADDR, -1, 1 );
	if (vab < 0) {
		printf("SsVabTransfer failed (%d)\n", vab);
		return;
	}

	/* SEQ �̃I�[�v�� */
	seq = SsSeqOpen((u_long *)SEQ_ADDR, vab);
	if (seq < 0)
		printf("SsSeqOpen failed (%d)\n", seq);
}

/* �T�E���h�Đ��̊J�n */
static void play_sound()
{
	SsSetMVol(MVOL, MVOL);			/* ���C���{�����[���̐ݒ� */
	SsSeqSetVol(seq, SVOL, SVOL);		/* SEQ���̃{�����[���̐ݒ� */
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);	/* �Đ��X�C�b�`�n�m */
}

/* �T�E���h�Đ��̏I�� */
static void stop_sound()
{
	SsSeqStop(seq);				/* �Đ��X�C�b�`�n�e�e */
	VSync(0);
	VSync(0);
	SsSeqClose(seq);			/* SEQ�̃N���|�Y */
	SsVabClose(vab);			/* VAB�̃N���[�Y */
}

/* �\�[�X�R�[�h�̏I��� */
