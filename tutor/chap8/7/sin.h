//--------------------------------------------------------------------------
// File: sin.h
// Author: Unknown
// Date: 1997 ???
// Description: 
//				rcos() - takes an angle (in PSX format)
//					   - finds the cosine function of the angle (4096=360 degrees)
//                       using fixed-point math (where 4096=1.0)
//					   - the speed is fast but size is large
//
//
//				rsin() - takes an angle (in PSX format)
//					   - finds the sin function of the angle (4096=360 degrees)
//                       using fixed-point math (where 4096=1.0)
//					   - the speed is fast but size is large 
//
// Notes: Description by George Bain (from PSX Library Overview)
//-------------------------------------------------------------------------- 

#ifndef _SIN_H
#define _SIN_H	

//--------------------------------------------------------------------------
// D E F I N E S 
//--------------------------------------------------------------------------

#define	rsin(a)		(Tsin[(a)&4095])
#define	rcos(a)		(Tsin[((a)+1024)&4095])

static	short	Tsin[4096] =
{
	    0,     6,    12,    18,    25,    31,    37,    43,    50,    56,    62,    69,    75,    81,    87,    94,
	  100,   106,   113,   119,   125,   131,   138,   144,   150,   157,   163,   169,   175,   182,   188,   194,
	  200,   207,   213,   219,   226,   232,   238,   244,   251,   257,   263,   269,   276,   282,   288,   295,
	  301,   307,   313,   320,   326,   332,   338,   345,   351,   357,   363,   370,   376,   382,   388,   395,
	  401,   407,   413,   420,   426,   432,   438,   445,   451,   457,   463,   470,   476,   482,   488,   495,
	  501,   507,   513,   520,   526,   532,   538,   545,   551,   557,   563,   569,   576,   582,   588,   594,
	  601,   607,   613,   619,   625,   632,   638,   644,   650,   656,   663,   669,   675,   681,   687,   694,
	  700,   706,   712,   718,   725,   731,   737,   743,   749,   755,   762,   768,   774,   780,   786,   792,
	  799,   805,   811,   817,   823,   829,   836,   842,   848,   854,   860,   866,   872,   879,   885,   891,
	  897,   903,   909,   915,   921,   928,   934,   940,   946,   952,   958,   964,   970,   976,   983,   989,
	  995,  1001,  1007,  1013,  1019,  1025,  1031,  1037,  1043,  1050,  1056,  1062,  1068,  1074,  1080,  1086,
	 1092,  1098,  1104,  1110,  1116,  1122,  1128,  1134,  1140,  1146,  1152,  1158,  1164,  1170,  1176,  1182,
	 1189,  1195,  1201,  1207,  1213,  1219,  1225,  1231,  1237,  1243,  1248,  1254,  1260,  1266,  1272,  1278,
	 1284,  1290,  1296,  1302,  1308,  1314,  1320,  1326,  1332,  1338,  1344,  1350,  1356,  1362,  1368,  1373,
	 1379,  1385,  1391,  1397,  1403,  1409,  1415,  1421,  1427,  1433,  1438,  1444,  1450,  1456,  1462,  1468,
	 1474,  1479,  1485,  1491,  1497,  1503,  1509,  1515,  1520,  1526,  1532,  1538,  1544,  1550,  1555,  1561,
	 1567,  1573,  1579,  1584,  1590,  1596,  1602,  1608,  1613,  1619,  1625,  1631,  1636,  1642,  1648,  1654,
	 1659,  1665,  1671,  1677,  1682,  1688,  1694,  1699,  1705,  1711,  1717,  1722,  1728,  1734,  1739,  1745,
	 1751,  1756,  1762,  1768,  1773,  1779,  1785,  1790,  1796,  1802,  1807,  1813,  1819,  1824,  1830,  1835,
	 1841,  1847,  1852,  1858,  1864,  1869,  1875,  1880,  1886,  1891,  1897,  1903,  1908,  1914,  1919,  1925,
	 1930,  1936,  1941,  1947,  1952,  1958,  1964,  1969,  1975,  1980,  1986,  1991,  1997,  2002,  2007,  2013,
	 2018,  2024,  2029,  2035,  2040,  2046,  2051,  2057,  2062,  2067,  2073,  2078,  2084,  2089,  2094,  2100,
	 2105,  2111,  2116,  2121,  2127,  2132,  2138,  2143,  2148,  2154,  2159,  2164,  2170,  2175,  2180,  2186,
	 2191,  2196,  2201,  2207,  2212,  2217,  2223,  2228,  2233,  2238,  2244,  2249,  2254,  2259,  2265,  2270,
	 2275,  2280,  2286,  2291,  2296,  2301,  2306,  2312,  2317,  2322,  2327,  2332,  2337,  2343,  2348,  2353,
	 2358,  2363,  2368,  2373,  2379,  2384,  2389,  2394,  2399,  2404,  2409,  2414,  2419,  2424,  2429,  2434,
	 2439,  2445,  2450,  2455,  2460,  2465,  2470,  2475,  2480,  2485,  2490,  2495,  2500,  2505,  2510,  2515,
	 2519,  2524,  2529,  2534,  2539,  2544,  2549,  2554,  2559,  2564,  2569,  2574,  2578,  2583,  2588,  2593,
	 2598,  2603,  2608,  2613,  2617,  2622,  2627,  2632,  2637,  2641,  2646,  2651,  2656,  2661,  2665,  2670,
	 2675,  2680,  2684,  2689,  2694,  2699,  2703,  2708,  2713,  2717,  2722,  2727,  2732,  2736,  2741,  2746,
	 2750,  2755,  2760,  2764,  2769,  2773,  2778,  2783,  2787,  2792,  2796,  2801,  2806,  2810,  2815,  2819,
	 2824,  2828,  2833,  2837,  2842,  2847,  2851,  2856,  2860,  2865,  2869,  2874,  2878,  2882,  2887,  2891,
	 2896,  2900,  2905,  2909,  2914,  2918,  2922,  2927,  2931,  2936,  2940,  2944,  2949,  2953,  2957,  2962,
	 2966,  2970,  2975,  2979,  2983,  2988,  2992,  2996,  3000,  3005,  3009,  3013,  3018,  3022,  3026,  3030,
	 3034,  3039,  3043,  3047,  3051,  3055,  3060,  3064,  3068,  3072,  3076,  3080,  3085,  3089,  3093,  3097,
	 3101,  3105,  3109,  3113,  3117,  3121,  3126,  3130,  3134,  3138,  3142,  3146,  3150,  3154,  3158,  3162,
	 3166,  3170,  3174,  3178,  3182,  3186,  3190,  3193,  3197,  3201,  3205,  3209,  3213,  3217,  3221,  3225,
	 3229,  3232,  3236,  3240,  3244,  3248,  3252,  3255,  3259,  3263,  3267,  3271,  3274,  3278,  3282,  3286,
	 3289,  3293,  3297,  3301,  3304,  3308,  3312,  3315,  3319,  3323,  3326,  3330,  3334,  3337,  3341,  3345,
	 3348,  3352,  3356,  3359,  3363,  3366,  3370,  3373,  3377,  3381,  3384,  3388,  3391,  3395,  3398,  3402,
	 3405,  3409,  3412,  3416,  3419,  3423,  3426,  3429,  3433,  3436,  3440,  3443,  3447,  3450,  3453,  3457,
	 3460,  3463,  3467,  3470,  3473,  3477,  3480,  3483,  3487,  3490,  3493,  3497,  3500,  3503,  3506,  3510,
	 3513,  3516,  3519,  3522,  3526,  3529,  3532,  3535,  3538,  3541,  3545,  3548,  3551,  3554,  3557,  3560,
	 3563,  3566,  3570,  3573,  3576,  3579,  3582,  3585,  3588,  3591,  3594,  3597,  3600,  3603,  3606,  3609,
	 3612,  3615,  3618,  3621,  3624,  3627,  3629,  3632,  3635,  3638,  3641,  3644,  3647,  3650,  3652,  3655,
	 3658,  3661,  3664,  3667,  3669,  3672,  3675,  3678,  3680,  3683,  3686,  3689,  3691,  3694,  3697,  3700,
	 3702,  3705,  3708,  3710,  3713,  3716,  3718,  3721,  3723,  3726,  3729,  3731,  3734,  3736,  3739,  3742,
	 3744,  3747,  3749,  3752,  3754,  3757,  3759,  3762,  3764,  3767,  3769,  3772,  3774,  3776,  3779,  3781,
	 3784,  3786,  3789,  3791,  3793,  3796,  3798,  3800,  3803,  3805,  3807,  3810,  3812,  3814,  3816,  3819,
	 3821,  3823,  3826,  3828,  3830,  3832,  3834,  3837,  3839,  3841,  3843,  3845,  3848,  3850,  3852,  3854,
	 3856,  3858,  3860,  3862,  3864,  3867,  3869,  3871,  3873,  3875,  3877,  3879,  3881,  3883,  3885,  3887,
	 3889,  3891,  3893,  3895,  3897,  3899,  3900,  3902,  3904,  3906,  3908,  3910,  3912,  3914,  3915,  3917,
	 3919,  3921,  3923,  3925,  3926,  3928,  3930,  3932,  3933,  3935,  3937,  3939,  3940,  3942,  3944,  3945,
	 3947,  3949,  3950,  3952,  3954,  3955,  3957,  3959,  3960,  3962,  3963,  3965,  3967,  3968,  3970,  3971,
	 3973,  3974,  3976,  3977,  3979,  3980,  3982,  3983,  3985,  3986,  3988,  3989,  3990,  3992,  3993,  3995,
	 3996,  3997,  3999,  4000,  4001,  4003,  4004,  4005,  4007,  4008,  4009,  4011,  4012,  4013,  4014,  4016,
	 4017,  4018,  4019,  4020,  4022,  4023,  4024,  4025,  4026,  4027,  4029,  4030,  4031,  4032,  4033,  4034,
	 4035,  4036,  4037,  4038,  4039,  4040,  4041,  4042,  4043,  4044,  4045,  4046,  4047,  4048,  4049,  4050,
	 4051,  4052,  4053,  4054,  4055,  4056,  4057,  4057,  4058,  4059,  4060,  4061,  4062,  4062,  4063,  4064,
	 4065,  4065,  4066,  4067,  4068,  4068,  4069,  4070,  4071,  4071,  4072,  4073,  4073,  4074,  4075,  4075,
	 4076,  4076,  4077,  4078,  4078,  4079,  4079,  4080,  4080,  4081,  4081,  4082,  4082,  4083,  4083,  4084,
	 4084,  4085,  4085,  4086,  4086,  4087,  4087,  4087,  4088,  4088,  4089,  4089,  4089,  4090,  4090,  4090,
	 4091,  4091,  4091,  4091,  4092,  4092,  4092,  4092,  4093,  4093,  4093,  4093,  4094,  4094,  4094,  4094,
	 4094,  4094,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,
	 4096,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4095,  4094,
	 4094,  4094,  4094,  4094,  4094,  4093,  4093,  4093,  4093,  4092,  4092,  4092,  4092,  4091,  4091,  4091,
	 4091,  4090,  4090,  4090,  4089,  4089,  4089,  4088,  4088,  4087,  4087,  4087,  4086,  4086,  4085,  4085,
	 4084,  4084,  4083,  4083,  4082,  4082,  4081,  4081,  4080,  4080,  4079,  4079,  4078,  4078,  4077,  4076,
	 4076,  4075,  4075,  4074,  4073,  4073,  4072,  4071,  4071,  4070,  4069,  4068,  4068,  4067,  4066,  4065,
	 4065,  4064,  4063,  4062,  4062,  4061,  4060,  4059,  4058,  4057,  4057,  4056,  4055,  4054,  4053,  4052,
	 4051,  4050,  4049,  4048,  4047,  4046,  4045,  4044,  4043,  4042,  4041,  4040,  4039,  4038,  4037,  4036,
	 4035,  4034,  4033,  4032,  4031,  4030,  4029,  4027,  4026,  4025,  4024,  4023,  4022,  4020,  4019,  4018,
	 4017,  4016,  4014,  4013,  4012,  4011,  4009,  4008,  4007,  4005,  4004,  4003,  4001,  4000,  3999,  3997,
	 3996,  3995,  3993,  3992,  3990,  3989,  3988,  3986,  3985,  3983,  3982,  3980,  3979,  3977,  3976,  3974,
	 3973,  3971,  3970,  3968,  3967,  3965,  3963,  3962,  3960,  3959,  3957,  3955,  3954,  3952,  3950,  3949,
	 3947,  3945,  3944,  3942,  3940,  3939,  3937,  3935,  3933,  3932,  3930,  3928,  3926,  3925,  3923,  3921,
	 3919,  3917,  3915,  3914,  3912,  3910,  3908,  3906,  3904,  3902,  3900,  3899,  3897,  3895,  3893,  3891,
	 3889,  3887,  3885,  3883,  3881,  3879,  3877,  3875,  3873,  3871,  3869,  3867,  3864,  3862,  3860,  3858,
	 3856,  3854,  3852,  3850,  3848,  3845,  3843,  3841,  3839,  3837,  3834,  3832,  3830,  3828,  3826,  3823,
	 3821,  3819,  3816,  3814,  3812,  3810,  3807,  3805,  3803,  3800,  3798,  3796,  3793,  3791,  3789,  3786,
	 3784,  3781,  3779,  3776,  3774,  3772,  3769,  3767,  3764,  3762,  3759,  3757,  3754,  3752,  3749,  3747,
	 3744,  3742,  3739,  3736,  3734,  3731,  3729,  3726,  3723,  3721,  3718,  3716,  3713,  3710,  3708,  3705,
	 3702,  3700,  3697,  3694,  3691,  3689,  3686,  3683,  3680,  3678,  3675,  3672,  3669,  3667,  3664,  3661,
	 3658,  3655,  3652,  3650,  3647,  3644,  3641,  3638,  3635,  3632,  3629,  3627,  3624,  3621,  3618,  3615,
	 3612,  3609,  3606,  3603,  3600,  3597,  3594,  3591,  3588,  3585,  3582,  3579,  3576,  3573,  3570,  3566,
	 3563,  3560,  3557,  3554,  3551,  3548,  3545,  3541,  3538,  3535,  3532,  3529,  3526,  3522,  3519,  3516,
	 3513,  3510,  3506,  3503,  3500,  3497,  3493,  3490,  3487,  3483,  3480,  3477,  3473,  3470,  3467,  3463,
	 3460,  3457,  3453,  3450,  3447,  3443,  3440,  3436,  3433,  3429,  3426,  3423,  3419,  3416,  3412,  3409,
	 3405,  3402,  3398,  3395,  3391,  3388,  3384,  3381,  3377,  3373,  3370,  3366,  3363,  3359,  3356,  3352,
	 3348,  3345,  3341,  3337,  3334,  3330,  3326,  3323,  3319,  3315,  3312,  3308,  3304,  3301,  3297,  3293,
	 3289,  3286,  3282,  3278,  3274,  3271,  3267,  3263,  3259,  3255,  3252,  3248,  3244,  3240,  3236,  3232,
	 3229,  3225,  3221,  3217,  3213,  3209,  3205,  3201,  3197,  3193,  3190,  3186,  3182,  3178,  3174,  3170,
	 3166,  3162,  3158,  3154,  3150,  3146,  3142,  3138,  3134,  3130,  3126,  3121,  3117,  3113,  3109,  3105,
	 3101,  3097,  3093,  3089,  3085,  3080,  3076,  3072,  3068,  3064,  3060,  3055,  3051,  3047,  3043,  3039,
	 3034,  3030,  3026,  3022,  3018,  3013,  3009,  3005,  3000,  2996,  2992,  2988,  2983,  2979,  2975,  2970,
	 2966,  2962,  2957,  2953,  2949,  2944,  2940,  2936,  2931,  2927,  2922,  2918,  2914,  2909,  2905,  2900,
	 2896,  2891,  2887,  2882,  2878,  2874,  2869,  2865,  2860,  2856,  2851,  2847,  2842,  2837,  2833,  2828,
	 2824,  2819,  2815,  2810,  2806,  2801,  2796,  2792,  2787,  2783,  2778,  2773,  2769,  2764,  2760,  2755,
	 2750,  2746,  2741,  2736,  2732,  2727,  2722,  2717,  2713,  2708,  2703,  2699,  2694,  2689,  2684,  2680,
	 2675,  2670,  2665,  2661,  2656,  2651,  2646,  2641,  2637,  2632,  2627,  2622,  2617,  2613,  2608,  2603,
	 2598,  2593,  2588,  2583,  2578,  2574,  2569,  2564,  2559,  2554,  2549,  2544,  2539,  2534,  2529,  2524,
	 2519,  2515,  2510,  2505,  2500,  2495,  2490,  2485,  2480,  2475,  2470,  2465,  2460,  2455,  2450,  2445,
	 2439,  2434,  2429,  2424,  2419,  2414,  2409,  2404,  2399,  2394,  2389,  2384,  2379,  2373,  2368,  2363,
	 2358,  2353,  2348,  2343,  2337,  2332,  2327,  2322,  2317,  2312,  2306,  2301,  2296,  2291,  2286,  2280,
	 2275,  2270,  2265,  2259,  2254,  2249,  2244,  2238,  2233,  2228,  2223,  2217,  2212,  2207,  2201,  2196,
	 2191,  2186,  2180,  2175,  2170,  2164,  2159,  2154,  2148,  2143,  2138,  2132,  2127,  2121,  2116,  2111,
	 2105,  2100,  2094,  2089,  2084,  2078,  2073,  2067,  2062,  2057,  2051,  2046,  2040,  2035,  2029,  2024,
	 2018,  2013,  2007,  2002,  1997,  1991,  1986,  1980,  1975,  1969,  1964,  1958,  1952,  1947,  1941,  1936,
	 1930,  1925,  1919,  1914,  1908,  1903,  1897,  1891,  1886,  1880,  1875,  1869,  1864,  1858,  1852,  1847,
	 1841,  1835,  1830,  1824,  1819,  1813,  1807,  1802,  1796,  1790,  1785,  1779,  1773,  1768,  1762,  1756,
	 1751,  1745,  1739,  1734,  1728,  1722,  1717,  1711,  1705,  1699,  1694,  1688,  1682,  1677,  1671,  1665,
	 1659,  1654,  1648,  1642,  1636,  1631,  1625,  1619,  1613,  1608,  1602,  1596,  1590,  1584,  1579,  1573,
	 1567,  1561,  1555,  1550,  1544,  1538,  1532,  1526,  1520,  1515,  1509,  1503,  1497,  1491,  1485,  1479,
	 1474,  1468,  1462,  1456,  1450,  1444,  1438,  1433,  1427,  1421,  1415,  1409,  1403,  1397,  1391,  1385,
	 1379,  1373,  1368,  1362,  1356,  1350,  1344,  1338,  1332,  1326,  1320,  1314,  1308,  1302,  1296,  1290,
	 1284,  1278,  1272,  1266,  1260,  1254,  1248,  1243,  1237,  1231,  1225,  1219,  1213,  1207,  1201,  1195,
	 1189,  1182,  1176,  1170,  1164,  1158,  1152,  1146,  1140,  1134,  1128,  1122,  1116,  1110,  1104,  1098,
	 1092,  1086,  1080,  1074,  1068,  1062,  1056,  1050,  1043,  1037,  1031,  1025,  1019,  1013,  1007,  1001,
	  995,   989,   983,   976,   970,   964,   958,   952,   946,   940,   934,   928,   921,   915,   909,   903,
	  897,   891,   885,   879,   872,   866,   860,   854,   848,   842,   836,   829,   823,   817,   811,   805,
	  799,   792,   786,   780,   774,   768,   762,   755,   749,   743,   737,   731,   725,   718,   712,   706,
	  700,   694,   687,   681,   675,   669,   663,   656,   650,   644,   638,   632,   625,   619,   613,   607,
	  601,   594,   588,   582,   576,   569,   563,   557,   551,   545,   538,   532,   526,   520,   513,   507,
	  501,   495,   488,   482,   476,   470,   463,   457,   451,   445,   438,   432,   426,   420,   413,   407,
	  401,   395,   388,   382,   376,   370,   363,   357,   351,   345,   338,   332,   326,   320,   313,   307,
	  301,   295,   288,   282,   276,   269,   263,   257,   251,   244,   238,   232,   226,   219,   213,   207,
	  200,   194,   188,   182,   175,   169,   163,   157,   150,   144,   138,   131,   125,   119,   113,   106,
	  100,    94,    87,    81,    75,    69,    62,    56,    50,    43,    37,    31,    25,    18,    12,     6,
	    0,    -6,   -12,   -18,   -25,   -31,   -37,   -43,   -50,   -56,   -62,   -69,   -75,   -81,   -87,   -94,
	 -100,  -106,  -113,  -119,  -125,  -131,  -138,  -144,  -150,  -157,  -163,  -169,  -175,  -182,  -188,  -194,
	 -200,  -207,  -213,  -219,  -226,  -232,  -238,  -244,  -251,  -257,  -263,  -269,  -276,  -282,  -288,  -295,
	 -301,  -307,  -313,  -320,  -326,  -332,  -338,  -345,  -351,  -357,  -363,  -370,  -376,  -382,  -388,  -395,
	 -401,  -407,  -413,  -420,  -426,  -432,  -438,  -445,  -451,  -457,  -463,  -470,  -476,  -482,  -488,  -495,
	 -501,  -507,  -513,  -520,  -526,  -532,  -538,  -545,  -551,  -557,  -563,  -569,  -576,  -582,  -588,  -594,
	 -601,  -607,  -613,  -619,  -625,  -632,  -638,  -644,  -650,  -656,  -663,  -669,  -675,  -681,  -687,  -694,
	 -700,  -706,  -712,  -718,  -725,  -731,  -737,  -743,  -749,  -755,  -762,  -768,  -774,  -780,  -786,  -792,
	 -799,  -805,  -811,  -817,  -823,  -829,  -836,  -842,  -848,  -854,  -860,  -866,  -872,  -879,  -885,  -891,
	 -897,  -903,  -909,  -915,  -921,  -928,  -934,  -940,  -946,  -952,  -958,  -964,  -970,  -976,  -983,  -989,
	 -995, -1001, -1007, -1013, -1019, -1025, -1031, -1037, -1043, -1050, -1056, -1062, -1068, -1074, -1080, -1086,
	-1092, -1098, -1104, -1110, -1116, -1122, -1128, -1134, -1140, -1146, -1152, -1158, -1164, -1170, -1176, -1182,
	-1189, -1195, -1201, -1207, -1213, -1219, -1225, -1231, -1237, -1243, -1248, -1254, -1260, -1266, -1272, -1278,
	-1284, -1290, -1296, -1302, -1308, -1314, -1320, -1326, -1332, -1338, -1344, -1350, -1356, -1362, -1368, -1373,
	-1379, -1385, -1391, -1397, -1403, -1409, -1415, -1421, -1427, -1433, -1438, -1444, -1450, -1456, -1462, -1468,
	-1474, -1479, -1485, -1491, -1497, -1503, -1509, -1515, -1520, -1526, -1532, -1538, -1544, -1550, -1555, -1561,
	-1567, -1573, -1579, -1584, -1590, -1596, -1602, -1608, -1613, -1619, -1625, -1631, -1636, -1642, -1648, -1654,
	-1659, -1665, -1671, -1677, -1682, -1688, -1694, -1699, -1705, -1711, -1717, -1722, -1728, -1734, -1739, -1745,
	-1751, -1756, -1762, -1768, -1773, -1779, -1785, -1790, -1796, -1802, -1807, -1813, -1819, -1824, -1830, -1835,
	-1841, -1847, -1852, -1858, -1864, -1869, -1875, -1880, -1886, -1891, -1897, -1903, -1908, -1914, -1919, -1925,
	-1930, -1936, -1941, -1947, -1952, -1958, -1964, -1969, -1975, -1980, -1986, -1991, -1997, -2002, -2007, -2013,
	-2018, -2024, -2029, -2035, -2040, -2046, -2051, -2057, -2062, -2067, -2073, -2078, -2084, -2089, -2094, -2100,
	-2105, -2111, -2116, -2121, -2127, -2132, -2138, -2143, -2148, -2154, -2159, -2164, -2170, -2175, -2180, -2186,
	-2191, -2196, -2201, -2207, -2212, -2217, -2223, -2228, -2233, -2238, -2244, -2249, -2254, -2259, -2265, -2270,
	-2275, -2280, -2286, -2291, -2296, -2301, -2306, -2312, -2317, -2322, -2327, -2332, -2337, -2343, -2348, -2353,
	-2358, -2363, -2368, -2373, -2379, -2384, -2389, -2394, -2399, -2404, -2409, -2414, -2419, -2424, -2429, -2434,
	-2439, -2445, -2450, -2455, -2460, -2465, -2470, -2475, -2480, -2485, -2490, -2495, -2500, -2505, -2510, -2515,
	-2519, -2524, -2529, -2534, -2539, -2544, -2549, -2554, -2559, -2564, -2569, -2574, -2578, -2583, -2588, -2593,
	-2598, -2603, -2608, -2613, -2617, -2622, -2627, -2632, -2637, -2641, -2646, -2651, -2656, -2661, -2665, -2670,
	-2675, -2680, -2684, -2689, -2694, -2699, -2703, -2708, -2713, -2717, -2722, -2727, -2732, -2736, -2741, -2746,
	-2750, -2755, -2760, -2764, -2769, -2773, -2778, -2783, -2787, -2792, -2796, -2801, -2806, -2810, -2815, -2819,
	-2824, -2828, -2833, -2837, -2842, -2847, -2851, -2856, -2860, -2865, -2869, -2874, -2878, -2882, -2887, -2891,
	-2896, -2900, -2905, -2909, -2914, -2918, -2922, -2927, -2931, -2936, -2940, -2944, -2949, -2953, -2957, -2962,
	-2966, -2970, -2975, -2979, -2983, -2988, -2992, -2996, -3000, -3005, -3009, -3013, -3018, -3022, -3026, -3030,
	-3034, -3039, -3043, -3047, -3051, -3055, -3060, -3064, -3068, -3072, -3076, -3080, -3085, -3089, -3093, -3097,
	-3101, -3105, -3109, -3113, -3117, -3121, -3126, -3130, -3134, -3138, -3142, -3146, -3150, -3154, -3158, -3162,
	-3166, -3170, -3174, -3178, -3182, -3186, -3190, -3193, -3197, -3201, -3205, -3209, -3213, -3217, -3221, -3225,
	-3229, -3232, -3236, -3240, -3244, -3248, -3252, -3255, -3259, -3263, -3267, -3271, -3274, -3278, -3282, -3286,
	-3289, -3293, -3297, -3301, -3304, -3308, -3312, -3315, -3319, -3323, -3326, -3330, -3334, -3337, -3341, -3345,
	-3348, -3352, -3356, -3359, -3363, -3366, -3370, -3373, -3377, -3381, -3384, -3388, -3391, -3395, -3398, -3402,
	-3405, -3409, -3412, -3416, -3419, -3423, -3426, -3429, -3433, -3436, -3440, -3443, -3447, -3450, -3453, -3457,
	-3460, -3463, -3467, -3470, -3473, -3477, -3480, -3483, -3487, -3490, -3493, -3497, -3500, -3503, -3506, -3510,
	-3513, -3516, -3519, -3522, -3526, -3529, -3532, -3535, -3538, -3541, -3545, -3548, -3551, -3554, -3557, -3560,
	-3563, -3566, -3570, -3573, -3576, -3579, -3582, -3585, -3588, -3591, -3594, -3597, -3600, -3603, -3606, -3609,
	-3612, -3615, -3618, -3621, -3624, -3627, -3629, -3632, -3635, -3638, -3641, -3644, -3647, -3650, -3652, -3655,
	-3658, -3661, -3664, -3667, -3669, -3672, -3675, -3678, -3680, -3683, -3686, -3689, -3691, -3694, -3697, -3700,
	-3702, -3705, -3708, -3710, -3713, -3716, -3718, -3721, -3723, -3726, -3729, -3731, -3734, -3736, -3739, -3742,
	-3744, -3747, -3749, -3752, -3754, -3757, -3759, -3762, -3764, -3767, -3769, -3772, -3774, -3776, -3779, -3781,
	-3784, -3786, -3789, -3791, -3793, -3796, -3798, -3800, -3803, -3805, -3807, -3810, -3812, -3814, -3816, -3819,
	-3821, -3823, -3826, -3828, -3830, -3832, -3834, -3837, -3839, -3841, -3843, -3845, -3848, -3850, -3852, -3854,
	-3856, -3858, -3860, -3862, -3864, -3867, -3869, -3871, -3873, -3875, -3877, -3879, -3881, -3883, -3885, -3887,
	-3889, -3891, -3893, -3895, -3897, -3899, -3900, -3902, -3904, -3906, -3908, -3910, -3912, -3914, -3915, -3917,
	-3919, -3921, -3923, -3925, -3926, -3928, -3930, -3932, -3933, -3935, -3937, -3939, -3940, -3942, -3944, -3945,
	-3947, -3949, -3950, -3952, -3954, -3955, -3957, -3959, -3960, -3962, -3963, -3965, -3967, -3968, -3970, -3971,
	-3973, -3974, -3976, -3977, -3979, -3980, -3982, -3983, -3985, -3986, -3988, -3989, -3990, -3992, -3993, -3995,
	-3996, -3997, -3999, -4000, -4001, -4003, -4004, -4005, -4007, -4008, -4009, -4011, -4012, -4013, -4014, -4016,
	-4017, -4018, -4019, -4020, -4022, -4023, -4024, -4025, -4026, -4027, -4029, -4030, -4031, -4032, -4033, -4034,
	-4035, -4036, -4037, -4038, -4039, -4040, -4041, -4042, -4043, -4044, -4045, -4046, -4047, -4048, -4049, -4050,
	-4051, -4052, -4053, -4054, -4055, -4056, -4057, -4057, -4058, -4059, -4060, -4061, -4062, -4062, -4063, -4064,
	-4065, -4065, -4066, -4067, -4068, -4068, -4069, -4070, -4071, -4071, -4072, -4073, -4073, -4074, -4075, -4075,
	-4076, -4076, -4077, -4078, -4078, -4079, -4079, -4080, -4080, -4081, -4081, -4082, -4082, -4083, -4083, -4084,
	-4084, -4085, -4085, -4086, -4086, -4087, -4087, -4087, -4088, -4088, -4089, -4089, -4089, -4090, -4090, -4090,
	-4091, -4091, -4091, -4091, -4092, -4092, -4092, -4092, -4093, -4093, -4093, -4093, -4094, -4094, -4094, -4094,
	-4094, -4094, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095,
	-4096, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4095, -4094,
	-4094, -4094, -4094, -4094, -4094, -4093, -4093, -4093, -4093, -4092, -4092, -4092, -4092, -4091, -4091, -4091,
	-4091, -4090, -4090, -4090, -4089, -4089, -4089, -4088, -4088, -4087, -4087, -4087, -4086, -4086, -4085, -4085,
	-4084, -4084, -4083, -4083, -4082, -4082, -4081, -4081, -4080, -4080, -4079, -4079, -4078, -4078, -4077, -4076,
	-4076, -4075, -4075, -4074, -4073, -4073, -4072, -4071, -4071, -4070, -4069, -4068, -4068, -4067, -4066, -4065,
	-4065, -4064, -4063, -4062, -4062, -4061, -4060, -4059, -4058, -4057, -4057, -4056, -4055, -4054, -4053, -4052,
	-4051, -4050, -4049, -4048, -4047, -4046, -4045, -4044, -4043, -4042, -4041, -4040, -4039, -4038, -4037, -4036,
	-4035, -4034, -4033, -4032, -4031, -4030, -4029, -4027, -4026, -4025, -4024, -4023, -4022, -4020, -4019, -4018,
	-4017, -4016, -4014, -4013, -4012, -4011, -4009, -4008, -4007, -4005, -4004, -4003, -4001, -4000, -3999, -3997,
	-3996, -3995, -3993, -3992, -3990, -3989, -3988, -3986, -3985, -3983, -3982, -3980, -3979, -3977, -3976, -3974,
	-3973, -3971, -3970, -3968, -3967, -3965, -3963, -3962, -3960, -3959, -3957, -3955, -3954, -3952, -3950, -3949,
	-3947, -3945, -3944, -3942, -3940, -3939, -3937, -3935, -3933, -3932, -3930, -3928, -3926, -3925, -3923, -3921,
	-3919, -3917, -3915, -3914, -3912, -3910, -3908, -3906, -3904, -3902, -3900, -3899, -3897, -3895, -3893, -3891,
	-3889, -3887, -3885, -3883, -3881, -3879, -3877, -3875, -3873, -3871, -3869, -3867, -3864, -3862, -3860, -3858,
	-3856, -3854, -3852, -3850, -3848, -3845, -3843, -3841, -3839, -3837, -3834, -3832, -3830, -3828, -3826, -3823,
	-3821, -3819, -3816, -3814, -3812, -3810, -3807, -3805, -3803, -3800, -3798, -3796, -3793, -3791, -3789, -3786,
	-3784, -3781, -3779, -3776, -3774, -3772, -3769, -3767, -3764, -3762, -3759, -3757, -3754, -3752, -3749, -3747,
	-3744, -3742, -3739, -3736, -3734, -3731, -3729, -3726, -3723, -3721, -3718, -3716, -3713, -3710, -3708, -3705,
	-3702, -3700, -3697, -3694, -3691, -3689, -3686, -3683, -3680, -3678, -3675, -3672, -3669, -3667, -3664, -3661,
	-3658, -3655, -3652, -3650, -3647, -3644, -3641, -3638, -3635, -3632, -3629, -3627, -3624, -3621, -3618, -3615,
	-3612, -3609, -3606, -3603, -3600, -3597, -3594, -3591, -3588, -3585, -3582, -3579, -3576, -3573, -3570, -3566,
	-3563, -3560, -3557, -3554, -3551, -3548, -3545, -3541, -3538, -3535, -3532, -3529, -3526, -3522, -3519, -3516,
	-3513, -3510, -3506, -3503, -3500, -3497, -3493, -3490, -3487, -3483, -3480, -3477, -3473, -3470, -3467, -3463,
	-3460, -3457, -3453, -3450, -3447, -3443, -3440, -3436, -3433, -3429, -3426, -3423, -3419, -3416, -3412, -3409,
	-3405, -3402, -3398, -3395, -3391, -3388, -3384, -3381, -3377, -3373, -3370, -3366, -3363, -3359, -3356, -3352,
	-3348, -3345, -3341, -3337, -3334, -3330, -3326, -3323, -3319, -3315, -3312, -3308, -3304, -3301, -3297, -3293,
	-3289, -3286, -3282, -3278, -3274, -3271, -3267, -3263, -3259, -3255, -3252, -3248, -3244, -3240, -3236, -3232,
	-3229, -3225, -3221, -3217, -3213, -3209, -3205, -3201, -3197, -3193, -3190, -3186, -3182, -3178, -3174, -3170,
	-3166, -3162, -3158, -3154, -3150, -3146, -3142, -3138, -3134, -3130, -3126, -3121, -3117, -3113, -3109, -3105,
	-3101, -3097, -3093, -3089, -3085, -3080, -3076, -3072, -3068, -3064, -3060, -3055, -3051, -3047, -3043, -3039,
	-3034, -3030, -3026, -3022, -3018, -3013, -3009, -3005, -3000, -2996, -2992, -2988, -2983, -2979, -2975, -2970,
	-2966, -2962, -2957, -2953, -2949, -2944, -2940, -2936, -2931, -2927, -2922, -2918, -2914, -2909, -2905, -2900,
	-2896, -2891, -2887, -2882, -2878, -2874, -2869, -2865, -2860, -2856, -2851, -2847, -2842, -2837, -2833, -2828,
	-2824, -2819, -2815, -2810, -2806, -2801, -2796, -2792, -2787, -2783, -2778, -2773, -2769, -2764, -2760, -2755,
	-2750, -2746, -2741, -2736, -2732, -2727, -2722, -2717, -2713, -2708, -2703, -2699, -2694, -2689, -2684, -2680,
	-2675, -2670, -2665, -2661, -2656, -2651, -2646, -2641, -2637, -2632, -2627, -2622, -2617, -2613, -2608, -2603,
	-2598, -2593, -2588, -2583, -2578, -2574, -2569, -2564, -2559, -2554, -2549, -2544, -2539, -2534, -2529, -2524,
	-2519, -2515, -2510, -2505, -2500, -2495, -2490, -2485, -2480, -2475, -2470, -2465, -2460, -2455, -2450, -2445,
	-2439, -2434, -2429, -2424, -2419, -2414, -2409, -2404, -2399, -2394, -2389, -2384, -2379, -2373, -2368, -2363,
	-2358, -2353, -2348, -2343, -2337, -2332, -2327, -2322, -2317, -2312, -2306, -2301, -2296, -2291, -2286, -2280,
	-2275, -2270, -2265, -2259, -2254, -2249, -2244, -2238, -2233, -2228, -2223, -2217, -2212, -2207, -2201, -2196,
	-2191, -2186, -2180, -2175, -2170, -2164, -2159, -2154, -2148, -2143, -2138, -2132, -2127, -2121, -2116, -2111,
	-2105, -2100, -2094, -2089, -2084, -2078, -2073, -2067, -2062, -2057, -2051, -2046, -2040, -2035, -2029, -2024,
	-2018, -2013, -2007, -2002, -1997, -1991, -1986, -1980, -1975, -1969, -1964, -1958, -1952, -1947, -1941, -1936,
	-1930, -1925, -1919, -1914, -1908, -1903, -1897, -1891, -1886, -1880, -1875, -1869, -1864, -1858, -1852, -1847,
	-1841, -1835, -1830, -1824, -1819, -1813, -1807, -1802, -1796, -1790, -1785, -1779, -1773, -1768, -1762, -1756,
	-1751, -1745, -1739, -1734, -1728, -1722, -1717, -1711, -1705, -1699, -1694, -1688, -1682, -1677, -1671, -1665,
	-1659, -1654, -1648, -1642, -1636, -1631, -1625, -1619, -1613, -1608, -1602, -1596, -1590, -1584, -1579, -1573,
	-1567, -1561, -1555, -1550, -1544, -1538, -1532, -1526, -1520, -1515, -1509, -1503, -1497, -1491, -1485, -1479,
	-1474, -1468, -1462, -1456, -1450, -1444, -1438, -1433, -1427, -1421, -1415, -1409, -1403, -1397, -1391, -1385,
	-1379, -1373, -1368, -1362, -1356, -1350, -1344, -1338, -1332, -1326, -1320, -1314, -1308, -1302, -1296, -1290,
	-1284, -1278, -1272, -1266, -1260, -1254, -1248, -1243, -1237, -1231, -1225, -1219, -1213, -1207, -1201, -1195,
	-1189, -1182, -1176, -1170, -1164, -1158, -1152, -1146, -1140, -1134, -1128, -1122, -1116, -1110, -1104, -1098,
	-1092, -1086, -1080, -1074, -1068, -1062, -1056, -1050, -1043, -1037, -1031, -1025, -1019, -1013, -1007, -1001,
	 -995,  -989,  -983,  -976,  -970,  -964,  -958,  -952,  -946,  -940,  -934,  -928,  -921,  -915,  -909,  -903,
	 -897,  -891,  -885,  -879,  -872,  -866,  -860,  -854,  -848,  -842,  -836,  -829,  -823,  -817,  -811,  -805,
	 -799,  -792,  -786,  -780,  -774,  -768,  -762,  -755,  -749,  -743,  -737,  -731,  -725,  -718,  -712,  -706,
	 -700,  -694,  -687,  -681,  -675,  -669,  -663,  -656,  -650,  -644,  -638,  -632,  -625,  -619,  -613,  -607,
	 -601,  -594,  -588,  -582,  -576,  -569,  -563,  -557,  -551,  -545,  -538,  -532,  -526,  -520,  -513,  -507,
	 -501,  -495,  -488,  -482,  -476,  -470,  -463,  -457,  -451,  -445,  -438,  -432,  -426,  -420,  -413,  -407,
	 -401,  -395,  -388,  -382,  -376,  -370,  -363,  -357,  -351,  -345,  -338,  -332,  -326,  -320,  -313,  -307,
	 -301,  -295,  -288,  -282,  -276,  -269,  -263,  -257,  -251,  -244,  -238,  -232,  -226,  -219,  -213,  -207,
	 -200,  -194,  -188,  -182,  -175,  -169,  -163,  -157,  -150,  -144,  -138,  -131,  -125,  -119,  -113,  -106,
	 -100,   -94,   -87,   -81,   -75,   -69,   -62,   -56,   -50,   -43,   -37,   -31,   -25,   -18,   -12,    -6,
};

#if 0
#define	ratan(a)	(Tatn[(a)*4096/360])
static	short	Tatn[256] =
{
	 0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,
	 3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,
	 7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10,
	10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13,
	14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20,
	20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23,
	23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26,
	26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29,
	29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31,
	32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34,
	34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36,
	36, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38,
	39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 41,
	41, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 43,
	43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 44, 44,
};
#endif



#endif // _SIN_H

//----------------------------------EOF-------------------------------------