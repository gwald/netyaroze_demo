/* $PSLibId: Run-time Library Release 4.0$ */
/*
 *	TOD functions (part2)
 :	TOD�A�j���[�V���������֐��Q�i���̂Q�j
 *
 *
 *		Version 1.30	Apr, 17, 1996
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */


#include <libps.h>

#include "tod.h"

extern GsF_LIGHT pslt[3];		/* Light source: �����~�R�� */

/*
 *  Prototype definitions: �v���g�^�C�v�錾
 */
u_long *TodSetFrame();
u_long *TodSetPacket();

/*
 *  TOD process for 1 frame
 :  �P�t���[������TOD�f�[�^�̏���
 */
u_long *TodSetFrame(currentFrame, todP, objTblP, tmdIdP, tmdTblP, mode)
int currentFrame;	/* Current time:���݂̎����i�t���[���ԍ��j */
u_long *todP;		/* Current TOD:�Đ�����TOD�f�[�^�ւ̃|�C���^ */
TodOBJTABLE *objTblP;	/* Object table:�I�u�W�F�N�g�e�[�u���ւ̃|�C���^ */
int *tmdIdP;		/* TMD ID list:���f�����O�f�[�^ID���X�g */
u_long *tmdTblP;	/* TMD data addr:TMD�f�[�^�ւ̃|�C���^ */
int mode;		/* Playing mode:�Đ����[�h */
{
	u_long hdr;
	u_long nPacket;
	u_long frame;
	int i;

	/* Get frame info.:�t���[�����𓾂� */
	hdr = *todP;			/* :�t���[���w�b�_�𓾂� */
	nPacket = (hdr>>16)&0x0ffff;	/* Number of packet:�p�P�b�g�� */
	frame = *(todP+1);		/* :�����i�t���[���ԍ��j */

	/* Wait until specified frame time
	 : ���ݎ����̃t���[���łȂ���Ή����������Ȃ�
	 */
	if(frame > currentFrame) return todP;

	/* Execute each packet in a frame
	 : �t���[���̒��̊e�p�P�b�g���������� */
	todP += 2;
	for(i = 0; i < nPacket; i++) {
		todP = TodSetPacket(todP, objTblP, tmdIdP, tmdTblP, mode);
	}

	/* Return next frame addr.
	 : ���̃t���[���ւ̃|�C���^��Ԃ� */
	return todP;	
}

/*
 *  Execute single packet
 :  �P�p�P�b�g�̏���
 */
u_long *TodSetPacket(packetP, tblP, tmdIdP, tmdTblP, mode)
u_long *packetP;
TodOBJTABLE *tblP;
int *tmdIdP;
u_long *tmdTblP;
int mode;
{
	u_long *dP;		/* Current TOD addr:��������TOD�ւ̃|�C���^ */

	u_long hdr;
	u_long id;
	u_long flag;
	u_long type;
	u_long len;

	/* Current object: �������I�u�W�F�N�g */
	GsDOBJ2 *objP;		/* �I�u�W�F�N�g�ւ̃|�C���^ */
	GsCOORD2PARAM *cparam;	/* Coordinate value(RST): ���W�p�����[�^(R,S,T) */
	MATRIX *coordp;		/* Coordinate matrix: ���W�}�g���N�X */
	GsDOBJ2 *parentP;	/* Parent object: �e�I�u�W�F�N�g */
	VECTOR v;
	SVECTOR sv;

	/* Dummy object: �_�~�[�I�u�W�F�N�g�ϐ� */
	GsDOBJ2 dummyObj;
	MATRIX dummyObjCoord;
	GsCOORD2PARAM dummyObjParam;


	/* Get Packet Info: �p�P�b�g���𓾂� */
	dP = packetP;
	hdr = *dP++;
	id = hdr&0x0ffff;	/* ID number */
	type = (hdr>>16)&0x0f;	/* Packet type (TOD_???) */
	flag = (hdr>>20)&0x0f;	/* Flags */
	len = (hdr>>24)&0x0ff;	/* Word lengthfor packet: �p�P�b�g�̃��[�h�� */

	/* Search specified object from ID
	 : ID�ԍ��ŊY������I�u�W�F�N�g���T�[�` */
	objP = TodSearchObjByID(tblP, id);
	if(objP == NULL) {
		/* Use dummy object when not found
		 : �Ȃ��ꍇ�̓_�~�[�I�u�W�F�N�g���g�� */
		objP = &dummyObj;
		coordp = &dummyObjCoord;
		cparam = &dummyObjParam;
	}
	else {
		coordp = &(objP->coord2->coord);
		cparam = (objP->coord2->param);
		objP->coord2->flg = 0;
	}

	/* �p�P�b�g�̃^�C�v�ʂɏ��� */
	switch(type) {
	    case TOD_ATTR:/* Update attribute: �A�g���r���[�g�ύX */
		objP->attribute = (objP->attribute&*dP)|*(dP+1);	
		dP += 2;
		break;

	    case TOD_COORD:/* Update Coordinate value
			    : ���W�ύX�i�ړ��^��]�^�g��j */

		/* Update 'coordinate' member of current object
		   using 'cparam' member.
		 : �Y���I�u�W�F�N�g��coordinate�̓��e���X�V����
		   �X�V����p�����[�^��cparam�����o�ɕۊǂ��Ă��� */

		if(flag&0x01) {

			/* Differencial value: �O�t���[������̍����̏ꍇ */

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
			/* Immediate value: ��Βl�̏ꍇ */

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

	    case TOD_TMDID:	/* Link to TMD: TMD�Ƃ̃����N��ݒ� */
		if(tmdTblP != NULL) {
			GsLinkObject4((u_long)TodSearchTMDByID(tmdTblP, tmdIdP,
					(unsigned long)(*dP&0xffff)), objP, 0);

		}
		break;

	    case TOD_PARENT:	/* Set parent: �e�I�u�W�F�N�g�̐ݒ� */
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
		/* in the objtable: �e�[�u�����I�u�W�F�N�g�̊Ǘ� */
		if(tblP != NULL) {
			switch(flag) {
			    case 0:
				/* Create new object: �V�K�I�u�W�F�N�g�̐��� */
				TodCreateNewObj(tblP, id);
				break;
			    case 1:
				/* Delete object: �I�u�W�F�N�g�̍폜 */
				TodRemoveObj(tblP, id);
				break;
			}
		}
		break;

	    case TOD_LIGHT:
		/* Light source control: �����̐ݒ� */
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
		 : �����Ƀ��[�U��`�p�P�b�g�̏������L�q����  */
		break;
	}
	/* Return next packet addr: ���̃p�P�b�g�ւ̃|�C���^��Ԃ� */
	return packetP+len;
}
