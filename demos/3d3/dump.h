#ifndef DUMP_H_INCLUDED



/************************************************************
 *															*
 *						dump.h								*
 *															*
 *															*
 *		Set of debug-output structure-printing routines		*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/






#define dumpRECT(rect)											\
			printf("RECT: x %d, y %d, w %d, h %d\n",			\
				((rect)->x), ((rect)->y), ((rect)->w), ((rect)->h))



#define dumpVECTOR(vector)											\
			printf("VECTOR: %ld, %ld, %ld\n",						\
				(vector)->vx, (vector)->vy, (vector)->vz)

#define dumpSVECTOR(svector)										\
			printf("SVECTOR: %d, %d, %d\n",						\
				(svector)->vx, (svector)->vy, (svector)->vz)


#define dumpMATRIX(matrix)																\
			printf("MATRIX: m: [0]: %d, %d, %d\n[1]: %d, %d, %d\n[2]: %d, %d, %d\n",	\
					(matrix)->m[0][0], (matrix)->m[0][1], (matrix)->m[0][2], 			\
					(matrix)->m[1][0], (matrix)->m[1][1], (matrix)->m[1][2], 			\
					(matrix)->m[2][0], (matrix)->m[2][1], (matrix)->m[2][2]),			\
			printf("MATRIX: t: %ld %ld %ld\n", 								 			\
					(matrix)->t[0], (matrix)->t[1], (matrix)->t[2]) 										 



#define dumpCOORD2(coord2)																					\
			printf("GsCOORDINATE2: flg: %u\n", ((unsigned int)(coord2)->flg)),												\
			printf("GsCOORDINATE2: coord.m\n[0]: %d, %d, %d\n[1]: %d, %d, %d\n[2]: %d, %d, %d\n",			\
					(coord2)->coord.m[0][0], (coord2)->coord.m[0][1], (coord2)->coord.m[0][2], 				\
					(coord2)->coord.m[1][0], (coord2)->coord.m[1][1], (coord2)->coord.m[1][2],				\
					(coord2)->coord.m[2][0], (coord2)->coord.m[2][1], (coord2)->coord.m[2][2]),				\
			printf("GsCOORDINATE2: coord.t: %ld %ld %ld\n", 										 			\
					(coord2)->coord.t[0], (coord2)->coord.t[1], (coord2)->coord.t[2]),						\
			printf("GsCOORDINATE2: super pointer: %u\n", ((unsigned int)(coord2)->super))



#define dumpOBJ2(obj2)															\
			printf("GsDOBJ2: attribute %d\n", (obj2)->attribute),				\
			printf("GsDOBJ2: coord pointer %u\n", (u_long)(obj2)->coord2),		\
			printf("GsDOBJ2: tmd pointer %u\n", (u_long)(obj2)->tmd),			\
			printf("GsDOBJ2: id %d\n", (obj2)->id)						
			
			
			
			
#define dumpRVIEW2(rview2)												\
			printf("GsRVIEW2: vr: %d, %d, %d\n",						\
				(rview2)->vrx, (rview2)->vry, (rview2)->vrz),			\
			printf("GsRVIEW2: vp %d, %d, %d\n",							\
				(rview2)->vpx, (rview2)->vpy, (rview2)->vpz),  			\
			printf("GsRVIEW2: rz %d\n", (rview2)->rz),					\
			printf("GsRVIEW2: super pointer %d\n", (rview2)->super)



#define dumpVIEW2(view2)																				\
			printf("GsVIEW2: view.m: [0]: %d, %d, %d\n[1]: %d, %d, %d\n[2]: %d, %d, %d\n",				\
					(view2)->view.m[0][0], (view2)->view.m[0][1], (view2)->view.m[0][2], 				\
					(view2)->view.m[1][0], (view2)->view.m[1][1], (view2)->view.m[1][2],				\
					(view2)->view.m[2][0], (view2)->coord.m[2][1], (view2)->view.m[2][2]),				\
			printf("GsVIEW2: view.t: %d %d %d\view", 										 			\
					(view2)->view.t[0], (view2)->view.t[1], (view2)->view.t[2]),						\
			printf("GsVIEW2: super pointer %d\n", (view)->super)
			




#define dumpFOG(fog)														\
			printf("GsFOGPARAM: dqa %d, dqb %d\n", (fog)->dqa, (fog)->dqb),	\
			printf("GsFOGPARAM: colours: %d %d %d\n",						\
				(fog)->rfc, (fog)->gfc, (fog)->bfc)



#define dumpLIGHT(light)													\
			printf("GsFLIGHT: direction vector %d, %d, %d\n",				\
				(light)->vx, (light)->vy, (light)->vz),						\
			printf("GsFLIGHT: colour %d, %d, %d\n",							\
				(light)->r, (light)->g, (light)->b) 




#define dumpGLINE(gline)													\
			printf("GsGLINE: attribute %d\n", (gline)->attribute),			\
			printf("GsGLINE: x0 %d, y0 %d\n", (gline)->x0, (gline)->y0),	\
			printf("GsGLINE: x1 %d, y1 %d\n", (gline)->x1, (gline)->y1),	\
			printf("GsGLINE: r0 %d, g0 %d, b0 %d\n", 						\
				(gline)->r0, (gline)->g0), (gline)->b0),					\
			printf("GsGLINE: r1 %d, g1 %d, b1 %d\n", 						\
				(gline)->r1, (gline)->g1), (gline)->b1)


#define dumpLINE(line)													\
			printf("GsLINE: attribute %d\n", (line)->attribute),		\
			printf("GsLINE: x0 %d, y0 %d\n", (line)->x0, (line)->y0),	\
			printf("GsLINE: x1 %d, y1 %d\n", (line)->x1, (line)->y1),	\
		   	printf("GsGLINE: r %d, g %d, b %d\n", 						\
				(line)->r, (line)->g), (line)->b)


	  

#define dumpBG(bg)														\
			printf("GsBG: attribute %d\n", (bg)->attribute),			\
			printf("GsBG: x %d, y %d, w %d, h %d\n",					\
				(bg)->x, (bg)->y, (bg)->w, (bg)->h),					\
			printf("GsBG: r %d, g %d, b %d\n",							\
				(bg)->r, (bg)->g, (bg)->b),								\
			printf("GsBG: map pointer %d\n", (bg)->map),				\
			printf("GsBG: mx %d, my %d\n", (bg)->mx, (bg)->my),			\
			printf("GsBG: scalex %d, scaley %d\n",						\
				(bg)->scalex, (bg)->scaley),							\
			printf("GsBG: rotate %d\n", (bg)->rotate)

	 
#define dumpMAP(map)													\
			printf("GsMAP: cellw %d, cellh %d\n", 						\
				(map)->cellw, (map)->cellh),							\
			printf("GsMAP: ncellw %d, ncellh %d\n", 					\
				(map)->ncellw, (map)->ncellh),							\
			printf("GsMAP: base pointer %d\n", (map)->base),			\
			printf("GsMAP: index pointer %d\n", (map)->index)

#define dumpCELL(cell)													\
			printf("GsCELL: u %d, v %d\n", (cell)->u, (cell)->v),		\
			printf("GsCELL: cba %d, flag %d, tpage %d\n",				\
				(cell)->cba, (cell)->flag, (cell)->tpage)


#define dumpSPRITE(sprite)													\
			printf("GsSPRITE: attribute 0x%X\n", (sprite)->attribute),		\
			printf("GsSPRITE: x %d, y %d, w %d, h %d\n", 					\
				(sprite)->x, (sprite)->y, (sprite)->w, (sprite->h)),		\
			printf("GsSPRITE: tpage %d, u %d, v %d\n",						\
				(sprite)->tpage, (sprite)->u, (sprite)->v),					\
			printf("GsSPRITE: cx %d, cy %d\n", (sprite)->cx, (sprite)->cy),	\
			printf("GsSPRITE: r %d, g %d, b %d\n",							\
				(sprite)->r, (sprite)->g, (sprite)->b),						\
			printf("GsSPRITE: mx %d, my %d\n", (sprite)->mx, (sprite)->my),	\
			printf("GsSPRITE: scalex %d, scaley %d, rotate %d\n",			\
				(sprite)->scalex, (sprite)->scaley, (sprite)->rotate)




#define dumpIMAGE(image)											 \
		printf("GsIMAGE: pmode %d\n", (image)->pmode),				 \
		printf("px %d py %d; pw %d ph %d\n",						 \
			(image)->px, (image)->py, (image)->pw, (image)->ph),	 \
		printf("cx %d cy %d; cw %d ch %d\n",						 \
			(image)->cx, (image)->cy, (image)->cw, (image)->ch)
		 


// MORE TO DO: see libgs

// GsBOXF, etc

// see GPU for standard dump<structure> functions






#define DUMP_H_INCLUDED 1

#endif