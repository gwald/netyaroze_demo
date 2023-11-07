                    .text                            # BB:0,L:0,H:1c
                    .globl     main                  
                    .ent       main                  
main:                                                # CS:1000,ETS:0
                                                     # Line number: 328
 000000  27bdffa0   addiu      $sp,$sp,-96           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 332
 000008  afa00058   sw         $0,88($sp)            # frameNumber
                                                     # Line number: 336
 00000c  3c058017   lui        $a1,0x8017            
 000010  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@26
 000014  0c000000   jal        MWbload               # R_MIPS_26,MWbload
 000018  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@26
                                                     # Line number: 337
 00001c  3c058016   lui        $a1,0x8016            # BB:1,L:1c,H:2c
 000020  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@27
 000024  0c000000   jal        MWbload               # R_MIPS_26,MWbload
 000028  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@27
                                                     # Line number: 340
 00002c  0c000000   jal        InitialiseAll         # BB:2,L:2c,H:34,R_MIPS_26,InitialiseAll
 000030  00000000   nop                              
                                                     # Line number: 344
 000034  100000de   b          $100                  # BB:3,L:34,H:3c
 000038  00000000   nop                              
$101:                                                # BB:4,L:3c,H:44
                                                     # Line number: 346
 00003c  0c000000   jal        DealWithControllerPad # R_MIPS_26,DealWithControllerPad
 000040  00000000   nop                              
                                                     # Line number: 348
 000044  0c000000   jal        HandleAllObjects      # BB:5,L:44,H:4c,R_MIPS_26,HandleAllObjects
 000048  00000000   nop                              
                                                     # Line number: 350
 00004c  0c000000   jal        GsGetActiveBuff       # BB:6,L:4c,H:54,R_MIPS_26,GsGetActiveBuff
 000050  00000000   nop                              
 000054  afa20050   sw         $v0,80($sp)           # BB:7,L:54,H:78,activeBuffer
                                                     # Line number: 351
 000058  8fa80050   lw         $t0,80($sp)           # activeBuffer
 00005c  240912f0   addiu      $t1,$0,4848           
 000060  01090018   mult       $t0,$t1               
 000064  00005012   mflo       $t2                   
 000068  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,GpuPacketArea
 00006c  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,GpuPacketArea
 000070  0c000000   jal        GsSetWorkBase         # R_MIPS_26,GsSetWorkBase
 000074  016a2021   addu       $a0,$t3,$t2           
                                                     # Line number: 353
 000078  8fac0050   lw         $t4,80($sp)           # BB:8,L:78,H:a0,activeBuffer
 00007c  240d0014   addiu      $t5,$0,20             
 000080  018d0018   mult       $t4,$t5               
 000084  00007012   mflo       $t6                   
 000088  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,WorldOrderingTable
 00008c  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,WorldOrderingTable
 000090  010e3021   addu       $a2,$t0,$t6           
 000094  00002021   move       $a0,$0                
 000098  0c000000   jal        GsClearOt             # R_MIPS_26,GsClearOt
 00009c  00002821   move       $a1,$0                
                                                     # Line number: 355
 0000a0  8fa90050   lw         $t1,80($sp)           # BB:9,L:a0,H:cc,activeBuffer
 0000a4  240a0014   addiu      $t2,$0,20             
 0000a8  012a0018   mult       $t1,$t2               
 0000ac  00005812   mflo       $t3                   
 0000b0  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0000b4  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,WorldOrderingTable
 0000b8  018b2821   addu       $a1,$t4,$t3           
 0000bc  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,BackgroundRectangle
 0000c0  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,BackgroundRectangle
 0000c4  0c000000   jal        GsSortBoxFill         # R_MIPS_26,GsSortBoxFill
 0000c8  00003021   move       $a2,$0                
                                                     # Line number: 358
 0000cc  8fad0050   lw         $t5,80($sp)           # BB:10,L:cc,H:f8,activeBuffer
 0000d0  240e0014   addiu      $t6,$0,20             
 0000d4  01ae0018   mult       $t5,$t6               
 0000d8  00004012   mflo       $t0                   
 0000dc  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0000e0  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,WorldOrderingTable
 0000e4  01282821   addu       $a1,$t1,$t0           
 0000e8  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,PlayersSprite
 0000ec  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,PlayersSprite
 0000f0  0c000000   jal        GsSortSprite          # R_MIPS_26,GsSortSprite
 0000f4  00003021   move       $a2,$0                
                                                     # Line number: 361
 0000f8  8faa0050   lw         $t2,80($sp)           # BB:11,L:f8,H:124,activeBuffer
 0000fc  240b0014   addiu      $t3,$0,20             
 000100  014b0018   mult       $t2,$t3               
 000104  00006012   mflo       $t4                   
 000108  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,WorldOrderingTable
 00010c  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,WorldOrderingTable
 000110  01ac2821   addu       $a1,$t5,$t4           
 000114  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,BallsSprite
 000118  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,BallsSprite
 00011c  0c000000   jal        GsSortSprite          # R_MIPS_26,GsSortSprite
 000120  00003021   move       $a2,$0                
                                                     # Line number: 364
 000124  10000021   b          $102                  # BB:12,L:124,H:12c
 000128  afa0005c   sw         $0,92($sp)            # i
$103:                                                # BB:13,L:12c,H:158
                                                     # Line number: 366
 00012c  8fae005c   lw         $t6,92($sp)           # i
 000130  2408004c   addiu      $t0,$0,76             
 000134  01c80018   mult       $t6,$t0               
 000138  00004812   mflo       $t1                   
 00013c  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000140  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 000144  01495821   addu       $t3,$t2,$t1           
 000148  8d6c0004   lw         $t4,4($t3)            
 00014c  24010001   addiu      $at,$0,1              
 000150  15810012   bne        $t4,$at,$104          
 000154  00000000   nop                              
                                                     # Line number: 368
 000158  8fad005c   lw         $t5,92($sp)           # BB:14,L:158,H:19c,i
 00015c  240e004c   addiu      $t6,$0,76             
 000160  01ae0018   mult       $t5,$t6               
 000164  00004012   mflo       $t0                   
 000168  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 00016c  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000170  01285021   addu       $t2,$t1,$t0           
 000174  8fab0050   lw         $t3,80($sp)           # activeBuffer
 000178  240c0014   addiu      $t4,$0,20             
 00017c  016c0018   mult       $t3,$t4               
 000180  00006812   mflo       $t5                   
 000184  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000188  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,WorldOrderingTable
 00018c  01cd2821   addu       $a1,$t6,$t5           
 000190  25440010   addiu      $a0,$t2,16            
 000194  0c000000   jal        GsSortSprite          # R_MIPS_26,GsSortSprite
 000198  00003021   move       $a2,$0                
                                                     # Line number: 370
$104:                                                # BB:15,L:19c,H:1ac
                                                     # Line number: 371
 00019c  8fa8005c   lw         $t0,92($sp)           # i
 0001a0  00000000   nop                              
 0001a4  21090001   addi       $t1,$t0,1             
 0001a8  afa9005c   sw         $t1,92($sp)           # i
$102:                                                # BB:16,L:1ac,H:1c0
 0001ac  8faa005c   lw         $t2,92($sp)           # i
 0001b0  00000000   nop                              
 0001b4  294100c8   slti       $at,$t2,200           
 0001b8  1420ffdc   bne        $at,$0,$103           
 0001bc  00000000   nop                              
                                                     # Line number: 373
 0001c0  0c000000   jal        DrawSync              # BB:17,L:1c0,H:1c8,R_MIPS_26,DrawSync
 0001c4  00002021   move       $a0,$0                
                                                     # Line number: 374
 0001c8  0c000000   jal        VSync                 # BB:18,L:1c8,H:1d0,R_MIPS_26,VSync
 0001cc  00002021   move       $a0,$0                
                                                     # BB:19,L:1d0,H:1d8,Line number: 375
 0001d0  0c000000   jal        GsSwapDispBuff        # R_MIPS_26,GsSwapDispBuff
 0001d4  afa20054   sw         $v0,84($sp)           # count
                                                     # Line number: 377
 0001d8  8fab0050   lw         $t3,80($sp)           # BB:20,L:1d8,H:204,activeBuffer
 0001dc  240c0014   addiu      $t4,$0,20             
 0001e0  016c0018   mult       $t3,$t4               
 0001e4  00006812   mflo       $t5                   
 0001e8  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0001ec  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,WorldOrderingTable
 0001f0  01cd3821   addu       $a3,$t6,$t5           
 0001f4  2404003c   addiu      $a0,$0,60             
 0001f8  24050078   addiu      $a1,$0,120            
 0001fc  0c000000   jal        GsSortClear           # R_MIPS_26,GsSortClear
 000200  24060078   addiu      $a2,$0,120            
                                                     # Line number: 379
 000204  8fa80050   lw         $t0,80($sp)           # BB:21,L:204,H:224,activeBuffer
 000208  24090014   addiu      $t1,$0,20             
 00020c  01090018   mult       $t0,$t1               
 000210  00005012   mflo       $t2                   
 000214  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000218  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,WorldOrderingTable
 00021c  0c000000   jal        GsDrawOt              # R_MIPS_26,GsDrawOt
 000220  016a2021   addu       $a0,$t3,$t2           
                                                     # Line number: 381
 000224  3c040000   lui        $a0,0x0000            # BB:22,L:224,H:230,R_MIPS_HI16,@28
 000228  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 00022c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@28
                                                     # Line number: 382
 000230  3c040000   lui        $a0,0x0000            # BB:23,L:230,H:244,R_MIPS_HI16,@29
 000234  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@29
 000238  8fa50058   lw         $a1,88($sp)           # frameNumber
 00023c  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000240  00000000   nop                              
                                                     # Line number: 383
 000244  3c040000   lui        $a0,0x0000            # BB:24,L:244,H:25c,R_MIPS_HI16,@30
 000248  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@30
 00024c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
 000250  8c250000   lw         $a1,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
 000254  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000258  00000000   nop                              
                                                     # Line number: 384
 00025c  3c040000   lui        $a0,0x0000            # BB:25,L:25c,H:274,R_MIPS_HI16,@31
 000260  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@31
 000264  3c010000   lui        $at,0x0000            # R_MIPS_HI16,CurrentScore
 000268  8c250010   lw         $a1,16($at)           # CurrentScore,R_MIPS_LO16,CurrentScore
 00026c  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000270  00000000   nop                              
                                                     # Line number: 386
 000274  0c000000   jal        FntFlush              # BB:26,L:274,H:27c,R_MIPS_26,FntFlush
 000278  2404ffff   addiu      $a0,$0,-1             
                                                     # Line number: 388
 00027c  0c000000   jal        CountNumberOfBricks   # BB:27,L:27c,H:284,R_MIPS_26,CountNumberOfBricks
 000280  00000000   nop                              
 000284  14400028   bne        $v0,$0,$105           # BB:28,L:284,H:28c
 000288  00000000   nop                              
                                                     # Line number: 390
 00028c  0c000000   jal        CrudestPause          # BB:29,L:28c,H:294,R_MIPS_26,CrudestPause
 000290  2404001e   addiu      $a0,$0,30             
                                                     # Line number: 391
 000294  3c010000   lui        $at,0x0000            # BB:30,L:294,H:2c4,R_MIPS_HI16,Level
 000298  8c2c0000   lw         $t4,0($at)            # Level,R_MIPS_LO16,Level
 00029c  00000000   nop                              
 0002a0  218d0001   addi       $t5,$t4,1             
 0002a4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Level
 0002a8  ac2d0000   sw         $t5,0($at)            # Level,R_MIPS_LO16,Level
                                                     # Line number: 392
 0002ac  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Level
 0002b0  8c2e0000   lw         $t6,0($at)            # Level,R_MIPS_LO16,Level
 0002b4  00000000   nop                              
 0002b8  29c1000b   slti       $at,$t6,11            
 0002bc  14200018   bne        $at,$0,$106           
 0002c0  00000000   nop                              
                                                     # Line number: 394
 0002c4  0c000000   jal        DealWithPlayerFinishingTenLevels c4,H:2cc,R_MIPS_26,DealWithPlayerFinishingTenLevels
 0002c8  00000000   nop                              
 0002cc  afa2004c   sw         $v0,76($sp)           # BB:32,L:2cc,H:2e0,playerWantsAnotherGame
                                                     # Line number: 396
 0002d0  8fa8004c   lw         $t0,76($sp)           # playerWantsAnotherGame
 0002d4  00000000   nop                              
 0002d8  1100000d   beq        $t0,$0,$107           
 0002dc  00000000   nop                              
                                                     # Line number: 398
 0002e0  24090001   addiu      $t1,$0,1              # BB:33,L:2e0,H:2fc
 0002e4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Level
 0002e8  ac290000   sw         $t1,0($at)            # Level,R_MIPS_LO16,Level
                                                     # Line number: 399
 0002ec  240a0005   addiu      $t2,$0,5              
 0002f0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
                                                     # Line number: 400
 0002f4  0c000000   jal        DestroyAllBricks      # R_MIPS_26,DestroyAllBricks
 0002f8  ac2a0000   sw         $t2,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
                                                     # Line number: 401
 0002fc  3c010000   lui        $at,0x0000            # BB:34,L:2fc,H:308,R_MIPS_HI16,CurrentScore
                                                     # Line number: 402
 000300  0c000000   jal        SetUpLevel1           # R_MIPS_26,SetUpLevel1
 000304  ac200010   sw         $0,16($at)            # CurrentScore,R_MIPS_LO16,CurrentScore
                                                     # Line number: 403
 000308  10000007   b          $108                  # BB:35,L:308,H:310
 00030c  00000000   nop                              
$107:                                                # BB:36,L:310,H:320
                                                     # Line number: 405
 000310  240b0001   addiu      $t3,$0,1              
 000314  3c010000   lui        $at,0x0000            # R_MIPS_HI16,QuitFlag
                                                     # Line number: 406
 000318  10000003   b          $108                  
 00031c  ac2b0000   sw         $t3,0($at)            # QuitFlag,R_MIPS_LO16,QuitFlag
$106:                                                # BB:37,L:320,H:328
                                                     # Line number: 408
 000320  0c000000   jal        SetUpLevel1           # R_MIPS_26,SetUpLevel1
 000324  00000000   nop                              
$108:                                                # BB:38,L:328,H:328
                                                     # Line number: 409
$105:                                                # BB:39,L:328,H:340
                                                     # Line number: 411
 000328  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
 00032c  8c2c0000   lw         $t4,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
 000330  00000000   nop                              
 000334  000c082a   slt        $at,$0,$t4            
 000338  14200019   bne        $at,$0,$109           
 00033c  00000000   nop                              
                                                     # Line number: 413
 000340  0c000000   jal        CrudestPause          # BB:40,L:340,H:348,R_MIPS_26,CrudestPause
 000344  2404001e   addiu      $a0,$0,30             
                                                     # Line number: 414
 000348  0c000000   jal        DealWithPlayersDeath  # BB:41,L:348,H:350,R_MIPS_26,DealWithPlayersDeath
 00034c  00000000   nop                              
 000350  afa2004c   sw         $v0,76($sp)           # BB:42,L:350,H:364,playerWantsAnotherGame
                                                     # Line number: 415
 000354  8fad004c   lw         $t5,76($sp)           # playerWantsAnotherGame
 000358  00000000   nop                              
 00035c  11a0000d   beq        $t5,$0,$110           
 000360  00000000   nop                              
                                                     # Line number: 417
 000364  240e0001   addiu      $t6,$0,1              # BB:43,L:364,H:380
 000368  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Level
 00036c  ac2e0000   sw         $t6,0($at)            # Level,R_MIPS_LO16,Level
                                                     # Line number: 418
 000370  24080005   addiu      $t0,$0,5              
 000374  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
                                                     # Line number: 419
 000378  0c000000   jal        DestroyAllBricks      # R_MIPS_26,DestroyAllBricks
 00037c  ac280000   sw         $t0,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
                                                     # Line number: 420
 000380  3c010000   lui        $at,0x0000            # BB:44,L:380,H:38c,R_MIPS_HI16,CurrentScore
                                                     # Line number: 421
 000384  0c000000   jal        SetUpLevel1           # R_MIPS_26,SetUpLevel1
 000388  ac200010   sw         $0,16($at)            # CurrentScore,R_MIPS_LO16,CurrentScore
                                                     # Line number: 422
 00038c  10000004   b          $111                  # BB:45,L:38c,H:394
 000390  00000000   nop                              
$110:                                                # BB:46,L:394,H:3a0
                                                     # Line number: 424
 000394  24090001   addiu      $t1,$0,1              
 000398  3c010000   lui        $at,0x0000            # R_MIPS_HI16,QuitFlag
 00039c  ac290000   sw         $t1,0($at)            # QuitFlag,R_MIPS_LO16,QuitFlag
$111:                                                # BB:47,L:3a0,H:3a0
                                                     # Line number: 425
$109:                                                # BB:48,L:3a0,H:3b0
                                                     # Line number: 427
 0003a0  8faa0058   lw         $t2,88($sp)           # frameNumber
 0003a4  00000000   nop                              
 0003a8  254b0001   addiu      $t3,$t2,1             
 0003ac  afab0058   sw         $t3,88($sp)           # frameNumber
                                                     # Line number: 428
$100:                                                # BB:49,L:3b0,H:3c4
 0003b0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,QuitFlag
 0003b4  8c2c0000   lw         $t4,0($at)            # QuitFlag,R_MIPS_LO16,QuitFlag
 0003b8  00000000   nop                              
 0003bc  1180ff1f   beq        $t4,$0,$101           
 0003c0  00000000   nop                              
                                                     # Line number: 431
 0003c4  0c000000   jal        StopSound             # BB:50,L:3c4,H:3cc,R_MIPS_26,StopSound
 0003c8  00000000   nop                              
                                                     # Line number: 432
 0003cc  0c000000   jal        ResetGraph            # BB:51,L:3cc,H:3d4,R_MIPS_26,ResetGraph
 0003d0  24040003   addiu      $a0,$0,3              
                                                     # Line number: 433
 0003d4  00001021   move       $v0,$0                # BB:52,L:3d4,H:3e8
                                                     # Line number: 434
 0003d8  8fbf003c   lw         $ra,60($sp)           
 0003dc  00000000   nop                              
 0003e0  03e00008   jr         $ra                   
 0003e4  27bd0060   addiu      $sp,$sp,96            
                    .end       main                  
 # *****************************************

                    .text                            # BB:0,L:0,H:14
                    .globl     CrudestPause          
                    .ent       CrudestPause          
CrudestPause:                                        # CS:84,ETS:0
                                                     # Line number: 442
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
 000008  afa40050   sw         $a0,80($sp)           # n
                                                     # Line number: 445
 00000c  10000007   b          $100                  
 000010  afa0004c   sw         $0,76($sp)            # i
$101:                                                # BB:1,L:14,H:1c
                                                     # Line number: 446
 000014  0c000000   jal        VSync                 # R_MIPS_26,VSync
 000018  00002021   move       $a0,$0                
 00001c  8fa8004c   lw         $t0,76($sp)           # BB:2,L:1c,H:2c,i
 000020  00000000   nop                              
 000024  21090001   addi       $t1,$t0,1             
 000028  afa9004c   sw         $t1,76($sp)           # i
$100:                                                # BB:3,L:2c,H:44
 00002c  8faa004c   lw         $t2,76($sp)           # i
 000030  8fab0050   lw         $t3,80($sp)           # n
 000034  00000000   nop                              
 000038  014b082a   slt        $at,$t2,$t3           
 00003c  1420fff5   bne        $at,$0,$101           
 000040  00000000   nop                              
                                                     # Line number: 447
 000044  8fbf003c   lw         $ra,60($sp)           # BB:4,L:44,H:54
 000048  00000000   nop                              
 00004c  03e00008   jr         $ra                   
 000050  27bd0050   addiu      $sp,$sp,80            
                    .end       CrudestPause          
 # *****************************************

                    .text                            # BB:0,L:0,H:8
                    .globl     PauseUntilL1          
                    .ent       PauseUntilL1          
PauseUntilL1:                                        # CS:72,ETS:0
                                                     # Line number: 453
 000000  27bdffc0   addiu      $sp,$sp,-64           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 454
$100:                                                # BB:1,L:8,H:10
                                                     # Line number: 456
 000008  0c000000   jal        PadRead               # R_MIPS_26,PadRead
 00000c  00000000   nop                              
 000010  3c010000   lui        $at,0x0000            # BB:2,L:10,H:20,R_MIPS_HI16,PadStatus
 000014  ac220000   sw         $v0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
                                                     # Line number: 458
 000018  0c000000   jal        VSync                 # R_MIPS_26,VSync
 00001c  00002021   move       $a0,$0                
                                                     # Line number: 460
 000020  3c010000   lui        $at,0x0000            # BB:3,L:20,H:38,R_MIPS_HI16,PadStatus
 000024  8c280000   lw         $t0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000028  00000000   nop                              
 00002c  31090004   andi       $t1,$t0,0x0004        
 000030  1120fff5   beq        $t1,$0,$100           
 000034  00000000   nop                              
                                                     # Line number: 463
 000038  8fbf003c   lw         $ra,60($sp)           # BB:4,L:38,H:48
 00003c  00000000   nop                              
 000040  03e00008   jr         $ra                   
 000044  27bd0040   addiu      $sp,$sp,64            
                    .end       PauseUntilL1          
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     SetUpLevel1           
                    .ent       SetUpLevel1           
SetUpLevel1:                                         # CS:664,ETS:0
                                                     # Line number: 474
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 478
 000008  0c000000   jal        RestartPlayersBat     # R_MIPS_26,RestartPlayersBat
 00000c  00000000   nop                              
                                                     # Line number: 480
 000010  0c000000   jal        RestartTheBall        # BB:1,L:10,H:18,R_MIPS_26,RestartTheBall
 000014  00000000   nop                              
                                                     # Line number: 483
 000018  1000002e   b          $100                  # BB:2,L:18,H:20
 00001c  afa00048   sw         $0,72($sp)            # i
$101:                                                # BB:3,L:20,H:28
                                                     # Line number: 485
 000020  10000023   b          $102                  
 000024  afa0004c   sw         $0,76($sp)            # j
$103:                                                # BB:4,L:28,H:34
                                                     # Line number: 487
 000028  24040064   addiu      $a0,$0,100            
 00002c  0c000000   jal        CreateNewBrick        # R_MIPS_26,CreateNewBrick
 000030  24050001   addiu      $a1,$0,1              
 000034  afa20044   sw         $v0,68($sp)           # BB:5,L:34,H:b0,id
                                                     # Line number: 488
 000038  8fa8004c   lw         $t0,76($sp)           # j
 00003c  24090012   addiu      $t1,$0,18             
 000040  01090018   mult       $t0,$t1               
 000044  00005012   mflo       $t2                   
 000048  214b0010   addi       $t3,$t2,16            
 00004c  8fac0044   lw         $t4,68($sp)           # id
 000050  240d004c   addiu      $t5,$0,76             
 000054  018d0018   mult       $t4,$t5               
 000058  00007012   mflo       $t6                   
 00005c  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 000060  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 000064  010e4821   addu       $t1,$t0,$t6           
 000068  ad2b0034   sw         $t3,52($t1)           
                                                     # Line number: 489
 00006c  8faa0048   lw         $t2,72($sp)           # i
 000070  240b0012   addiu      $t3,$0,18             
 000074  014b0018   mult       $t2,$t3               
 000078  00006012   mflo       $t4                   
 00007c  218d003b   addi       $t5,$t4,59            
 000080  8fae0044   lw         $t6,68($sp)           # id
 000084  2408004c   addiu      $t0,$0,76             
 000088  01c80018   mult       $t6,$t0               
 00008c  00004812   mflo       $t1                   
 000090  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000094  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 000098  01495821   addu       $t3,$t2,$t1           
 00009c  ad6d0038   sw         $t5,56($t3)           
                                                     # Line number: 490
 0000a0  8fac004c   lw         $t4,76($sp)           # j
 0000a4  00000000   nop                              
 0000a8  218d0001   addi       $t5,$t4,1             
 0000ac  afad004c   sw         $t5,76($sp)           # j
$102:                                                # BB:6,L:b0,H:c4
 0000b0  8fae004c   lw         $t6,76($sp)           # j
 0000b4  00000000   nop                              
 0000b8  29c1000f   slti       $at,$t6,15            
 0000bc  1420ffda   bne        $at,$0,$103           
 0000c0  00000000   nop                              
                                                     # Line number: 491
 0000c4  8fa80048   lw         $t0,72($sp)           # BB:7,L:c4,H:d4,i
 0000c8  00000000   nop                              
 0000cc  21090001   addi       $t1,$t0,1             
 0000d0  afa90048   sw         $t1,72($sp)           # i
$100:                                                # BB:8,L:d4,H:e8
 0000d4  8faa0048   lw         $t2,72($sp)           # i
 0000d8  00000000   nop                              
 0000dc  29410003   slti       $at,$t2,3             
 0000e0  1420ffcf   bne        $at,$0,$101           
 0000e4  00000000   nop                              
                                                     # Line number: 492
 0000e8  10000062   b          $104                  # BB:9,L:e8,H:f0
 0000ec  afa00048   sw         $0,72($sp)            # i
$105:                                                # BB:10,L:f0,H:f8
                                                     # Line number: 494
 0000f0  10000023   b          $106                  
 0000f4  afa0004c   sw         $0,76($sp)            # j
$107:                                                # BB:11,L:f8,H:104
                                                     # Line number: 496
 0000f8  24040064   addiu      $a0,$0,100            
 0000fc  0c000000   jal        CreateNewBrick        # R_MIPS_26,CreateNewBrick
 000100  24050001   addiu      $a1,$0,1              
 000104  afa20044   sw         $v0,68($sp)           # BB:12,L:104,H:180,id
                                                     # Line number: 497
 000108  8fab004c   lw         $t3,76($sp)           # j
 00010c  240c0012   addiu      $t4,$0,18             
 000110  016c0018   mult       $t3,$t4               
 000114  00006812   mflo       $t5                   
 000118  21ae0019   addi       $t6,$t5,25            
 00011c  8fa80044   lw         $t0,68($sp)           # id
 000120  2409004c   addiu      $t1,$0,76             
 000124  01090018   mult       $t0,$t1               
 000128  00005012   mflo       $t2                   
 00012c  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000130  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000134  016a6021   addu       $t4,$t3,$t2           
 000138  ad8e0034   sw         $t6,52($t4)           
                                                     # Line number: 498
 00013c  8fad0048   lw         $t5,72($sp)           # i
 000140  240e0012   addiu      $t6,$0,18             
 000144  01ae0018   mult       $t5,$t6               
 000148  00004012   mflo       $t0                   
 00014c  21090044   addi       $t1,$t0,68            
 000150  8faa0044   lw         $t2,68($sp)           # id
 000154  240b004c   addiu      $t3,$0,76             
 000158  014b0018   mult       $t2,$t3               
 00015c  00006012   mflo       $t4                   
 000160  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000164  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000168  01ac7021   addu       $t6,$t5,$t4           
 00016c  adc90038   sw         $t1,56($t6)           
                                                     # Line number: 499
 000170  8fa8004c   lw         $t0,76($sp)           # j
 000174  00000000   nop                              
 000178  21090001   addi       $t1,$t0,1             
 00017c  afa9004c   sw         $t1,76($sp)           # j
$106:                                                # BB:13,L:180,H:194
 000180  8faa004c   lw         $t2,76($sp)           # j
 000184  00000000   nop                              
 000188  2941000e   slti       $at,$t2,14            
 00018c  1420ffda   bne        $at,$0,$107           
 000190  00000000   nop                              
                                                     # Line number: 500
 000194  24040065   addiu      $a0,$0,101            # BB:14,L:194,H:1a0
 000198  0c000000   jal        CreateNewBrick        # R_MIPS_26,CreateNewBrick
 00019c  24050001   addiu      $a1,$0,1              
 0001a0  afa20044   sw         $v0,68($sp)           # BB:15,L:1a0,H:208,id
                                                     # Line number: 501
 0001a4  240b0010   addiu      $t3,$0,16             
 0001a8  8fac0044   lw         $t4,68($sp)           # id
 0001ac  240d004c   addiu      $t5,$0,76             
 0001b0  018d0018   mult       $t4,$t5               
 0001b4  00007012   mflo       $t6                   
 0001b8  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0001bc  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 0001c0  010e4821   addu       $t1,$t0,$t6           
 0001c4  ad2b0034   sw         $t3,52($t1)           
                                                     # Line number: 502
 0001c8  8faa0048   lw         $t2,72($sp)           # i
 0001cc  240b0012   addiu      $t3,$0,18             
 0001d0  014b0018   mult       $t2,$t3               
 0001d4  00006012   mflo       $t4                   
 0001d8  218d0044   addi       $t5,$t4,68            
 0001dc  8fae0044   lw         $t6,68($sp)           # id
 0001e0  2408004c   addiu      $t0,$0,76             
 0001e4  01c80018   mult       $t6,$t0               
 0001e8  00004812   mflo       $t1                   
 0001ec  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 0001f0  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 0001f4  01495821   addu       $t3,$t2,$t1           
 0001f8  ad6d0038   sw         $t5,56($t3)           
                                                     # Line number: 503
 0001fc  24040065   addiu      $a0,$0,101            
 000200  0c000000   jal        CreateNewBrick        # R_MIPS_26,CreateNewBrick
 000204  24050001   addiu      $a1,$0,1              
 000208  afa20044   sw         $v0,68($sp)           # BB:16,L:208,H:274,id
                                                     # Line number: 504
 00020c  240c0115   addiu      $t4,$0,277            
 000210  8fad0044   lw         $t5,68($sp)           # id
 000214  240e004c   addiu      $t6,$0,76             
 000218  01ae0018   mult       $t5,$t6               
 00021c  00004012   mflo       $t0                   
 000220  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000224  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000228  01285021   addu       $t2,$t1,$t0           
 00022c  ad4c0034   sw         $t4,52($t2)           
                                                     # Line number: 505
 000230  8fab0048   lw         $t3,72($sp)           # i
 000234  240c0012   addiu      $t4,$0,18             
 000238  016c0018   mult       $t3,$t4               
 00023c  00006812   mflo       $t5                   
 000240  21ae0044   addi       $t6,$t5,68            
 000244  8fa80044   lw         $t0,68($sp)           # id
 000248  2409004c   addiu      $t1,$0,76             
 00024c  01090018   mult       $t0,$t1               
 000250  00005012   mflo       $t2                   
 000254  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000258  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 00025c  016a6021   addu       $t4,$t3,$t2           
 000260  ad8e0038   sw         $t6,56($t4)           
                                                     # Line number: 506
 000264  8fad0048   lw         $t5,72($sp)           # i
 000268  00000000   nop                              
 00026c  21ae0001   addi       $t6,$t5,1             
 000270  afae0048   sw         $t6,72($sp)           # i
$104:                                                # BB:17,L:274,H:288
 000274  8fa80048   lw         $t0,72($sp)           # i
 000278  00000000   nop                              
 00027c  29010002   slti       $at,$t0,2             
 000280  1420ff9b   bne        $at,$0,$105           
 000284  00000000   nop                              
                                                     # Line number: 507
 000288  8fbf003c   lw         $ra,60($sp)           # BB:18,L:288,H:298
 00028c  00000000   nop                              
 000290  03e00008   jr         $ra                   
 000294  27bd0050   addiu      $sp,$sp,80            
                    .end       SetUpLevel1           
 # *****************************************

                    .text                            # BB:0,L:0,H:34
                    .globl     DealWithPlayersDeath  
                    .ent       DealWithPlayersDeath  
DealWithPlayersDeath:                                # CS:756,ETS:0
                                                     # Line number: 515
 000000  27bdffa0   addiu      $sp,$sp,-96           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 520
 000008  afa00050   sw         $0,80($sp)            # frameNow
 00000c  2408ffff   addiu      $t0,$0,-1             
 000010  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
 000014  afa00058   sw         $0,88($sp)            # exitSet
                                                     # Line number: 525
 000018  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,CurrentScore
 00001c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,CurrentScore
 000020  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,HighScoreTable
 000024  20a50000   addi       $a1,$a1,0             # R_MIPS_LO16,HighScoreTable
 000028  24060005   addiu      $a2,$0,5              
 00002c  0c000000   jal        DealWithNewScore      # R_MIPS_26,DealWithNewScore
 000030  27a70044   addiu      $a3,$sp,68            
                                                     # Line number: 527
$100:                                                # BB:1,L:34,H:3c
                                                     # Line number: 529
 000034  0c000000   jal        PadRead               # R_MIPS_26,PadRead
 000038  00000000   nop                              
 00003c  3c010000   lui        $at,0x0000            # BB:2,L:3c,H:5c,R_MIPS_HI16,PadStatus
 000040  ac220000   sw         $v0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
                                                     # Line number: 531
 000044  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 000048  8c290000   lw         $t1,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 00004c  00000000   nop                              
 000050  312a0004   andi       $t2,$t1,0x0004        
 000054  11400012   beq        $t2,$0,$101           
 000058  00000000   nop                              
 00005c  3c010000   lui        $at,0x0000            # BB:3,L:5c,H:74,R_MIPS_HI16,PadStatus
 000060  8c2b0000   lw         $t3,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000064  00000000   nop                              
 000068  316c0008   andi       $t4,$t3,0x0008        
 00006c  1180000c   beq        $t4,$0,$101           
 000070  00000000   nop                              
                                                     # Line number: 533
 000074  8fad0058   lw         $t5,88($sp)           # BB:4,L:74,H:84,exitSet
 000078  00000000   nop                              
 00007c  15a00008   bne        $t5,$0,$102           
 000080  00000000   nop                              
                                                     # Line number: 535
 000084  afa00040   sw         $0,64($sp)            # BB:5,L:84,H:a0,newGame
                                                     # Line number: 536
 000088  8fae0050   lw         $t6,80($sp)           # frameNow
 00008c  00000000   nop                              
 000090  21c8003c   addi       $t0,$t6,60            
 000094  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
                                                     # Line number: 537
 000098  24090001   addiu      $t1,$0,1              
 00009c  afa90058   sw         $t1,88($sp)           # exitSet
                                                     # Line number: 538
$102:                                                # BB:6,L:a0,H:a0
                                                     # Line number: 539
$101:                                                # BB:7,L:a0,H:b8
                                                     # Line number: 540
 0000a0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 0000a4  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 0000a8  00000000   nop                              
 0000ac  314b0040   andi       $t3,$t2,0x0040        
 0000b0  1160000d   beq        $t3,$0,$103           
 0000b4  00000000   nop                              
                                                     # Line number: 542
 0000b8  8fac0058   lw         $t4,88($sp)           # BB:8,L:b8,H:c8,exitSet
 0000bc  00000000   nop                              
 0000c0  15800009   bne        $t4,$0,$104           
 0000c4  00000000   nop                              
                                                     # Line number: 544
 0000c8  240d0001   addiu      $t5,$0,1              # BB:9,L:c8,H:e8
 0000cc  afad0040   sw         $t5,64($sp)           # newGame
                                                     # Line number: 545
 0000d0  8fae0050   lw         $t6,80($sp)           # frameNow
 0000d4  00000000   nop                              
 0000d8  21c8003c   addi       $t0,$t6,60            
 0000dc  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
                                                     # Line number: 546
 0000e0  24090001   addiu      $t1,$0,1              
 0000e4  afa90058   sw         $t1,88($sp)           # exitSet
                                                     # Line number: 547
$104:                                                # BB:10,L:e8,H:e8
                                                     # Line number: 548
$103:                                                # BB:11,L:e8,H:f0
                                                     # Line number: 550
 0000e8  0c000000   jal        GsGetActiveBuff       # R_MIPS_26,GsGetActiveBuff
 0000ec  00000000   nop                              
 0000f0  afa2004c   sw         $v0,76($sp)           # BB:12,L:f0,H:114,activeBuffer
                                                     # Line number: 551
 0000f4  8faa004c   lw         $t2,76($sp)           # activeBuffer
 0000f8  240b12f0   addiu      $t3,$0,4848           
 0000fc  014b0018   mult       $t2,$t3               
 000100  00006012   mflo       $t4                   
 000104  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,GpuPacketArea
 000108  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,GpuPacketArea
 00010c  0c000000   jal        GsSetWorkBase         # R_MIPS_26,GsSetWorkBase
 000110  01ac2021   addu       $a0,$t5,$t4           
                                                     # Line number: 553
 000114  8fae004c   lw         $t6,76($sp)           # BB:13,L:114,H:13c,activeBuffer
 000118  24080014   addiu      $t0,$0,20             
 00011c  01c80018   mult       $t6,$t0               
 000120  00004812   mflo       $t1                   
 000124  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000128  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,WorldOrderingTable
 00012c  01493021   addu       $a2,$t2,$t1           
 000130  00002021   move       $a0,$0                
 000134  0c000000   jal        GsClearOt             # R_MIPS_26,GsClearOt
 000138  00002821   move       $a1,$0                
                                                     # Line number: 556
 00013c  8fab004c   lw         $t3,76($sp)           # BB:14,L:13c,H:168,activeBuffer
 000140  240c0014   addiu      $t4,$0,20             
 000144  016c0018   mult       $t3,$t4               
 000148  00006812   mflo       $t5                   
 00014c  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000150  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,WorldOrderingTable
 000154  01cd2821   addu       $a1,$t6,$t5           
 000158  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00015c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,BackgroundRectangle
 000160  0c000000   jal        GsSortBoxFill         # R_MIPS_26,GsSortBoxFill
 000164  00003021   move       $a2,$0                
                                                     # Line number: 559
 000168  0c000000   jal        DrawSync              # BB:15,L:168,H:170,R_MIPS_26,DrawSync
 00016c  00002021   move       $a0,$0                
                                                     # Line number: 560
 000170  0c000000   jal        VSync                 # BB:16,L:170,H:178,R_MIPS_26,VSync
 000174  00002021   move       $a0,$0                
                                                     # BB:17,L:178,H:180,Line number: 561
 000178  0c000000   jal        GsSwapDispBuff        # R_MIPS_26,GsSwapDispBuff
 00017c  afa20048   sw         $v0,72($sp)           # count
                                                     # Line number: 563
 000180  8fa8004c   lw         $t0,76($sp)           # BB:18,L:180,H:1ac,activeBuffer
 000184  24090014   addiu      $t1,$0,20             
 000188  01090018   mult       $t0,$t1               
 00018c  00005012   mflo       $t2                   
 000190  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000194  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,WorldOrderingTable
 000198  016a3821   addu       $a3,$t3,$t2           
 00019c  2404003c   addiu      $a0,$0,60             
 0001a0  24050078   addiu      $a1,$0,120            
 0001a4  0c000000   jal        GsSortClear           # R_MIPS_26,GsSortClear
 0001a8  24060078   addiu      $a2,$0,120            
                                                     # Line number: 565
 0001ac  8fac004c   lw         $t4,76($sp)           # BB:19,L:1ac,H:1cc,activeBuffer
 0001b0  240d0014   addiu      $t5,$0,20             
 0001b4  018d0018   mult       $t4,$t5               
 0001b8  00007012   mflo       $t6                   
 0001bc  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0001c0  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,WorldOrderingTable
 0001c4  0c000000   jal        GsDrawOt              # R_MIPS_26,GsDrawOt
 0001c8  010e2021   addu       $a0,$t0,$t6           
                                                     # Line number: 567
 0001cc  3c040000   lui        $a0,0x0000            # BB:20,L:1cc,H:1d8,R_MIPS_HI16,@91
 0001d0  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0001d4  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@91
                                                     # Line number: 568
 0001d8  3c040000   lui        $a0,0x0000            # BB:21,L:1d8,H:1f0,R_MIPS_HI16,@92
 0001dc  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@92
 0001e0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,CurrentScore
 0001e4  8c250010   lw         $a1,16($at)           # CurrentScore,R_MIPS_LO16,CurrentScore
 0001e8  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0001ec  00000000   nop                              
                                                     # Line number: 569
 0001f0  8fa90044   lw         $t1,68($sp)           # BB:22,L:1f0,H:200,position
 0001f4  2401ffff   addiu      $at,$0,-1             
 0001f8  11210007   beq        $t1,$at,$105          
 0001fc  00000000   nop                              
                                                     # Line number: 570
 000200  8faa0044   lw         $t2,68($sp)           # BB:23,L:200,H:218,position
 000204  00000000   nop                              
 000208  21450001   addi       $a1,$t2,1             
 00020c  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@93
 000210  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000214  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@93
$105:                                                # BB:24,L:218,H:224
                                                     # Line number: 571
 000218  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@94
 00021c  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000220  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@94
                                                     # Line number: 572
 000224  10000018   b          $106                  # BB:25,L:224,H:22c
 000228  afa0005c   sw         $0,92($sp)            # i
$107:                                                # BB:26,L:22c,H:278
                                                     # Line number: 574
 00022c  8fab005c   lw         $t3,92($sp)           # i
 000230  240c0014   addiu      $t4,$0,20             
 000234  016c0018   mult       $t3,$t4               
 000238  00006812   mflo       $t5                   
 00023c  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,HighScoreTable
 000240  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,HighScoreTable
 000244  01cd2821   addu       $a1,$t6,$t5           
 000248  8fa8005c   lw         $t0,92($sp)           # i
 00024c  24090014   addiu      $t1,$0,20             
 000250  01090018   mult       $t0,$t1               
 000254  00005012   mflo       $t2                   
 000258  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,HighScoreTable
 00025c  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,HighScoreTable
 000260  016a6021   addu       $t4,$t3,$t2           
 000264  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@95
 000268  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@95
 00026c  8d860010   lw         $a2,16($t4)           
 000270  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000274  00000000   nop                              
                                                     # Line number: 577
 000278  8fad005c   lw         $t5,92($sp)           # BB:27,L:278,H:288,i
 00027c  00000000   nop                              
 000280  21ae0001   addi       $t6,$t5,1             
 000284  afae005c   sw         $t6,92($sp)           # i
$106:                                                # BB:28,L:288,H:29c
 000288  8fa8005c   lw         $t0,92($sp)           # i
 00028c  00000000   nop                              
 000290  29010005   slti       $at,$t0,5             
 000294  1420ffe5   bne        $at,$0,$107           
 000298  00000000   nop                              
                                                     # Line number: 578
 00029c  3c040000   lui        $a0,0x0000            # BB:29,L:29c,H:2a8,R_MIPS_HI16,@96
 0002a0  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0002a4  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@96
                                                     # Line number: 579
 0002a8  3c040000   lui        $a0,0x0000            # BB:30,L:2a8,H:2b4,R_MIPS_HI16,@97
 0002ac  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0002b0  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@97
                                                     # Line number: 580
 0002b4  0c000000   jal        FntFlush              # BB:31,L:2b4,H:2bc,R_MIPS_26,FntFlush
 0002b8  2404ffff   addiu      $a0,$0,-1             
                                                     # Line number: 581
 0002bc  8fa90050   lw         $t1,80($sp)           # BB:32,L:2bc,H:2e0,frameNow
 0002c0  00000000   nop                              
 0002c4  212a0001   addi       $t2,$t1,1             
 0002c8  afaa0050   sw         $t2,80($sp)           # frameNow
                                                     # Line number: 582
 0002cc  8fab0054   lw         $t3,84($sp)           # whenToLeaveLoop
 0002d0  8fac0050   lw         $t4,80($sp)           # frameNow
 0002d4  00000000   nop                              
 0002d8  156cff56   bne        $t3,$t4,$100          
 0002dc  00000000   nop                              
                                                     # Line number: 586
 0002e0  8fa20040   lw         $v0,64($sp)           # BB:33,L:2e0,H:2f4,newGame
                                                     # Line number: 587
 0002e4  8fbf003c   lw         $ra,60($sp)           
 0002e8  00000000   nop                              
 0002ec  03e00008   jr         $ra                   
 0002f0  27bd0060   addiu      $sp,$sp,96            
                    .end       DealWithPlayersDeath  
 # *****************************************

                    .text                            # BB:0,L:0,H:34
                    .globl     DealWithPlayerFinishingTenLevels
                    .ent       DealWithPlayerFinishingTenLevels
DealWithPlayerFinishingTenLevels:                    # CS:756,ETS:0
                                                     # Line number: 594
 000000  27bdffa0   addiu      $sp,$sp,-96           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 599
 000008  afa00050   sw         $0,80($sp)            # frameNow
 00000c  2408ffff   addiu      $t0,$0,-1             
 000010  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
 000014  afa00058   sw         $0,88($sp)            # exitSet
                                                     # Line number: 602
 000018  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,CurrentScore
 00001c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,CurrentScore
 000020  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,HighScoreTable
 000024  20a50000   addi       $a1,$a1,0             # R_MIPS_LO16,HighScoreTable
 000028  24060005   addiu      $a2,$0,5              
 00002c  0c000000   jal        DealWithNewScore      # R_MIPS_26,DealWithNewScore
 000030  27a70044   addiu      $a3,$sp,68            
                                                     # Line number: 604
$100:                                                # BB:1,L:34,H:3c
                                                     # Line number: 606
 000034  0c000000   jal        PadRead               # R_MIPS_26,PadRead
 000038  00000000   nop                              
 00003c  3c010000   lui        $at,0x0000            # BB:2,L:3c,H:5c,R_MIPS_HI16,PadStatus
 000040  ac220000   sw         $v0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
                                                     # Line number: 608
 000044  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 000048  8c290000   lw         $t1,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 00004c  00000000   nop                              
 000050  312a0004   andi       $t2,$t1,0x0004        
 000054  11400012   beq        $t2,$0,$101           
 000058  00000000   nop                              
 00005c  3c010000   lui        $at,0x0000            # BB:3,L:5c,H:74,R_MIPS_HI16,PadStatus
 000060  8c2b0000   lw         $t3,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000064  00000000   nop                              
 000068  316c0008   andi       $t4,$t3,0x0008        
 00006c  1180000c   beq        $t4,$0,$101           
 000070  00000000   nop                              
                                                     # Line number: 610
 000074  8fad0058   lw         $t5,88($sp)           # BB:4,L:74,H:84,exitSet
 000078  00000000   nop                              
 00007c  15a00008   bne        $t5,$0,$102           
 000080  00000000   nop                              
                                                     # Line number: 612
 000084  afa00040   sw         $0,64($sp)            # BB:5,L:84,H:a0,newGame
                                                     # Line number: 613
 000088  8fae0050   lw         $t6,80($sp)           # frameNow
 00008c  00000000   nop                              
 000090  21c8003c   addi       $t0,$t6,60            
 000094  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
                                                     # Line number: 614
 000098  24090001   addiu      $t1,$0,1              
 00009c  afa90058   sw         $t1,88($sp)           # exitSet
                                                     # Line number: 615
$102:                                                # BB:6,L:a0,H:a0
                                                     # Line number: 616
$101:                                                # BB:7,L:a0,H:b8
                                                     # Line number: 617
 0000a0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 0000a4  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 0000a8  00000000   nop                              
 0000ac  314b0040   andi       $t3,$t2,0x0040        
 0000b0  1160000d   beq        $t3,$0,$103           
 0000b4  00000000   nop                              
                                                     # Line number: 619
 0000b8  8fac0058   lw         $t4,88($sp)           # BB:8,L:b8,H:c8,exitSet
 0000bc  00000000   nop                              
 0000c0  15800009   bne        $t4,$0,$104           
 0000c4  00000000   nop                              
                                                     # Line number: 621
 0000c8  240d0001   addiu      $t5,$0,1              # BB:9,L:c8,H:e8
 0000cc  afad0040   sw         $t5,64($sp)           # newGame
                                                     # Line number: 622
 0000d0  8fae0050   lw         $t6,80($sp)           # frameNow
 0000d4  00000000   nop                              
 0000d8  21c8003c   addi       $t0,$t6,60            
 0000dc  afa80054   sw         $t0,84($sp)           # whenToLeaveLoop
                                                     # Line number: 623
 0000e0  24090001   addiu      $t1,$0,1              
 0000e4  afa90058   sw         $t1,88($sp)           # exitSet
                                                     # Line number: 624
$104:                                                # BB:10,L:e8,H:e8
                                                     # Line number: 625
$103:                                                # BB:11,L:e8,H:f0
                                                     # Line number: 627
 0000e8  0c000000   jal        GsGetActiveBuff       # R_MIPS_26,GsGetActiveBuff
 0000ec  00000000   nop                              
 0000f0  afa2004c   sw         $v0,76($sp)           # BB:12,L:f0,H:114,activeBuffer
                                                     # Line number: 628
 0000f4  8faa004c   lw         $t2,76($sp)           # activeBuffer
 0000f8  240b12f0   addiu      $t3,$0,4848           
 0000fc  014b0018   mult       $t2,$t3               
 000100  00006012   mflo       $t4                   
 000104  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,GpuPacketArea
 000108  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,GpuPacketArea
 00010c  0c000000   jal        GsSetWorkBase         # R_MIPS_26,GsSetWorkBase
 000110  01ac2021   addu       $a0,$t5,$t4           
                                                     # Line number: 630
 000114  8fae004c   lw         $t6,76($sp)           # BB:13,L:114,H:13c,activeBuffer
 000118  24080014   addiu      $t0,$0,20             
 00011c  01c80018   mult       $t6,$t0               
 000120  00004812   mflo       $t1                   
 000124  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000128  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,WorldOrderingTable
 00012c  01493021   addu       $a2,$t2,$t1           
 000130  00002021   move       $a0,$0                
 000134  0c000000   jal        GsClearOt             # R_MIPS_26,GsClearOt
 000138  00002821   move       $a1,$0                
                                                     # Line number: 632
 00013c  8fab004c   lw         $t3,76($sp)           # BB:14,L:13c,H:168,activeBuffer
 000140  240c0014   addiu      $t4,$0,20             
 000144  016c0018   mult       $t3,$t4               
 000148  00006812   mflo       $t5                   
 00014c  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000150  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,WorldOrderingTable
 000154  01cd2821   addu       $a1,$t6,$t5           
 000158  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00015c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,BackgroundRectangle
 000160  0c000000   jal        GsSortBoxFill         # R_MIPS_26,GsSortBoxFill
 000164  00003021   move       $a2,$0                
                                                     # Line number: 635
 000168  0c000000   jal        DrawSync              # BB:15,L:168,H:170,R_MIPS_26,DrawSync
 00016c  00002021   move       $a0,$0                
                                                     # Line number: 636
 000170  0c000000   jal        VSync                 # BB:16,L:170,H:178,R_MIPS_26,VSync
 000174  00002021   move       $a0,$0                
                                                     # BB:17,L:178,H:180,Line number: 637
 000178  0c000000   jal        GsSwapDispBuff        # R_MIPS_26,GsSwapDispBuff
 00017c  afa20048   sw         $v0,72($sp)           # count
                                                     # Line number: 639
 000180  8fa8004c   lw         $t0,76($sp)           # BB:18,L:180,H:1ac,activeBuffer
 000184  24090014   addiu      $t1,$0,20             
 000188  01090018   mult       $t0,$t1               
 00018c  00005012   mflo       $t2                   
 000190  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000194  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,WorldOrderingTable
 000198  016a3821   addu       $a3,$t3,$t2           
 00019c  2404003c   addiu      $a0,$0,60             
 0001a0  24050078   addiu      $a1,$0,120            
 0001a4  0c000000   jal        GsSortClear           # R_MIPS_26,GsSortClear
 0001a8  24060078   addiu      $a2,$0,120            
                                                     # Line number: 641
 0001ac  8fac004c   lw         $t4,76($sp)           # BB:19,L:1ac,H:1cc,activeBuffer
 0001b0  240d0014   addiu      $t5,$0,20             
 0001b4  018d0018   mult       $t4,$t5               
 0001b8  00007012   mflo       $t6                   
 0001bc  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0001c0  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,WorldOrderingTable
 0001c4  0c000000   jal        GsDrawOt              # R_MIPS_26,GsDrawOt
 0001c8  010e2021   addu       $a0,$t0,$t6           
                                                     # Line number: 643
 0001cc  3c040000   lui        $a0,0x0000            # BB:20,L:1cc,H:1d8,R_MIPS_HI16,@122
 0001d0  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0001d4  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@122
                                                     # Line number: 644
 0001d8  3c040000   lui        $a0,0x0000            # BB:21,L:1d8,H:1f0,R_MIPS_HI16,@123
 0001dc  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@123
 0001e0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,CurrentScore
 0001e4  8c250010   lw         $a1,16($at)           # CurrentScore,R_MIPS_LO16,CurrentScore
 0001e8  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0001ec  00000000   nop                              
                                                     # Line number: 645
 0001f0  8fa90044   lw         $t1,68($sp)           # BB:22,L:1f0,H:200,position
 0001f4  2401ffff   addiu      $at,$0,-1             
 0001f8  11210007   beq        $t1,$at,$105          
 0001fc  00000000   nop                              
                                                     # Line number: 646
 000200  8faa0044   lw         $t2,68($sp)           # BB:23,L:200,H:218,position
 000204  00000000   nop                              
 000208  21450001   addi       $a1,$t2,1             
 00020c  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@124
 000210  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000214  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@124
$105:                                                # BB:24,L:218,H:224
                                                     # Line number: 647
 000218  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@125
 00021c  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000220  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@125
                                                     # Line number: 648
 000224  10000018   b          $106                  # BB:25,L:224,H:22c
 000228  afa0005c   sw         $0,92($sp)            # i
$107:                                                # BB:26,L:22c,H:278
                                                     # Line number: 650
 00022c  8fab005c   lw         $t3,92($sp)           # i
 000230  240c0014   addiu      $t4,$0,20             
 000234  016c0018   mult       $t3,$t4               
 000238  00006812   mflo       $t5                   
 00023c  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,HighScoreTable
 000240  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,HighScoreTable
 000244  01cd2821   addu       $a1,$t6,$t5           
 000248  8fa8005c   lw         $t0,92($sp)           # i
 00024c  24090014   addiu      $t1,$0,20             
 000250  01090018   mult       $t0,$t1               
 000254  00005012   mflo       $t2                   
 000258  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,HighScoreTable
 00025c  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,HighScoreTable
 000260  016a6021   addu       $t4,$t3,$t2           
 000264  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@126
 000268  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@126
 00026c  8d860010   lw         $a2,16($t4)           
 000270  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 000274  00000000   nop                              
                                                     # Line number: 653
 000278  8fad005c   lw         $t5,92($sp)           # BB:27,L:278,H:288,i
 00027c  00000000   nop                              
 000280  21ae0001   addi       $t6,$t5,1             
 000284  afae005c   sw         $t6,92($sp)           # i
$106:                                                # BB:28,L:288,H:29c
 000288  8fa8005c   lw         $t0,92($sp)           # i
 00028c  00000000   nop                              
 000290  29010005   slti       $at,$t0,5             
 000294  1420ffe5   bne        $at,$0,$107           
 000298  00000000   nop                              
                                                     # Line number: 654
 00029c  3c040000   lui        $a0,0x0000            # BB:29,L:29c,H:2a8,R_MIPS_HI16,@127
 0002a0  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0002a4  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@127
                                                     # Line number: 655
 0002a8  3c040000   lui        $a0,0x0000            # BB:30,L:2a8,H:2b4,R_MIPS_HI16,@128
 0002ac  0c000000   jal        FntPrint              # R_MIPS_26,FntPrint
 0002b0  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@128
                                                     # Line number: 656
 0002b4  0c000000   jal        FntFlush              # BB:31,L:2b4,H:2bc,R_MIPS_26,FntFlush
 0002b8  2404ffff   addiu      $a0,$0,-1             
                                                     # Line number: 657
 0002bc  8fa90050   lw         $t1,80($sp)           # BB:32,L:2bc,H:2e0,frameNow
 0002c0  00000000   nop                              
 0002c4  212a0001   addi       $t2,$t1,1             
 0002c8  afaa0050   sw         $t2,80($sp)           # frameNow
                                                     # Line number: 658
 0002cc  8fab0054   lw         $t3,84($sp)           # whenToLeaveLoop
 0002d0  8fac0050   lw         $t4,80($sp)           # frameNow
 0002d4  00000000   nop                              
 0002d8  156cff56   bne        $t3,$t4,$100          
 0002dc  00000000   nop                              
                                                     # Line number: 662
 0002e0  8fa20040   lw         $v0,64($sp)           # BB:33,L:2e0,H:2f4,newGame
                                                     # Line number: 663
 0002e4  8fbf003c   lw         $ra,60($sp)           
 0002e8  00000000   nop                              
 0002ec  03e00008   jr         $ra                   
 0002f0  27bd0060   addiu      $sp,$sp,96            
                    .end       DealWithPlayerFinishingTenLevels
 # *****************************************

                    .text                            # BB:0,L:0,H:24
                    .globl     ClearScores           
                    .ent       ClearScores           
ClearScores:                                         # CS:296,ETS:0
                                                     # Line number: 669
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 672
 000008  3c010000   lui        $at,0x0000            # R_MIPS_HI16,CurrentScore
 00000c  ac200010   sw         $0,16($at)            # CurrentScore,R_MIPS_LO16,CurrentScore
                                                     # Line number: 673
 000010  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,CurrentScore
 000014  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,CurrentScore
 000018  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,@140
 00001c  0c000000   jal        strcpy                # R_MIPS_26,strcpy
 000020  20a50000   addi       $a1,$a1,0             # R_MIPS_LO16,@140
                                                     # Line number: 675
 000024  10000037   b          $100                  # BB:1,L:24,H:2c
 000028  afa00048   sw         $0,72($sp)            # i
$101:                                                # BB:2,L:2c,H:70
                                                     # Line number: 677
 00002c  8fa80048   lw         $t0,72($sp)           # i
 000030  00000000   nop                              
 000034  2109fffb   addi       $t1,$t0,-5            
 000038  00095022   neg        $t2,$t1               
 00003c  240b03e8   addiu      $t3,$0,1000           
 000040  014b0018   mult       $t2,$t3               
 000044  00006012   mflo       $t4                   
 000048  8fad0048   lw         $t5,72($sp)           # i
 00004c  240e0014   addiu      $t6,$0,20             
 000050  01ae0018   mult       $t5,$t6               
 000054  00004012   mflo       $t0                   
 000058  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,HighScoreTable
 00005c  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,HighScoreTable
 000060  01285021   addu       $t2,$t1,$t0           
 000064  ad4c0010   sw         $t4,16($t2)           
                                                     # Line number: 678
 000068  10000015   b          $102                  
 00006c  afa0004c   sw         $0,76($sp)            # j
$103:                                                # BB:3,L:70,H:c0
                                                     # Line number: 680
 000070  8fab0048   lw         $t3,72($sp)           # i
 000074  00000000   nop                              
 000078  216c0041   addi       $t4,$t3,65            
 00007c  000c6e00   sll        $t5,$t4,24            
 000080  000d7603   sra        $t6,$t5,24            
 000084  8fa80048   lw         $t0,72($sp)           # i
 000088  24090014   addiu      $t1,$0,20             
 00008c  01090018   mult       $t0,$t1               
 000090  00005012   mflo       $t2                   
 000094  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,HighScoreTable
 000098  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,HighScoreTable
 00009c  016a6021   addu       $t4,$t3,$t2           
 0000a0  8fad004c   lw         $t5,76($sp)           # j
 0000a4  00000000   nop                              
 0000a8  01ac4021   addu       $t0,$t5,$t4           
 0000ac  a10e0000   sb         $t6,0($t0)            
                                                     # Line number: 681
 0000b0  8fa9004c   lw         $t1,76($sp)           # j
 0000b4  00000000   nop                              
 0000b8  212a0001   addi       $t2,$t1,1             
 0000bc  afaa004c   sw         $t2,76($sp)           # j
$102:                                                # BB:4,L:c0,H:d4
 0000c0  8fab004c   lw         $t3,76($sp)           # j
 0000c4  00000000   nop                              
 0000c8  29610008   slti       $at,$t3,8             
 0000cc  1420ffe8   bne        $at,$0,$103           
 0000d0  00000000   nop                              
                                                     # Line number: 682
 0000d4  8fac0048   lw         $t4,72($sp)           # BB:5,L:d4,H:104,i
 0000d8  240d0014   addiu      $t5,$0,20             
 0000dc  018d0018   mult       $t4,$t5               
 0000e0  00007012   mflo       $t6                   
 0000e4  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,HighScoreTable
 0000e8  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,HighScoreTable
 0000ec  010e4821   addu       $t1,$t0,$t6           
 0000f0  a1200008   sb         $0,8($t1)             
                                                     # Line number: 683
 0000f4  8faa0048   lw         $t2,72($sp)           # i
 0000f8  00000000   nop                              
 0000fc  214b0001   addi       $t3,$t2,1             
 000100  afab0048   sw         $t3,72($sp)           # i
$100:                                                # BB:6,L:104,H:118
 000104  8fac0048   lw         $t4,72($sp)           # i
 000108  00000000   nop                              
 00010c  29810005   slti       $at,$t4,5             
 000110  1420ffc6   bne        $at,$0,$101           
 000114  00000000   nop                              
                                                     # Line number: 684
 000118  8fbf003c   lw         $ra,60($sp)           # BB:7,L:118,H:128
 00011c  00000000   nop                              
 000120  03e00008   jr         $ra                   
 000124  27bd0050   addiu      $sp,$sp,80            
                    .end       ClearScores           
 # *****************************************

                    .text                            # BB:0,L:0,H:38
                    .globl     DealWithNewScore      
                    .ent       DealWithNewScore      
DealWithNewScore:                                    # CS:536,ETS:0
                                                     # Line number: 693
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
 000008  afa40050   sw         $a0,80($sp)           # last
 00000c  afa50054   sw         $a1,84($sp)           # table
 000010  afa60058   sw         $a2,88($sp)           # tableLength
 000014  afa7005c   sw         $a3,92($sp)           # outputPosition
                                                     # Line number: 694
 000018  8fa80058   lw         $t0,88($sp)           # tableLength
 00001c  00000000   nop                              
 000020  afa80048   sw         $t0,72($sp)           # highestPosition
                                                     # Line number: 697
 000024  8fa90058   lw         $t1,88($sp)           # tableLength
 000028  00000000   nop                              
 00002c  212affff   addi       $t2,$t1,-1            
 000030  10000017   b          $100                  
 000034  afaa004c   sw         $t2,76($sp)           # i
$101:                                                # BB:1,L:38,H:74
                                                     # Line number: 699
 000038  8fab004c   lw         $t3,76($sp)           # i
 00003c  240c0014   addiu      $t4,$0,20             
 000040  016c0018   mult       $t3,$t4               
 000044  00006812   mflo       $t5                   
 000048  8fae0054   lw         $t6,84($sp)           # table
 00004c  00000000   nop                              
 000050  01cd4021   addu       $t0,$t6,$t5           
 000054  8d090010   lw         $t1,16($t0)           
 000058  8faa0050   lw         $t2,80($sp)           # last
 00005c  00000000   nop                              
 000060  8d4b0010   lw         $t3,16($t2)           
 000064  00000000   nop                              
 000068  012b082b   sltu       $at,$t1,$t3           
 00006c  10200004   beq        $at,$0,$102           
 000070  00000000   nop                              
                                                     # Line number: 701
 000074  8fac004c   lw         $t4,76($sp)           # BB:2,L:74,H:80,i
 000078  00000000   nop                              
 00007c  afac0048   sw         $t4,72($sp)           # highestPosition
                                                     # Line number: 702
$102:                                                # BB:3,L:80,H:90
                                                     # Line number: 703
 000080  8fad004c   lw         $t5,76($sp)           # i
 000084  00000000   nop                              
 000088  21aeffff   addi       $t6,$t5,-1            
 00008c  afae004c   sw         $t6,76($sp)           # i
$100:                                                # BB:4,L:90,H:a4
 000090  8fa8004c   lw         $t0,76($sp)           # i
 000094  00000000   nop                              
 000098  0100082a   slt        $at,$t0,$0            
 00009c  1020ffe6   beq        $at,$0,$101           
 0000a0  00000000   nop                              
                                                     # Line number: 705
 0000a4  8fa90048   lw         $t1,72($sp)           # BB:5,L:a4,H:b8,highestPosition
 0000a8  8faa0058   lw         $t2,88($sp)           # tableLength
 0000ac  00000000   nop                              
 0000b0  152a0005   bne        $t1,$t2,$103          
 0000b4  00000000   nop                              
                                                     # Line number: 707
 0000b8  240bffff   addiu      $t3,$0,-1             # BB:6,L:b8,H:c8
 0000bc  8fac005c   lw         $t4,92($sp)           # outputPosition
                                                     # Line number: 708
 0000c0  10000051   b          $104                  
 0000c4  ad8b0000   sw         $t3,0($t4)            
$103:                                                # BB:7,L:c8,H:ec
                                                     # Line number: 711
 0000c8  8fad0048   lw         $t5,72($sp)           # highestPosition
 0000cc  8fae005c   lw         $t6,92($sp)           # outputPosition
 0000d0  00000000   nop                              
 0000d4  adcd0000   sw         $t5,0($t6)            
                                                     # Line number: 713
 0000d8  8fa8005c   lw         $t0,92($sp)           # outputPosition
 0000dc  00000000   nop                              
 0000e0  8d090000   lw         $t1,0($t0)            
 0000e4  10000027   b          $105                  
 0000e8  afa9004c   sw         $t1,76($sp)           # i
$106:                                                # BB:8,L:ec,H:12c
                                                     # Line number: 715
 0000ec  8faa004c   lw         $t2,76($sp)           # i
 0000f0  00000000   nop                              
 0000f4  214b0001   addi       $t3,$t2,1             
 0000f8  240c0014   addiu      $t4,$0,20             
 0000fc  016c0018   mult       $t3,$t4               
 000100  00006812   mflo       $t5                   
 000104  8fae0054   lw         $t6,84($sp)           # table
 000108  00000000   nop                              
 00010c  01cd2021   addu       $a0,$t6,$t5           
 000110  8fa8004c   lw         $t0,76($sp)           # i
 000114  24090014   addiu      $t1,$0,20             
 000118  01090018   mult       $t0,$t1               
 00011c  00005012   mflo       $t2                   
 000120  8fab0054   lw         $t3,84($sp)           # table
 000124  0c000000   jal        strcpy                # R_MIPS_26,strcpy
 000128  016a2821   addu       $a1,$t3,$t2           
                                                     # Line number: 716
 00012c  8fac004c   lw         $t4,76($sp)           # BB:9,L:12c,H:184,i
 000130  240d0014   addiu      $t5,$0,20             
 000134  018d0018   mult       $t4,$t5               
 000138  00007012   mflo       $t6                   
 00013c  8fa80054   lw         $t0,84($sp)           # table
 000140  00000000   nop                              
 000144  010e4821   addu       $t1,$t0,$t6           
 000148  8d2a0010   lw         $t2,16($t1)           
 00014c  8fab004c   lw         $t3,76($sp)           # i
 000150  00000000   nop                              
 000154  216c0001   addi       $t4,$t3,1             
 000158  240d0014   addiu      $t5,$0,20             
 00015c  018d0018   mult       $t4,$t5               
 000160  00007012   mflo       $t6                   
 000164  8fa80054   lw         $t0,84($sp)           # table
 000168  00000000   nop                              
 00016c  010e4821   addu       $t1,$t0,$t6           
 000170  ad2a0010   sw         $t2,16($t1)           
                                                     # Line number: 717
 000174  8faa004c   lw         $t2,76($sp)           # i
 000178  00000000   nop                              
 00017c  214b0001   addi       $t3,$t2,1             
 000180  afab004c   sw         $t3,76($sp)           # i
$105:                                                # BB:10,L:184,H:1a4
 000184  8fac0058   lw         $t4,88($sp)           # tableLength
 000188  00000000   nop                              
 00018c  218dfffe   addi       $t5,$t4,-2            
 000190  8fae004c   lw         $t6,76($sp)           # i
 000194  00000000   nop                              
 000198  01ae082a   slt        $at,$t5,$t6           
 00019c  1020ffd3   beq        $at,$0,$106           
 0001a0  00000000   nop                              
                                                     # Line number: 718
 0001a4  8fa8005c   lw         $t0,92($sp)           # BB:11,L:1a4,H:1d4,outputPosition
 0001a8  00000000   nop                              
 0001ac  8d090000   lw         $t1,0($t0)            
 0001b0  240a0014   addiu      $t2,$0,20             
 0001b4  012a0018   mult       $t1,$t2               
 0001b8  00005812   mflo       $t3                   
 0001bc  8fac0054   lw         $t4,84($sp)           # table
 0001c0  00000000   nop                              
 0001c4  018b2021   addu       $a0,$t4,$t3           
 0001c8  8fa50050   lw         $a1,80($sp)           # last
 0001cc  0c000000   jal        strcpy                # R_MIPS_26,strcpy
 0001d0  00000000   nop                              
                                                     # Line number: 719
 0001d4  8fad0050   lw         $t5,80($sp)           # BB:12,L:1d4,H:208,last
 0001d8  00000000   nop                              
 0001dc  8dae0010   lw         $t6,16($t5)           
 0001e0  8fa8005c   lw         $t0,92($sp)           # outputPosition
 0001e4  00000000   nop                              
 0001e8  8d090000   lw         $t1,0($t0)            
 0001ec  240a0014   addiu      $t2,$0,20             
 0001f0  012a0018   mult       $t1,$t2               
 0001f4  00005812   mflo       $t3                   
 0001f8  8fac0054   lw         $t4,84($sp)           # table
 0001fc  00000000   nop                              
 000200  018b6821   addu       $t5,$t4,$t3           
 000204  adae0010   sw         $t6,16($t5)           
                                                     # Line number: 720
$104:                                                # BB:13,L:208,H:218
                                                     # Line number: 721
 000208  8fbf003c   lw         $ra,60($sp)           
 00020c  00000000   nop                              
 000210  03e00008   jr         $ra                   
 000214  27bd0050   addiu      $sp,$sp,80            
                    .end       DealWithNewScore      
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     InitialiseAll         
                    .ent       InitialiseAll         
InitialiseAll:                                       # CS:368,ETS:0
                                                     # Line number: 728
 000000  27bdffa0   addiu      $sp,$sp,-96           
 000004  afbf004c   sw         $ra,76($sp)           
                                                     # Line number: 732
 000008  0c000000   jal        DatafileSearch        # R_MIPS_26,DatafileSearch
 00000c  00000000   nop                              
                                                     # Line number: 733
 000010  0c000000   jal        DatafileRead          # BB:1,L:10,H:18,R_MIPS_26,DatafileRead
 000014  00000000   nop                              
                                                     # Line number: 736
 000018  afa00010   sw         $0,16($sp)            # BB:2,L:18,H:30
 00001c  24040140   addiu      $a0,$0,320            
 000020  240500f0   addiu      $a1,$0,240            
 000024  24060004   addiu      $a2,$0,4              
 000028  0c000000   jal        GsInitGraph           # R_MIPS_26,GsInitGraph
 00002c  00003821   move       $a3,$0                
                                                     # Line number: 737
 000030  00002021   move       $a0,$0                # BB:3,L:30,H:44
 000034  00002821   move       $a1,$0                
 000038  00003021   move       $a2,$0                
 00003c  0c000000   jal        GsDefDispBuff         # R_MIPS_26,GsDefDispBuff
 000040  240700f0   addiu      $a3,$0,240            
                                                     # Line number: 740
 000044  3c010000   lui        $at,0x0000            # BB:4,L:44,H:60,R_MIPS_HI16,GsDISPENV
 000048  a4200008   sh         $0,8($at)             # GsDISPENV,R_MIPS_LO16,GsDISPENV
                                                     # Line number: 741
 00004c  24080010   addiu      $t0,$0,16             
 000050  3c010000   lui        $at,0x0000            # R_MIPS_HI16,GsDISPENV
 000054  a428000a   sh         $t0,10($at)           # GsDISPENV,R_MIPS_LO16,GsDISPENV
                                                     # Line number: 745
 000058  1000001c   b          $100                  
 00005c  afa0005c   sw         $0,92($sp)            # i
$101:                                                # BB:5,L:60,H:cc
                                                     # Line number: 747
 000060  24090001   addiu      $t1,$0,1              
 000064  8faa005c   lw         $t2,92($sp)           # i
 000068  240b0014   addiu      $t3,$0,20             
 00006c  014b0018   mult       $t2,$t3               
 000070  00006012   mflo       $t4                   
 000074  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,WorldOrderingTable
 000078  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,WorldOrderingTable
 00007c  01ac7021   addu       $t6,$t5,$t4           
 000080  adc90000   sw         $t1,0($t6)            
                                                     # Line number: 748
 000084  8fa8005c   lw         $t0,92($sp)           # i
 000088  00000000   nop                              
 00008c  00084900   sll        $t1,$t0,4             
 000090  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,OrderingTableTags
 000094  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,OrderingTableTags
 000098  01495821   addu       $t3,$t2,$t1           
 00009c  8fac005c   lw         $t4,92($sp)           # i
 0000a0  240d0014   addiu      $t5,$0,20             
 0000a4  018d0018   mult       $t4,$t5               
 0000a8  00007012   mflo       $t6                   
 0000ac  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,WorldOrderingTable
 0000b0  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,WorldOrderingTable
 0000b4  010e4821   addu       $t1,$t0,$t6           
 0000b8  ad2b0004   sw         $t3,4($t1)            
                                                     # Line number: 749
 0000bc  8faa005c   lw         $t2,92($sp)           # i
 0000c0  00000000   nop                              
 0000c4  214b0001   addi       $t3,$t2,1             
 0000c8  afab005c   sw         $t3,92($sp)           # i
$100:                                                # BB:6,L:cc,H:e0
 0000cc  8fac005c   lw         $t4,92($sp)           # i
 0000d0  00000000   nop                              
 0000d4  29810002   slti       $at,$t4,2             
 0000d8  1420ffe1   bne        $at,$0,$101           
 0000dc  00000000   nop                              
                                                     # Line number: 751
 0000e0  0c000000   jal        PadInit               # BB:7,L:e0,H:e8,R_MIPS_26,PadInit
 0000e4  00000000   nop                              
                                                     # Line number: 753
 0000e8  240403c0   addiu      $a0,$0,960            # BB:8,L:e8,H:f4
 0000ec  0c000000   jal        FntLoad               # R_MIPS_26,FntLoad
 0000f0  24050100   addiu      $a1,$0,256            
                                                     # Line number: 754
 0000f4  afa00010   sw         $0,16($sp)            # BB:9,L:f4,H:114
 0000f8  240d0200   addiu      $t5,$0,512            
 0000fc  afad0014   sw         $t5,20($sp)           
 000100  24040010   addiu      $a0,$0,16             
 000104  24050010   addiu      $a1,$0,16             
 000108  24060100   addiu      $a2,$0,256            
 00010c  0c000000   jal        FntOpen               # R_MIPS_26,FntOpen
 000110  240700c8   addiu      $a3,$0,200            
                                                     # Line number: 756
 000114  0c000000   jal        InitialiseObjects     # BB:10,L:114,H:11c,R_MIPS_26,InitialiseObjects
 000118  00000000   nop                              
                                                     # Line number: 759
 00011c  0c000000   jal        InitialiseTexturesAndSprites ,L:11c,H:124,R_MIPS_26,InitialiseTexturesAndSprites
 000120  00000000   nop                              
                                                     # Line number: 761
 000124  0c000000   jal        ClearBackground       # BB:12,L:124,H:12c,R_MIPS_26,ClearBackground
 000128  00000000   nop                              
                                                     # Line number: 763
 00012c  240e0001   addiu      $t6,$0,1              # BB:13,L:12c,H:148
 000130  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Level
 000134  ac2e0000   sw         $t6,0($at)            # Level,R_MIPS_LO16,Level
                                                     # Line number: 764
 000138  24080005   addiu      $t0,$0,5              
 00013c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
                                                     # Line number: 766
 000140  0c000000   jal        SetUpLevel1           # R_MIPS_26,SetUpLevel1
 000144  ac280000   sw         $t0,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
                                                     # Line number: 768
 000148  0c000000   jal        ClearScores           # BB:14,L:148,H:150,R_MIPS_26,ClearScores
 00014c  00000000   nop                              
                                                     # Line number: 770
 000150  0c000000   jal        InitSound             # BB:15,L:150,H:158,R_MIPS_26,InitSound
 000154  00000000   nop                              
                                                     # Line number: 771
 000158  0c000000   jal        PlaySound             # BB:16,L:158,H:160,R_MIPS_26,PlaySound
 00015c  00000000   nop                              
                                                     # Line number: 772
 000160  8fbf004c   lw         $ra,76($sp)           # BB:17,L:160,H:170
 000164  00000000   nop                              
 000168  03e00008   jr         $ra                   
 00016c  27bd0060   addiu      $sp,$sp,96            
                    .end       InitialiseAll         
 # *****************************************

                    .text                            # BB:0,L:0,H:4c
                    .globl     InitialiseObjects     
                    .ent       InitialiseObjects     
InitialiseObjects:                                   # CS:584,ETS:0
                                                     # Line number: 778
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 782
 000008  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 00000c  ac200000   sw         $0,0($at)             # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 783
 000010  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000014  ac200004   sw         $0,4($at)             # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 784
 000018  24080028   addiu      $t0,$0,40             
 00001c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000020  ac280008   sw         $t0,8($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 785
 000024  24090008   addiu      $t1,$0,8              
 000028  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 00002c  ac29000c   sw         $t1,12($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 786
 000030  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000034  ac200010   sw         $0,16($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 787
 000038  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 00003c  ac200014   sw         $0,20($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 788
 000040  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,PlayersSprite
 000044  0c000000   jal        InitGsSprite          # R_MIPS_26,InitGsSprite
 000048  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,PlayersSprite
                                                     # Line number: 789
 00004c  240a0003   addiu      $t2,$0,3              # BB:1,L:4c,H:9c
 000050  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayerSpeed
 000054  ac2a0000   sw         $t2,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
                                                     # Line number: 792
 000058  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00005c  ac200000   sw         $0,0($at)             # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 793
 000060  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000064  ac200004   sw         $0,4($at)             # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 794
 000068  240b0007   addiu      $t3,$0,7              
 00006c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000070  ac2b0008   sw         $t3,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 795
 000074  240c0007   addiu      $t4,$0,7              
 000078  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00007c  ac2c000c   sw         $t4,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 796
 000080  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000084  ac200010   sw         $0,16($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 797
 000088  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00008c  ac200014   sw         $0,20($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 798
 000090  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,BallsSprite
 000094  0c000000   jal        InitGsSprite          # R_MIPS_26,InitGsSprite
 000098  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,BallsSprite
                                                     # Line number: 801
 00009c  10000061   b          $100                  # BB:2,L:9c,H:a4
 0000a0  afa0004c   sw         $0,76($sp)            # i
$101:                                                # BB:3,L:a4,H:214
                                                     # Line number: 803
 0000a4  8fad004c   lw         $t5,76($sp)           # i
 0000a8  8fae004c   lw         $t6,76($sp)           # i
 0000ac  2408004c   addiu      $t0,$0,76             
 0000b0  01c80018   mult       $t6,$t0               
 0000b4  00004812   mflo       $t1                   
 0000b8  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 0000bc  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 0000c0  01495821   addu       $t3,$t2,$t1           
 0000c4  ad6d0000   sw         $t5,0($t3)            
                                                     # Line number: 804
 0000c8  8fac004c   lw         $t4,76($sp)           # i
 0000cc  240d004c   addiu      $t5,$0,76             
 0000d0  018d0018   mult       $t4,$t5               
 0000d4  00007012   mflo       $t6                   
 0000d8  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0000dc  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 0000e0  010e4821   addu       $t1,$t0,$t6           
 0000e4  ad200004   sw         $0,4($t1)             
                                                     # Line number: 805
 0000e8  240a7ffe   addiu      $t2,$0,32766          
 0000ec  8fab004c   lw         $t3,76($sp)           # i
 0000f0  240c004c   addiu      $t4,$0,76             
 0000f4  016c0018   mult       $t3,$t4               
 0000f8  00006812   mflo       $t5                   
 0000fc  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 000100  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 000104  01cd4021   addu       $t0,$t6,$t5           
 000108  ad0a0008   sw         $t2,8($t0)            
                                                     # Line number: 806
 00010c  24097ffe   addiu      $t1,$0,32766          
 000110  8faa004c   lw         $t2,76($sp)           # i
 000114  240b004c   addiu      $t3,$0,76             
 000118  014b0018   mult       $t2,$t3               
 00011c  00006012   mflo       $t4                   
 000120  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000124  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000128  01ac7021   addu       $t6,$t5,$t4           
 00012c  adc9000c   sw         $t1,12($t6)           
                                                     # Line number: 807
 000130  8fa8004c   lw         $t0,76($sp)           # i
 000134  2409004c   addiu      $t1,$0,76             
 000138  01090018   mult       $t0,$t1               
 00013c  00005012   mflo       $t2                   
 000140  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000144  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000148  016a6021   addu       $t4,$t3,$t2           
 00014c  ad800034   sw         $0,52($t4)            
                                                     # Line number: 808
 000150  8fad004c   lw         $t5,76($sp)           # i
 000154  240e004c   addiu      $t6,$0,76             
 000158  01ae0018   mult       $t5,$t6               
 00015c  00004012   mflo       $t0                   
 000160  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000164  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000168  01285021   addu       $t2,$t1,$t0           
 00016c  ad400038   sw         $0,56($t2)            
                                                     # Line number: 809
 000170  8fab004c   lw         $t3,76($sp)           # i
 000174  240c004c   addiu      $t4,$0,76             
 000178  016c0018   mult       $t3,$t4               
 00017c  00006812   mflo       $t5                   
 000180  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 000184  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 000188  01cd4021   addu       $t0,$t6,$t5           
 00018c  ad00003c   sw         $0,60($t0)            
                                                     # Line number: 810
 000190  8fa9004c   lw         $t1,76($sp)           # i
 000194  240a004c   addiu      $t2,$0,76             
 000198  012a0018   mult       $t1,$t2               
 00019c  00005812   mflo       $t3                   
 0001a0  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 0001a4  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 0001a8  018b6821   addu       $t5,$t4,$t3           
 0001ac  ada00040   sw         $0,64($t5)            
                                                     # Line number: 811
 0001b0  8fae004c   lw         $t6,76($sp)           # i
 0001b4  2408004c   addiu      $t0,$0,76             
 0001b8  01c80018   mult       $t6,$t0               
 0001bc  00004812   mflo       $t1                   
 0001c0  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 0001c4  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 0001c8  01495821   addu       $t3,$t2,$t1           
 0001cc  ad600044   sw         $0,68($t3)            
                                                     # Line number: 812
 0001d0  8fac004c   lw         $t4,76($sp)           # i
 0001d4  240d004c   addiu      $t5,$0,76             
 0001d8  018d0018   mult       $t4,$t5               
 0001dc  00007012   mflo       $t6                   
 0001e0  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0001e4  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 0001e8  010e4821   addu       $t1,$t0,$t6           
 0001ec  ad200048   sw         $0,72($t1)            
                                                     # Line number: 813
 0001f0  8faa004c   lw         $t2,76($sp)           # i
 0001f4  240b004c   addiu      $t3,$0,76             
 0001f8  014b0018   mult       $t2,$t3               
 0001fc  00006012   mflo       $t4                   
 000200  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000204  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000208  01ac7021   addu       $t6,$t5,$t4           
 00020c  0c000000   jal        InitGsSprite          # R_MIPS_26,InitGsSprite
 000210  25c40010   addiu      $a0,$t6,16            
                                                     # Line number: 814
 000214  8fa8004c   lw         $t0,76($sp)           # BB:4,L:214,H:224,i
 000218  00000000   nop                              
 00021c  21090001   addi       $t1,$t0,1             
 000220  afa9004c   sw         $t1,76($sp)           # i
$100:                                                # BB:5,L:224,H:238
 000224  8faa004c   lw         $t2,76($sp)           # i
 000228  00000000   nop                              
 00022c  294100c8   slti       $at,$t2,200           
 000230  1420ff9c   bne        $at,$0,$101           
 000234  00000000   nop                              
                                                     # Line number: 815
 000238  8fbf003c   lw         $ra,60($sp)           # BB:6,L:238,H:248
 00023c  00000000   nop                              
 000240  03e00008   jr         $ra                   
 000244  27bd0050   addiu      $sp,$sp,80            
                    .end       InitialiseObjects     
 # *****************************************

                    .text                            # BB:0,L:0,H:60
                    .globl     InitGsSprite          
                    .ent       InitGsSprite          
InitGsSprite:                                        # CS:96,ETS:0
                                                     # Line number: 820
                                                     # Line number: 822
 000000  ac800000   sw         $0,0($a0)             
                                                     # Line number: 823
 000004  a4800004   sh         $0,4($a0)             
                                                     # Line number: 824
 000008  a4800006   sh         $0,6($a0)             
                                                     # Line number: 825
 00000c  a4800008   sh         $0,8($a0)             
                                                     # Line number: 826
 000010  a480000a   sh         $0,10($a0)            
                                                     # Line number: 827
 000014  a480000c   sh         $0,12($a0)            
                                                     # Line number: 828
 000018  a080000e   sb         $0,14($a0)            
                                                     # Line number: 829
 00001c  a080000f   sb         $0,15($a0)            
                                                     # Line number: 830
 000020  a4800010   sh         $0,16($a0)            
                                                     # Line number: 831
 000024  a4800012   sh         $0,18($a0)            
                                                     # Line number: 832
 000028  24080080   addiu      $t0,$0,128            
 00002c  a0880014   sb         $t0,20($a0)           
                                                     # Line number: 833
 000030  24090080   addiu      $t1,$0,128            
 000034  a0890015   sb         $t1,21($a0)           
                                                     # Line number: 834
 000038  240a0080   addiu      $t2,$0,128            
 00003c  a08a0016   sb         $t2,22($a0)           
                                                     # Line number: 835
 000040  a4800018   sh         $0,24($a0)            
                                                     # Line number: 836
 000044  a480001a   sh         $0,26($a0)            
                                                     # Line number: 837
 000048  240b1000   addiu      $t3,$0,4096           
 00004c  a48b001c   sh         $t3,28($a0)           
                                                     # Line number: 838
 000050  240c1000   addiu      $t4,$0,4096           
 000054  a48c001e   sh         $t4,30($a0)           
                                                     # Line number: 839
                                                     # Line number: 840
 000058  03e00008   jr         $ra                   
 00005c  ac800020   sw         $0,32($a0)            
                    .end       InitGsSprite          
 # *****************************************

                    .text                            # BB:0,L:0,H:14
                    .globl     CreateNewBrick        
                    .ent       CreateNewBrick        
CreateNewBrick:                                      # CS:456,ETS:0
                                                     # Line number: 847
 000000  27bdfff0   addiu      $sp,$sp,-16           
                                                     # Line number: 849
 000004  2408ffff   addiu      $t0,$0,-1             
 000008  afa80008   sw         $t0,8($sp)            # found
                                                     # Line number: 852
 00000c  10000015   b          $100                  
 000010  afa0000c   sw         $0,12($sp)            # i
$101:                                                # BB:1,L:14,H:40
                                                     # Line number: 854
 000014  8fa9000c   lw         $t1,12($sp)           # i
 000018  240a004c   addiu      $t2,$0,76             
 00001c  012a0018   mult       $t1,$t2               
 000020  00005812   mflo       $t3                   
 000024  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 000028  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 00002c  018b6821   addu       $t5,$t4,$t3           
 000030  8dae0004   lw         $t6,4($t5)            
 000034  00000000   nop                              
 000038  15c00006   bne        $t6,$0,$102           
 00003c  00000000   nop                              
                                                     # Line number: 856
 000040  24080001   addiu      $t0,$0,1              # BB:2,L:40,H:54
 000044  afa80008   sw         $t0,8($sp)            # found
                                                     # Line number: 857
 000048  8fa9000c   lw         $t1,12($sp)           # i
                                                     # Line number: 858
 00004c  1000000a   b          $103                  
 000050  afa90004   sw         $t1,4($sp)            # id
$102:                                                # BB:3,L:54,H:64
                                                     # Line number: 860
 000054  8faa000c   lw         $t2,12($sp)           # i
 000058  00000000   nop                              
 00005c  214b0001   addi       $t3,$t2,1             
 000060  afab000c   sw         $t3,12($sp)           # i
$100:                                                # BB:4,L:64,H:78
 000064  8fac000c   lw         $t4,12($sp)           # i
 000068  00000000   nop                              
 00006c  298100c8   slti       $at,$t4,200           
 000070  1420ffe8   bne        $at,$0,$101           
 000074  00000000   nop                              
$103:                                                # BB:5,L:78,H:88
                                                     # Line number: 862
 000078  8fad0008   lw         $t5,8($sp)            # found
 00007c  2401ffff   addiu      $at,$0,-1             
 000080  15a10003   bne        $t5,$at,$104          
 000084  00000000   nop                              
                                                     # Line number: 863
 000088  1000004d   b          $105                  # BB:6,L:88,H:90
 00008c  2402ffff   addiu      $v0,$0,-1             
$104:                                                # BB:7,L:90,H:100
                                                     # Line number: 866
 000090  240e0001   addiu      $t6,$0,1              
 000094  8fa80004   lw         $t0,4($sp)            # id
 000098  2409004c   addiu      $t1,$0,76             
 00009c  01090018   mult       $t0,$t1               
 0000a0  00005012   mflo       $t2                   
 0000a4  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 0000a8  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 0000ac  016a6021   addu       $t4,$t3,$t2           
 0000b0  ad8e0004   sw         $t6,4($t4)            
                                                     # Line number: 867
 0000b4  8fad0004   lw         $t5,4($sp)            # id
 0000b8  240e004c   addiu      $t6,$0,76             
 0000bc  01ae0018   mult       $t5,$t6               
 0000c0  00004012   mflo       $t0                   
 0000c4  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 0000c8  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 0000cc  01285021   addu       $t2,$t1,$t0           
 0000d0  ad440008   sw         $a0,8($t2)            
                                                     # Line number: 868
 0000d4  8fab0004   lw         $t3,4($sp)            # id
 0000d8  240c004c   addiu      $t4,$0,76             
 0000dc  016c0018   mult       $t3,$t4               
 0000e0  00006812   mflo       $t5                   
 0000e4  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 0000e8  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 0000ec  01cd4021   addu       $t0,$t6,$t5           
 0000f0  ad05000c   sw         $a1,12($t0)           
                                                     # Line number: 870
 0000f4  24010064   addiu      $at,$0,100            
 0000f8  14810014   bne        $a0,$at,$106          
 0000fc  00000000   nop                              
                                                     # Line number: 872
 000100  24090010   addiu      $t1,$0,16             # BB:8,L:100,H:14c
 000104  8faa0004   lw         $t2,4($sp)            # id
 000108  240b004c   addiu      $t3,$0,76             
 00010c  014b0018   mult       $t2,$t3               
 000110  00006012   mflo       $t4                   
 000114  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000118  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 00011c  01ac7021   addu       $t6,$t5,$t4           
 000120  adc9003c   sw         $t1,60($t6)           
                                                     # Line number: 873
 000124  24080008   addiu      $t0,$0,8              
 000128  8fa90004   lw         $t1,4($sp)            # id
 00012c  240a004c   addiu      $t2,$0,76             
 000130  012a0018   mult       $t1,$t2               
 000134  00005812   mflo       $t3                   
 000138  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 00013c  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 000140  018b6821   addu       $t5,$t4,$t3           
                                                     # Line number: 874
 000144  1000001c   b          $107                  
 000148  ada80040   sw         $t0,64($t5)           
$106:                                                # BB:9,L:14c,H:1b8
                                                     # Line number: 877
 00014c  240e0008   addiu      $t6,$0,8              
 000150  8fa80004   lw         $t0,4($sp)            # id
 000154  2409004c   addiu      $t1,$0,76             
 000158  01090018   mult       $t0,$t1               
 00015c  00005012   mflo       $t2                   
 000160  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000164  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000168  016a6021   addu       $t4,$t3,$t2           
 00016c  ad8e003c   sw         $t6,60($t4)           
                                                     # Line number: 878
 000170  240d0008   addiu      $t5,$0,8              
 000174  8fae0004   lw         $t6,4($sp)            # id
 000178  2408004c   addiu      $t0,$0,76             
 00017c  01c80018   mult       $t6,$t0               
 000180  00004812   mflo       $t1                   
 000184  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000188  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 00018c  01495821   addu       $t3,$t2,$t1           
 000190  ad6d0040   sw         $t5,64($t3)           
                                                     # Line number: 879
 000194  240c0200   addiu      $t4,$0,512            
 000198  8fad0004   lw         $t5,4($sp)            # id
 00019c  240e004c   addiu      $t6,$0,76             
 0001a0  01ae0018   mult       $t5,$t6               
 0001a4  00004012   mflo       $t0                   
 0001a8  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 0001ac  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 0001b0  01285021   addu       $t2,$t1,$t0           
 0001b4  a54c002c   sh         $t4,44($t2)           
                                                     # Line number: 880
$107:                                                # BB:10,L:1b8,H:1c0
                                                     # Line number: 882
 0001b8  8fa20004   lw         $v0,4($sp)            # id
 0001bc  00000000   nop                              
$105:                                                # BB:11,L:1c0,H:1c8
                                                     # Line number: 884
 0001c0  03e00008   jr         $ra                   
 0001c4  27bd0010   addiu      $sp,$sp,16            
                    .end       CreateNewBrick        
 # *****************************************

                    .text                            # BB:0,L:0,H:60
                    .globl     DestroyBrick          
                    .ent       DestroyBrick          
DestroyBrick:                                        # CS:96,ETS:0
                                                     # Line number: 888
                                                     # Line number: 889
 000000  2408004c   addiu      $t0,$0,76             
 000004  00880018   mult       $a0,$t0               
 000008  00004812   mflo       $t1                   
 00000c  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000010  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 000014  01495821   addu       $t3,$t2,$t1           
 000018  ad600004   sw         $0,4($t3)             
                                                     # Line number: 890
 00001c  240c7ffe   addiu      $t4,$0,32766          
 000020  240d004c   addiu      $t5,$0,76             
 000024  008d0018   mult       $a0,$t5               
 000028  00007012   mflo       $t6                   
 00002c  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 000030  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 000034  010e4821   addu       $t1,$t0,$t6           
 000038  ad2c0008   sw         $t4,8($t1)            
                                                     # Line number: 891
 00003c  240a7ffe   addiu      $t2,$0,32766          
 000040  240b004c   addiu      $t3,$0,76             
 000044  008b0018   mult       $a0,$t3               
 000048  00006012   mflo       $t4                   
 00004c  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000050  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000054  01ac7021   addu       $t6,$t5,$t4           
                                                     # Line number: 892
 000058  03e00008   jr         $ra                   
 00005c  adca000c   sw         $t2,12($t6)           
                    .end       DestroyBrick          
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     CountNumberOfBricks   
                    .ent       CountNumberOfBricks   
CountNumberOfBricks:                                 # CS:124,ETS:0
                                                     # Line number: 897
 000000  27bdfff0   addiu      $sp,$sp,-16           
                                                     # Line number: 898
 000004  afa00008   sw         $0,8($sp)             # count
                                                     # Line number: 901
 000008  10000014   b          $100                  
 00000c  afa0000c   sw         $0,12($sp)            # i
$101:                                                # BB:1,L:10,H:3c
                                                     # Line number: 903
 000010  8fa8000c   lw         $t0,12($sp)           # i
 000014  2409004c   addiu      $t1,$0,76             
 000018  01090018   mult       $t0,$t1               
 00001c  00005012   mflo       $t2                   
 000020  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000024  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000028  016a6021   addu       $t4,$t3,$t2           
 00002c  8d8d0004   lw         $t5,4($t4)            
 000030  24010001   addiu      $at,$0,1              
 000034  15a10005   bne        $t5,$at,$102          
 000038  00000000   nop                              
                                                     # Line number: 905
 00003c  8fae0008   lw         $t6,8($sp)            # BB:2,L:3c,H:4c,count
 000040  00000000   nop                              
 000044  21c80001   addi       $t0,$t6,1             
 000048  afa80008   sw         $t0,8($sp)            # count
                                                     # Line number: 906
$102:                                                # BB:3,L:4c,H:5c
                                                     # Line number: 907
 00004c  8fa9000c   lw         $t1,12($sp)           # i
 000050  00000000   nop                              
 000054  212a0001   addi       $t2,$t1,1             
 000058  afaa000c   sw         $t2,12($sp)           # i
$100:                                                # BB:4,L:5c,H:70
 00005c  8fab000c   lw         $t3,12($sp)           # i
 000060  00000000   nop                              
 000064  296100c8   slti       $at,$t3,200           
 000068  1420ffe9   bne        $at,$0,$101           
 00006c  00000000   nop                              
                                                     # Line number: 909
 000070  8fa20008   lw         $v0,8($sp)            # BB:5,L:70,H:7c,count
                                                     # Line number: 910
 000074  03e00008   jr         $ra                   
 000078  27bd0010   addiu      $sp,$sp,16            
                    .end       CountNumberOfBricks   
 # *****************************************

                    .text                            # BB:0,L:0,H:c
                    .globl     DestroyAllBricks      
                    .ent       DestroyAllBricks      
DestroyAllBricks:                                    # CS:432,ETS:0
                                                     # Line number: 916
 000000  27bdfff0   addiu      $sp,$sp,-16           
                                                     # Line number: 919
 000004  10000063   b          $100                  
 000008  afa0000c   sw         $0,12($sp)            # i
$101:                                                # BB:1,L:c,H:38
                                                     # Line number: 921
 00000c  8fa8000c   lw         $t0,12($sp)           # i
 000010  2409004c   addiu      $t1,$0,76             
 000014  01090018   mult       $t0,$t1               
 000018  00005012   mflo       $t2                   
 00001c  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000020  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000024  016a6021   addu       $t4,$t3,$t2           
 000028  8d8d0004   lw         $t5,4($t4)            
 00002c  24010001   addiu      $at,$0,1              
 000030  15a10054   bne        $t5,$at,$102          
 000034  00000000   nop                              
                                                     # Line number: 923
 000038  8fae000c   lw         $t6,12($sp)           # BB:2,L:38,H:184,i
 00003c  8fa8000c   lw         $t0,12($sp)           # i
 000040  2409004c   addiu      $t1,$0,76             
 000044  01090018   mult       $t0,$t1               
 000048  00005012   mflo       $t2                   
 00004c  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000050  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000054  016a6021   addu       $t4,$t3,$t2           
 000058  ad8e0000   sw         $t6,0($t4)            
                                                     # Line number: 924
 00005c  8fad000c   lw         $t5,12($sp)           # i
 000060  240e004c   addiu      $t6,$0,76             
 000064  01ae0018   mult       $t5,$t6               
 000068  00004012   mflo       $t0                   
 00006c  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000070  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000074  01285021   addu       $t2,$t1,$t0           
 000078  ad400004   sw         $0,4($t2)             
                                                     # Line number: 925
 00007c  240b7ffe   addiu      $t3,$0,32766          
 000080  8fac000c   lw         $t4,12($sp)           # i
 000084  240d004c   addiu      $t5,$0,76             
 000088  018d0018   mult       $t4,$t5               
 00008c  00007012   mflo       $t6                   
 000090  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 000094  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 000098  010e4821   addu       $t1,$t0,$t6           
 00009c  ad2b0008   sw         $t3,8($t1)            
                                                     # Line number: 926
 0000a0  240a7ffe   addiu      $t2,$0,32766          
 0000a4  8fab000c   lw         $t3,12($sp)           # i
 0000a8  240c004c   addiu      $t4,$0,76             
 0000ac  016c0018   mult       $t3,$t4               
 0000b0  00006812   mflo       $t5                   
 0000b4  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 0000b8  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 0000bc  01cd4021   addu       $t0,$t6,$t5           
 0000c0  ad0a000c   sw         $t2,12($t0)           
                                                     # Line number: 927
 0000c4  8fa9000c   lw         $t1,12($sp)           # i
 0000c8  240a004c   addiu      $t2,$0,76             
 0000cc  012a0018   mult       $t1,$t2               
 0000d0  00005812   mflo       $t3                   
 0000d4  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 0000d8  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 0000dc  018b6821   addu       $t5,$t4,$t3           
 0000e0  ada00034   sw         $0,52($t5)            
                                                     # Line number: 928
 0000e4  8fae000c   lw         $t6,12($sp)           # i
 0000e8  2408004c   addiu      $t0,$0,76             
 0000ec  01c80018   mult       $t6,$t0               
 0000f0  00004812   mflo       $t1                   
 0000f4  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 0000f8  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 0000fc  01495821   addu       $t3,$t2,$t1           
 000100  ad600038   sw         $0,56($t3)            
                                                     # Line number: 929
 000104  8fac000c   lw         $t4,12($sp)           # i
 000108  240d004c   addiu      $t5,$0,76             
 00010c  018d0018   mult       $t4,$t5               
 000110  00007012   mflo       $t6                   
 000114  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 000118  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 00011c  010e4821   addu       $t1,$t0,$t6           
 000120  ad20003c   sw         $0,60($t1)            
                                                     # Line number: 930
 000124  8faa000c   lw         $t2,12($sp)           # i
 000128  240b004c   addiu      $t3,$0,76             
 00012c  014b0018   mult       $t2,$t3               
 000130  00006012   mflo       $t4                   
 000134  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000138  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 00013c  01ac7021   addu       $t6,$t5,$t4           
 000140  adc00040   sw         $0,64($t6)            
                                                     # Line number: 931
 000144  8fa8000c   lw         $t0,12($sp)           # i
 000148  2409004c   addiu      $t1,$0,76             
 00014c  01090018   mult       $t0,$t1               
 000150  00005012   mflo       $t2                   
 000154  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000158  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 00015c  016a6021   addu       $t4,$t3,$t2           
 000160  ad800044   sw         $0,68($t4)            
                                                     # Line number: 932
 000164  8fad000c   lw         $t5,12($sp)           # i
 000168  240e004c   addiu      $t6,$0,76             
 00016c  01ae0018   mult       $t5,$t6               
 000170  00004012   mflo       $t0                   
 000174  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000178  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 00017c  01285021   addu       $t2,$t1,$t0           
 000180  ad400048   sw         $0,72($t2)            
                                                     # Line number: 933
$102:                                                # BB:3,L:184,H:194
                                                     # Line number: 934
 000184  8fab000c   lw         $t3,12($sp)           # i
 000188  00000000   nop                              
 00018c  216c0001   addi       $t4,$t3,1             
 000190  afac000c   sw         $t4,12($sp)           # i
$100:                                                # BB:4,L:194,H:1a8
 000194  8fad000c   lw         $t5,12($sp)           # i
 000198  00000000   nop                              
 00019c  29a100c8   slti       $at,$t5,200           
 0001a0  1420ff9a   bne        $at,$0,$101           
 0001a4  00000000   nop                              
                                                     # Line number: 935
 0001a8  03e00008   jr         $ra                   # BB:5,L:1a8,H:1b0
 0001ac  27bd0010   addiu      $sp,$sp,16            
                    .end       DestroyAllBricks      
 # *****************************************

                    .text                            # BB:0,L:0,H:14
                    .globl     RestartTheBall        
                    .ent       RestartTheBall        
RestartTheBall:                                      # CS:112,ETS:0
                                                     # Line number: 943
                                                     # Line number: 945
 000000  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000004  8c290008   lw         $t1,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000008  00000000   nop                              
 00000c  05210003   bgez       $t1,$100              
 000010  0009c843   sra        $t9,$t1,1             
 000014  252a0001   addiu      $t2,$t1,1             # BB:1,L:14,H:1c
 000018  000ac843   sra        $t9,$t2,1             
$100:                                                # BB:2,L:1c,H:40
 00001c  232bff60   addi       $t3,$t9,-160          
 000020  000b6022   neg        $t4,$t3               
 000024  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000028  ac2c0000   sw         $t4,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 946
 00002c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000030  8c2e000c   lw         $t6,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000034  00000000   nop                              
 000038  05c10003   bgez       $t6,$101              
 00003c  000ec843   sra        $t9,$t6,1             
 000040  25c80001   addiu      $t0,$t6,1             # BB:3,L:40,H:48
 000044  0008c843   sra        $t9,$t0,1             
$101:                                                # BB:4,L:48,H:70
 000048  2329ff88   addi       $t1,$t9,-120          
 00004c  00095022   neg        $t2,$t1               
 000050  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000054  ac2a0004   sw         $t2,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 947
 000058  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00005c  ac200010   sw         $0,16($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 948
 000060  240b0001   addiu      $t3,$0,1              
 000064  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
                                                     # Line number: 949
 000068  03e00008   jr         $ra                   
 00006c  ac2b0014   sw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                    .end       RestartTheBall        
 # *****************************************

                    .text                            # BB:0,L:0,H:14
                    .globl     RestartPlayersBat     
                    .ent       RestartPlayersBat     
RestartPlayersBat:                                   # CS:92,ETS:0
                                                     # Line number: 953
                                                     # Line number: 955
 000000  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000004  8c290008   lw         $t1,8($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000008  00000000   nop                              
 00000c  05210003   bgez       $t1,$100              
 000010  0009c843   sra        $t9,$t1,1             
 000014  252a0001   addiu      $t2,$t1,1             # BB:1,L:14,H:1c
 000018  000ac843   sra        $t9,$t2,1             
$100:                                                # BB:2,L:1c,H:5c
 00001c  232bff60   addi       $t3,$t9,-160          
 000020  000b6022   neg        $t4,$t3               
 000024  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000028  ac2c0000   sw         $t4,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 956
 00002c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000030  8c2d000c   lw         $t5,12($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000034  00000000   nop                              
 000038  21aeff3c   addi       $t6,$t5,-196          
 00003c  000e4022   neg        $t0,$t6               
 000040  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000044  ac280004   sw         $t0,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 957
 000048  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 00004c  ac200010   sw         $0,16($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 958
 000050  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
                                                     # Line number: 959
 000054  03e00008   jr         $ra                   
 000058  ac200014   sw         $0,20($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                    .end       RestartPlayersBat     
 # *****************************************

                    .text                            # BB:0,L:0,H:4c
                    .globl     InitialiseTexturesAndSprites
                    .ent       InitialiseTexturesAndSprites
InitialiseTexturesAndSprites:                        # CS:1176,ETS:0
                                                     # Line number: 985
 000000  27bdff60   addiu      $sp,$sp,-160          
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 990
 000008  3c080100   lui        $t0,0x0100            
 00000c  afa8007c   sw         $t0,124($sp)          # eightBitTextureAttributeMask
                                                     # Line number: 991
 000010  3c090200   lui        $t1,0x0200            
 000014  afa90080   sw         $t1,128($sp)          # sixteenBitTextureAttributeMask
                                                     # Line number: 992
 000018  3c0a2000   lui        $t2,0x2000            
 00001c  afaa0084   sw         $t2,132($sp)          # mask1
                                                     # Line number: 993
 000020  3c0b4000   lui        $t3,0x4000            
 000024  afab0088   sw         $t3,136($sp)          # mask2
                                                     # Line number: 997
 000028  3c0c8016   lui        $t4,0x8016            
 00002c  afac0040   sw         $t4,64($sp)           # timPointer
                                                     # Line number: 998
 000030  8fad0040   lw         $t5,64($sp)           # timPointer
 000034  00000000   nop                              
 000038  25ae0004   addiu      $t6,$t5,4             
 00003c  afae0040   sw         $t6,64($sp)           # timPointer
                                                     # Line number: 1000
 000040  8fa40040   lw         $a0,64($sp)           # timPointer
 000044  0c000000   jal        GsGetTimInfo          # R_MIPS_26,GsGetTimInfo
 000048  27a50044   addiu      $a1,$sp,68            
                                                     # Line number: 1003
 00004c  24080140   addiu      $t0,$0,320            # BB:1,L:4c,H:80
 000050  a7a80092   sh         $t0,146($sp)          # frameBufferPosition
                                                     # Line number: 1004
 000054  a7a00094   sh         $0,148($sp)           # frameBufferPosition
                                                     # Line number: 1005
 000058  97a9004c   lhu        $t1,76($sp)           # image1
 00005c  00000000   nop                              
 000060  a7a90096   sh         $t1,150($sp)          # frameBufferPosition
                                                     # Line number: 1006
 000064  97aa004e   lhu        $t2,78($sp)           # image1
 000068  00000000   nop                              
 00006c  a7aa0098   sh         $t2,152($sp)          # frameBufferPosition
                                                     # Line number: 1008
 000070  27a40092   addiu      $a0,$sp,146           
 000074  8fa50050   lw         $a1,80($sp)           # image1
 000078  0c000000   jal        LoadImage             # R_MIPS_26,LoadImage
 00007c  00000000   nop                              
                                                     # Line number: 1009
 000080  0c000000   jal        DrawSync              # BB:2,L:80,H:88,R_MIPS_26,DrawSync
 000084  00002021   move       $a0,$0                
                                                     # Line number: 1012
 000088  87ab0054   lh         $t3,84($sp)           # BB:3,L:88,H:c8,image1
 00008c  00000000   nop                              
 000090  a7ab0092   sh         $t3,146($sp)          # frameBufferPosition
                                                     # Line number: 1013
 000094  87ac0056   lh         $t4,86($sp)           # image1
 000098  00000000   nop                              
 00009c  a7ac0094   sh         $t4,148($sp)          # frameBufferPosition
                                                     # Line number: 1014
 0000a0  97ad0058   lhu        $t5,88($sp)           # image1
 0000a4  00000000   nop                              
 0000a8  a7ad0096   sh         $t5,150($sp)          # frameBufferPosition
                                                     # Line number: 1015
 0000ac  97ae005a   lhu        $t6,90($sp)           # image1
 0000b0  00000000   nop                              
 0000b4  a7ae0098   sh         $t6,152($sp)          # frameBufferPosition
                                                     # Line number: 1017
 0000b8  27a40092   addiu      $a0,$sp,146           
 0000bc  8fa5005c   lw         $a1,92($sp)           # image1
 0000c0  0c000000   jal        LoadImage             # R_MIPS_26,LoadImage
 0000c4  00000000   nop                              
                                                     # Line number: 1018
 0000c8  0c000000   jal        DrawSync              # BB:4,L:c8,H:d0,R_MIPS_26,DrawSync
 0000cc  00002021   move       $a0,$0                
                                                     # Line number: 1021
 0000d0  24040001   addiu      $a0,$0,1              # BB:5,L:d0,H:e4
 0000d4  00002821   move       $a1,$0                
 0000d8  24060140   addiu      $a2,$0,320            
 0000dc  0c000000   jal        GetTPage              # R_MIPS_26,GetTPage
 0000e0  00003821   move       $a3,$0                
 0000e4  a7a2009a   sh         $v0,154($sp)          # BB:6,L:e4,H:1a8,texturePageID1
                                                     # Line number: 1023
 0000e8  97a8004c   lhu        $t0,76($sp)           # image1
 0000ec  00000000   nop                              
 0000f0  00084840   sll        $t1,$t0,1             
 0000f4  312affff   andi       $t2,$t1,0xffff        
 0000f8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 0000fc  a42a0008   sh         $t2,8($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1024
 000100  97ab004e   lhu        $t3,78($sp)           # image1
 000104  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000108  a42b000a   sh         $t3,10($at)           # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1025
 00010c  97ac009a   lhu        $t4,154($sp)          # texturePageID1
 000110  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000114  a42c000c   sh         $t4,12($at)           # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1026
 000118  87ad0054   lh         $t5,84($sp)           # image1
 00011c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000120  a42d0010   sh         $t5,16($at)           # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1027
 000124  87ae0056   lh         $t6,86($sp)           # image1
 000128  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 00012c  a42e0012   sh         $t6,18($at)           # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1028
 000130  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000134  8c280000   lw         $t0,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
 000138  8fa9007c   lw         $t1,124($sp)          # eightBitTextureAttributeMask
 00013c  00000000   nop                              
 000140  01095025   or         $t2,$t0,$t1           
 000144  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000148  ac2a0000   sw         $t2,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1029
 00014c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000150  8c2b0000   lw         $t3,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
 000154  8fac0084   lw         $t4,132($sp)          # mask1
 000158  00000000   nop                              
 00015c  016c6825   or         $t5,$t3,$t4           
 000160  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000164  ac2d0000   sw         $t5,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1030
 000168  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 00016c  8c2e0000   lw         $t6,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
 000170  8fa80088   lw         $t0,136($sp)          # mask2
 000174  00000000   nop                              
 000178  01c84825   or         $t1,$t6,$t0           
 00017c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000180  ac290000   sw         $t1,0($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1035
 000184  3c0a8017   lui        $t2,0x8017            
 000188  afaa0040   sw         $t2,64($sp)           # timPointer
                                                     # Line number: 1036
 00018c  8fab0040   lw         $t3,64($sp)           # timPointer
 000190  00000000   nop                              
 000194  256c0004   addiu      $t4,$t3,4             
 000198  afac0040   sw         $t4,64($sp)           # timPointer
                                                     # Line number: 1038
 00019c  8fa40040   lw         $a0,64($sp)           # timPointer
 0001a0  0c000000   jal        GsGetTimInfo          # R_MIPS_26,GsGetTimInfo
 0001a4  27a50060   addiu      $a1,$sp,96            
                                                     # Line number: 1040
 0001a8  240d0280   addiu      $t5,$0,640            # BB:7,L:1a8,H:1dc
 0001ac  a7ad0092   sh         $t5,146($sp)          # frameBufferPosition
                                                     # Line number: 1041
 0001b0  a7a00094   sh         $0,148($sp)           # frameBufferPosition
                                                     # Line number: 1042
 0001b4  97ae0068   lhu        $t6,104($sp)          # image2
 0001b8  00000000   nop                              
 0001bc  a7ae0096   sh         $t6,150($sp)          # frameBufferPosition
                                                     # Line number: 1043
 0001c0  97a8006a   lhu        $t0,106($sp)          # image2
 0001c4  00000000   nop                              
 0001c8  a7a80098   sh         $t0,152($sp)          # frameBufferPosition
                                                     # Line number: 1045
 0001cc  27a40092   addiu      $a0,$sp,146           
 0001d0  8fa5006c   lw         $a1,108($sp)          # image2
 0001d4  0c000000   jal        LoadImage             # R_MIPS_26,LoadImage
 0001d8  00000000   nop                              
                                                     # Line number: 1046
 0001dc  0c000000   jal        DrawSync              # BB:8,L:1dc,H:1e4,R_MIPS_26,DrawSync
 0001e0  00002021   move       $a0,$0                
                                                     # Line number: 1049
 0001e4  24040002   addiu      $a0,$0,2              # BB:9,L:1e4,H:1f8
 0001e8  00002821   move       $a1,$0                
 0001ec  24060280   addiu      $a2,$0,640            
 0001f0  0c000000   jal        GetTPage              # R_MIPS_26,GetTPage
 0001f4  00003821   move       $a3,$0                
 0001f8  a7a2009c   sh         $v0,156($sp)          # BB:10,L:1f8,H:204,texturePageID2
                                                     # Line number: 1051
 0001fc  10000068   b          $100                  
 000200  afa0008c   sw         $0,140($sp)           # i
$101:                                                # BB:11,L:204,H:3a0
                                                     # Line number: 1053
 000204  97a90068   lhu        $t1,104($sp)          # image2
 000208  8faa008c   lw         $t2,140($sp)          # i
 00020c  240b004c   addiu      $t3,$0,76             
 000210  014b0018   mult       $t2,$t3               
 000214  00006012   mflo       $t4                   
 000218  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 00021c  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000220  01ac7021   addu       $t6,$t5,$t4           
 000224  a5c90018   sh         $t1,24($t6)           
                                                     # Line number: 1054
 000228  97a8006a   lhu        $t0,106($sp)          # image2
 00022c  8fa9008c   lw         $t1,140($sp)          # i
 000230  240a004c   addiu      $t2,$0,76             
 000234  012a0018   mult       $t1,$t2               
 000238  00005812   mflo       $t3                   
 00023c  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 000240  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 000244  018b6821   addu       $t5,$t4,$t3           
 000248  a5a8001a   sh         $t0,26($t5)           
                                                     # Line number: 1055
 00024c  97ae009c   lhu        $t6,156($sp)          # texturePageID2
 000250  8fa8008c   lw         $t0,140($sp)          # i
 000254  2409004c   addiu      $t1,$0,76             
 000258  01090018   mult       $t0,$t1               
 00025c  00005012   mflo       $t2                   
 000260  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000264  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000268  016a6021   addu       $t4,$t3,$t2           
 00026c  a58e001c   sh         $t6,28($t4)           
                                                     # Line number: 1056
 000270  87ad0070   lh         $t5,112($sp)          # image2
 000274  8fae008c   lw         $t6,140($sp)          # i
 000278  2408004c   addiu      $t0,$0,76             
 00027c  01c80018   mult       $t6,$t0               
 000280  00004812   mflo       $t1                   
 000284  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000288  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 00028c  01495821   addu       $t3,$t2,$t1           
 000290  a56d0020   sh         $t5,32($t3)           
                                                     # Line number: 1057
 000294  87ac0072   lh         $t4,114($sp)          # image2
 000298  8fad008c   lw         $t5,140($sp)          # i
 00029c  240e004c   addiu      $t6,$0,76             
 0002a0  01ae0018   mult       $t5,$t6               
 0002a4  00004012   mflo       $t0                   
 0002a8  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 0002ac  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 0002b0  01285021   addu       $t2,$t1,$t0           
 0002b4  a54c0022   sh         $t4,34($t2)           
                                                     # Line number: 1058
 0002b8  8fab008c   lw         $t3,140($sp)          # i
 0002bc  240c004c   addiu      $t4,$0,76             
 0002c0  016c0018   mult       $t3,$t4               
 0002c4  00006812   mflo       $t5                   
 0002c8  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 0002cc  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 0002d0  01cd4021   addu       $t0,$t6,$t5           
 0002d4  8d090010   lw         $t1,16($t0)           
 0002d8  8faa0080   lw         $t2,128($sp)          # sixteenBitTextureAttributeMask
 0002dc  00000000   nop                              
 0002e0  012a5825   or         $t3,$t1,$t2           
 0002e4  ad0b0010   sw         $t3,16($t0)           
                                                     # Line number: 1059
 0002e8  8fac008c   lw         $t4,140($sp)          # i
 0002ec  240d004c   addiu      $t5,$0,76             
 0002f0  018d0018   mult       $t4,$t5               
 0002f4  00007012   mflo       $t6                   
 0002f8  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0002fc  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 000300  010e4821   addu       $t1,$t0,$t6           
 000304  8d2a0010   lw         $t2,16($t1)           
 000308  8fab0084   lw         $t3,132($sp)          # mask1
 00030c  00000000   nop                              
 000310  014b6025   or         $t4,$t2,$t3           
 000314  ad2c0010   sw         $t4,16($t1)           
                                                     # Line number: 1060
 000318  8fad008c   lw         $t5,140($sp)          # i
 00031c  240e004c   addiu      $t6,$0,76             
 000320  01ae0018   mult       $t5,$t6               
 000324  00004012   mflo       $t0                   
 000328  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 00032c  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000330  01285021   addu       $t2,$t1,$t0           
 000334  8d4b0010   lw         $t3,16($t2)           
 000338  8fac0088   lw         $t4,136($sp)          # mask2
 00033c  00000000   nop                              
 000340  016c6825   or         $t5,$t3,$t4           
 000344  ad4d0010   sw         $t5,16($t2)           
                                                     # Line number: 1061
 000348  240e0400   addiu      $t6,$0,1024           
 00034c  8fa8008c   lw         $t0,140($sp)          # i
 000350  2409004c   addiu      $t1,$0,76             
 000354  01090018   mult       $t0,$t1               
 000358  00005012   mflo       $t2                   
 00035c  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000360  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000364  016a6021   addu       $t4,$t3,$t2           
 000368  a58e002c   sh         $t6,44($t4)           
                                                     # Line number: 1062
 00036c  240d0400   addiu      $t5,$0,1024           
 000370  8fae008c   lw         $t6,140($sp)          # i
 000374  2408004c   addiu      $t0,$0,76             
 000378  01c80018   mult       $t6,$t0               
 00037c  00004812   mflo       $t1                   
 000380  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000384  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 000388  01495821   addu       $t3,$t2,$t1           
 00038c  a56d002e   sh         $t5,46($t3)           
                                                     # Line number: 1063
 000390  8fac008c   lw         $t4,140($sp)          # i
 000394  00000000   nop                              
 000398  218d0001   addi       $t5,$t4,1             
 00039c  afad008c   sw         $t5,140($sp)          # i
$100:                                                # BB:12,L:3a0,H:3b4
 0003a0  8fae008c   lw         $t6,140($sp)          # i
 0003a4  00000000   nop                              
 0003a8  29c100c8   slti       $at,$t6,200           
 0003ac  1420ff95   bne        $at,$0,$101           
 0003b0  00000000   nop                              
                                                     # Line number: 1067
 0003b4  24080140   addiu      $t0,$0,320            # BB:13,L:3b4,H:3e4
 0003b8  a7a80092   sh         $t0,146($sp)          # frameBufferPosition
                                                     # Line number: 1068
 0003bc  24090100   addiu      $t1,$0,256            
 0003c0  a7a90094   sh         $t1,148($sp)          # frameBufferPosition
                                                     # Line number: 1069
 0003c4  240a0004   addiu      $t2,$0,4              
 0003c8  a7aa0096   sh         $t2,150($sp)          # frameBufferPosition
                                                     # Line number: 1070
 0003cc  240b0010   addiu      $t3,$0,16             
 0003d0  a7ab0098   sh         $t3,152($sp)          # frameBufferPosition
                                                     # Line number: 1072
 0003d4  27a40092   addiu      $a0,$sp,146           
 0003d8  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,ball16x16
 0003dc  0c000000   jal        LoadImage             # R_MIPS_26,LoadImage
 0003e0  20a50000   addi       $a1,$a1,0             # R_MIPS_LO16,ball16x16
                                                     # Line number: 1073
 0003e4  0c000000   jal        DrawSync              # BB:14,L:3e4,H:3ec,R_MIPS_26,DrawSync
 0003e8  00002021   move       $a0,$0                
                                                     # Line number: 1075
 0003ec  a7a00092   sh         $0,146($sp)           # BB:15,L:3ec,H:418,frameBufferPosition
                                                     # Line number: 1076
 0003f0  240c01e1   addiu      $t4,$0,481            
 0003f4  a7ac0094   sh         $t4,148($sp)          # frameBufferPosition
                                                     # Line number: 1077
 0003f8  240d0010   addiu      $t5,$0,16             
 0003fc  a7ad0096   sh         $t5,150($sp)          # frameBufferPosition
                                                     # Line number: 1078
 000400  240e0001   addiu      $t6,$0,1              
 000404  a7ae0098   sh         $t6,152($sp)          # frameBufferPosition
                                                     # Line number: 1080
 000408  27a40092   addiu      $a0,$sp,146           
 00040c  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,ballcolor
 000410  0c000000   jal        LoadImage             # R_MIPS_26,LoadImage
 000414  20a50080   addi       $a1,$a1,128           # R_MIPS_LO16,ballcolor
                                                     # Line number: 1081
 000418  0c000000   jal        DrawSync              # BB:16,L:418,H:420,R_MIPS_26,DrawSync
 00041c  00002021   move       $a0,$0                
                                                     # Line number: 1084
 000420  00002021   move       $a0,$0                # BB:17,L:420,H:434
 000424  00002821   move       $a1,$0                
 000428  24060140   addiu      $a2,$0,320            
 00042c  0c000000   jal        GetTPage              # R_MIPS_26,GetTPage
 000430  24070100   addiu      $a3,$0,256            
 000434  a7a2009e   sh         $v0,158($sp)          # BB:18,L:434,H:498,texturePageID3
                                                     # Line number: 1086
 000438  24080010   addiu      $t0,$0,16             
 00043c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000440  a4280008   sh         $t0,8($at)            # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1087
 000444  24090010   addiu      $t1,$0,16             
 000448  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 00044c  a429000a   sh         $t1,10($at)           # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1088
 000450  97aa009e   lhu        $t2,158($sp)          # texturePageID3
 000454  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000458  a42a000c   sh         $t2,12($at)           # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1089
 00045c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000460  a4200010   sh         $0,16($at)            # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1090
 000464  240b01e1   addiu      $t3,$0,481            
 000468  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 00046c  a42b0012   sh         $t3,18($at)           # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1091
 000470  240c0800   addiu      $t4,$0,2048           
 000474  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000478  a42c001c   sh         $t4,28($at)           # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1092
 00047c  240d0800   addiu      $t5,$0,2048           
 000480  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000484  a42d001e   sh         $t5,30($at)           # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1093
 000488  8fbf003c   lw         $ra,60($sp)           
 00048c  00000000   nop                              
 000490  03e00008   jr         $ra                   
 000494  27bd00a0   addiu      $sp,$sp,160           
                    .end       InitialiseTexturesAndSprites
 # *****************************************

                    .text                            # BB:0,L:0,H:88
                    .globl     ClearBackground       
                    .ent       ClearBackground       
ClearBackground:                                     # CS:136,ETS:0
                                                     # Line number: 1104
 000000  27bdfff0   addiu      $sp,$sp,-16           
                                                     # Line number: 1107
 000004  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000008  ac200000   sw         $0,0($at)             # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1108
 00000c  3c084000   lui        $t0,0x4000            
 000010  afa80004   sw         $t0,4($sp)            # mask1
                                                     # Line number: 1109
 000014  3c092000   lui        $t1,0x2000            
 000018  afa90008   sw         $t1,8($sp)            # mask2
                                                     # Line number: 1110
 00001c  3c0a1000   lui        $t2,0x1000            
 000020  afaa000c   sw         $t2,12($sp)           # mask3
                                                     # Line number: 1113
 000024  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000028  8c2b0000   lw         $t3,0($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
 00002c  8fac0004   lw         $t4,4($sp)            # mask1
 000030  00000000   nop                              
 000034  016c6825   or         $t5,$t3,$t4           
 000038  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00003c  ac2d0000   sw         $t5,0($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1116
 000040  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000044  a4200004   sh         $0,4($at)             # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1117
 000048  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00004c  a4200006   sh         $0,6($at)             # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1118
 000050  240e0140   addiu      $t6,$0,320            
 000054  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000058  a42e0008   sh         $t6,8($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1119
 00005c  240800f0   addiu      $t0,$0,240            
 000060  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000064  a428000a   sh         $t0,10($at)           # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1120
 000068  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00006c  a020000c   sb         $0,12($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1121
 000070  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 000074  a020000d   sb         $0,13($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1122
 000078  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BackgroundRectangle
 00007c  a020000e   sb         $0,14($at)            # BackgroundRectangle,R_MIPS_LO16,BackgroundRectangle
                                                     # Line number: 1123
 000080  03e00008   jr         $ra                   
 000084  27bd0010   addiu      $sp,$sp,16            
                    .end       ClearBackground       
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     DealWithControllerPad 
                    .ent       DealWithControllerPad 
DealWithControllerPad:                               # CS:544,ETS:0
                                                     # Line number: 1129
 000000  27bdffd0   addiu      $sp,$sp,-48           
 000004  afbf002c   sw         $ra,44($sp)           
                                                     # Line number: 1133
 000008  0c000000   jal        PadRead               # R_MIPS_26,PadRead
 00000c  00000000   nop                              
 000010  3c010000   lui        $at,0x0000            # BB:1,L:10,H:30,R_MIPS_HI16,PadStatus
 000014  ac220000   sw         $v0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
                                                     # Line number: 1136
 000018  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 00001c  8c280000   lw         $t0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000020  00000000   nop                              
 000024  31090100   andi       $t1,$t0,0x0100        
 000028  1120000b   beq        $t1,$0,$100           
 00002c  00000000   nop                              
 000030  3c010000   lui        $at,0x0000            # BB:2,L:30,H:48,R_MIPS_HI16,PadStatus
 000034  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000038  00000000   nop                              
 00003c  314b0800   andi       $t3,$t2,0x0800        
 000040  11600005   beq        $t3,$0,$100           
 000044  00000000   nop                              
                                                     # Line number: 1138
 000048  240c0001   addiu      $t4,$0,1              # BB:3,L:48,H:58
 00004c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,QuitFlag
                                                     # Line number: 1139
 000050  1000006f   b          $101                  
 000054  ac2c0000   sw         $t4,0($at)            # QuitFlag,R_MIPS_LO16,QuitFlag
$100:                                                # BB:4,L:58,H:70
                                                     # Line number: 1143
 000058  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 00005c  8c2d0000   lw         $t5,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000060  00000000   nop                              
 000064  31ae0004   andi       $t6,$t5,0x0004        
 000068  11c0000b   beq        $t6,$0,$102           
 00006c  00000000   nop                              
                                                     # Line number: 1145
$103:                                                # BB:5,L:70,H:78
                                                     # Line number: 1147
 000070  0c000000   jal        PadRead               # R_MIPS_26,PadRead
 000074  00000000   nop                              
 000078  3c010000   lui        $at,0x0000            # BB:6,L:78,H:98,R_MIPS_HI16,PadStatus
 00007c  ac220000   sw         $v0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
                                                     # Line number: 1148
 000080  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 000084  8c280000   lw         $t0,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 000088  00000000   nop                              
 00008c  31090004   andi       $t1,$t0,0x0004        
 000090  1520fff7   bne        $t1,$0,$103           
 000094  00000000   nop                              
                                                     # Line number: 1151
$102:                                                # BB:7,L:98,H:b0
                                                     # Line number: 1154
 000098  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 00009c  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 0000a0  00000000   nop                              
 0000a4  314b8000   andi       $t3,$t2,0x8000        
 0000a8  1160001e   beq        $t3,$0,$104           
 0000ac  00000000   nop                              
                                                     # Line number: 1156
 0000b0  3c010000   lui        $at,0x0000            # BB:8,L:b0,H:e8,R_MIPS_HI16,PlayersMovement
 0000b4  8c2c0010   lw         $t4,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0000b8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayerSpeed
 0000bc  8c2d0000   lw         $t5,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
 0000c0  00000000   nop                              
 0000c4  018d7022   sub        $t6,$t4,$t5           
 0000c8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0000cc  ac2e0010   sw         $t6,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1158
 0000d0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0000d4  8c280010   lw         $t0,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0000d8  00000000   nop                              
 0000dc  2901fffc   slti       $at,$t0,-4            
 0000e0  10200003   beq        $at,$0,$106           
 0000e4  00000000   nop                              
 0000e8  1000000c   b          $107                  # BB:9,L:e8,H:f0
 0000ec  2419fffc   addiu      $t9,$0,-4             
$106:                                                # BB:10,L:f0,H:108
 0000f0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0000f4  8c290010   lw         $t1,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0000f8  00000000   nop                              
 0000fc  29210005   slti       $at,$t1,5             
 000100  14200003   bne        $at,$0,$109           
 000104  00000000   nop                              
 000108  10000004   b          $110                  # BB:11,L:108,H:110
 00010c  24190004   addiu      $t9,$0,4              
$109:                                                # BB:12,L:110,H:11c
 000110  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000114  8c390010   lw         $t9,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000118  00000000   nop                              
$110:                                                # BB:13,L:11c,H:11c
$107:                                                # BB:14,L:11c,H:124
 00011c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000120  ac390010   sw         $t9,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1159
$104:                                                # BB:15,L:124,H:13c
                                                     # Line number: 1160
 000124  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 000128  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 00012c  00000000   nop                              
 000130  314b2000   andi       $t3,$t2,0x2000        
 000134  1160001e   beq        $t3,$0,$111           
 000138  00000000   nop                              
                                                     # Line number: 1162
 00013c  3c010000   lui        $at,0x0000            # BB:16,L:13c,H:174,R_MIPS_HI16,PlayersMovement
 000140  8c2c0010   lw         $t4,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000144  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayerSpeed
 000148  8c2d0000   lw         $t5,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
 00014c  00000000   nop                              
 000150  018d7020   add        $t6,$t4,$t5           
 000154  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000158  ac2e0010   sw         $t6,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1164
 00015c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000160  8c280010   lw         $t0,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000164  00000000   nop                              
 000168  2901fffc   slti       $at,$t0,-4            
 00016c  10200003   beq        $at,$0,$113           
 000170  00000000   nop                              
 000174  1000000c   b          $114                  # BB:17,L:174,H:17c
 000178  2419fffc   addiu      $t9,$0,-4             
$113:                                                # BB:18,L:17c,H:194
 00017c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000180  8c290010   lw         $t1,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000184  00000000   nop                              
 000188  29210005   slti       $at,$t1,5             
 00018c  14200003   bne        $at,$0,$116           
 000190  00000000   nop                              
 000194  10000004   b          $117                  # BB:19,L:194,H:19c
 000198  24190004   addiu      $t9,$0,4              
$116:                                                # BB:20,L:19c,H:1a8
 00019c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0001a0  8c390010   lw         $t9,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0001a4  00000000   nop                              
$117:                                                # BB:21,L:1a8,H:1a8
$114:                                                # BB:22,L:1a8,H:1b0
 0001a8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0001ac  ac390010   sw         $t9,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1165
$111:                                                # BB:23,L:1b0,H:1c8
                                                     # Line number: 1167
 0001b0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 0001b4  8c2a0000   lw         $t2,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 0001b8  00000000   nop                              
 0001bc  314b0008   andi       $t3,$t2,0x0008        
 0001c0  11600007   beq        $t3,$0,$118           
 0001c4  00000000   nop                              
                                                     # Line number: 1168
 0001c8  3c010000   lui        $at,0x0000            # BB:24,L:1c8,H:1e0,R_MIPS_HI16,PlayerSpeed
 0001cc  8c2c0000   lw         $t4,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
 0001d0  00000000   nop                              
 0001d4  218d0001   addi       $t5,$t4,1             
 0001d8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayerSpeed
 0001dc  ac2d0000   sw         $t5,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
$118:                                                # BB:25,L:1e0,H:1f8
                                                     # Line number: 1169
 0001e0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PadStatus
 0001e4  8c2e0000   lw         $t6,0($at)            # PadStatus,R_MIPS_LO16,PadStatus
 0001e8  00000000   nop                              
 0001ec  31c80002   andi       $t0,$t6,0x0002        
 0001f0  11000007   beq        $t0,$0,$119           
 0001f4  00000000   nop                              
                                                     # Line number: 1170
 0001f8  3c010000   lui        $at,0x0000            # BB:26,L:1f8,H:210,R_MIPS_HI16,PlayerSpeed
 0001fc  8c290000   lw         $t1,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
 000200  00000000   nop                              
 000204  212affff   addi       $t2,$t1,-1            
 000208  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayerSpeed
 00020c  ac2a0000   sw         $t2,0($at)            # PlayerSpeed,R_MIPS_LO16,PlayerSpeed
$119:                                                # BB:27,L:210,H:210
                                                     # Line number: 1171
$101:                                                # BB:28,L:210,H:220
 000210  8fbf002c   lw         $ra,44($sp)           
 000214  00000000   nop                              
 000218  03e00008   jr         $ra                   
 00021c  27bd0030   addiu      $sp,$sp,48            
                    .end       DealWithControllerPad 
 # *****************************************

                    .text                            # BB:0,L:0,H:d8
                    .globl     HandleAllObjects      
                    .ent       HandleAllObjects      
HandleAllObjects:                                    # CS:3660,ETS:0
                                                     # Line number: 1187
 000000  27bdff80   addiu      $sp,$sp,-128          
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1192
 000008  afa00060   sw         $0,96($sp)            # ballMoved
                                                     # Line number: 1199
 00000c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000010  8c280000   lw         $t0,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000014  00000000   nop                              
 000018  00084c00   sll        $t1,$t0,16            
 00001c  00095403   sra        $t2,$t1,16            
 000020  a7aa0068   sh         $t2,104($sp)          # player
                                                     # Line number: 1200
 000024  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000028  8c2b0004   lw         $t3,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 00002c  00000000   nop                              
 000030  000b6400   sll        $t4,$t3,16            
 000034  000c6c03   sra        $t5,$t4,16            
 000038  a7ad006a   sh         $t5,106($sp)          # player
                                                     # Line number: 1201
 00003c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000040  8c2e0008   lw         $t6,8($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000044  00000000   nop                              
 000048  000e4400   sll        $t0,$t6,16            
 00004c  00084c03   sra        $t1,$t0,16            
 000050  a7a9006c   sh         $t1,108($sp)          # player
                                                     # Line number: 1202
 000054  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000058  8c2a000c   lw         $t2,12($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 00005c  00000000   nop                              
 000060  000a5c00   sll        $t3,$t2,16            
 000064  000b6403   sra        $t4,$t3,16            
 000068  a7ac006e   sh         $t4,110($sp)          # player
                                                     # Line number: 1205
 00006c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000070  8c2d0000   lw         $t5,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000074  00000000   nop                              
 000078  000d7400   sll        $t6,$t5,16            
 00007c  000e4403   sra        $t0,$t6,16            
 000080  a7a80070   sh         $t0,112($sp)          # ball
                                                     # Line number: 1206
 000084  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000088  8c290004   lw         $t1,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 00008c  00000000   nop                              
 000090  00095400   sll        $t2,$t1,16            
 000094  000a5c03   sra        $t3,$t2,16            
 000098  a7ab0072   sh         $t3,114($sp)          # ball
                                                     # Line number: 1207
 00009c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0000a0  8c2c0008   lw         $t4,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0000a4  00000000   nop                              
 0000a8  000c6c00   sll        $t5,$t4,16            
 0000ac  000d7403   sra        $t6,$t5,16            
 0000b0  a7ae0074   sh         $t6,116($sp)          # ball
                                                     # Line number: 1208
 0000b4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0000b8  8c28000c   lw         $t0,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0000bc  00000000   nop                              
 0000c0  00084c00   sll        $t1,$t0,16            
 0000c4  00095403   sra        $t2,$t1,16            
 0000c8  a7aa0076   sh         $t2,118($sp)          # ball
                                                     # Line number: 1212
 0000cc  27a40068   addiu      $a0,$sp,104           
 0000d0  0c000000   jal        RectanglesTouch       # R_MIPS_26,RectanglesTouch
 0000d4  27a50070   addiu      $a1,$sp,112           
 0000d8  14400006   bne        $v0,$0,$100           # BB:1,L:d8,H:e0
 0000dc  00000000   nop                              
 0000e0  27a40068   addiu      $a0,$sp,104           # BB:2,L:e0,H:ec
 0000e4  0c000000   jal        RectanglesOverlap     # R_MIPS_26,RectanglesOverlap
 0000e8  27a50070   addiu      $a1,$sp,112           
 0000ec  104000b8   beq        $v0,$0,$101           # BB:3,L:ec,H:f4
 0000f0  00000000   nop                              
$100:                                                # BB:4,L:f4,H:100
                                                     # Line number: 1215
 0000f4  27a40068   addiu      $a0,$sp,104           
 0000f8  0c000000   jal        RectanglesOverlap     # R_MIPS_26,RectanglesOverlap
 0000fc  27a50070   addiu      $a1,$sp,112           
 000100  10400003   beq        $v0,$0,$102           # BB:5,L:100,H:108
 000104  00000000   nop                              
                                                     # Line number: 1217
 000108  0c000000   jal        MoveBallOutOfBat      # BB:6,L:108,H:110,R_MIPS_26,MoveBallOutOfBat
 00010c  00000000   nop                              
                                                     # Line number: 1218
$102:                                                # BB:7,L:110,H:14c
                                                     # Line number: 1221
 000110  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000114  8c2b000c   lw         $t3,12($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000118  00000000   nop                              
 00011c  216cff3c   addi       $t4,$t3,-196          
 000120  000c6822   neg        $t5,$t4               
 000124  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000128  8c2e000c   lw         $t6,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00012c  00000000   nop                              
 000130  01ae4022   sub        $t0,$t5,$t6           
 000134  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000138  8c290004   lw         $t1,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 00013c  00000000   nop                              
 000140  0109082a   slt        $at,$t0,$t1           
 000144  1020004b   beq        $at,$0,$103           
 000148  00000000   nop                              
                                                     # Line number: 1224
 00014c  3c010000   lui        $at,0x0000            # BB:8,L:14c,H:160,R_MIPS_HI16,BallsMovement
 000150  8c2a0010   lw         $t2,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000154  00000000   nop                              
 000158  15400006   bne        $t2,$0,$104           
 00015c  00000000   nop                              
                                                     # Line number: 1225
 000160  3c010000   lui        $at,0x0000            # BB:9,L:160,H:174,R_MIPS_HI16,PlayersMovement
 000164  8c2b0010   lw         $t3,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000168  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00016c  1000005b   b          $105                  
 000170  ac2b0010   sw         $t3,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
$104:                                                # BB:10,L:174,H:18c
                                                     # Line number: 1226
 000174  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000178  8c2c0010   lw         $t4,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00017c  00000000   nop                              
 000180  0180082a   slt        $at,$t4,$0            
 000184  14200003   bne        $at,$0,$107           
 000188  00000000   nop                              
 00018c  10000002   b          $108                  # BB:11,L:18c,H:194
 000190  24190001   addiu      $t9,$0,1              
$107:                                                # BB:12,L:194,H:198
 000194  2419ffff   addiu      $t9,$0,-1             
$108:                                                # BB:13,L:198,H:1b4
 000198  03206821   move       $t5,$t9               
 00019c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0001a0  8c2e0010   lw         $t6,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0001a4  00000000   nop                              
 0001a8  01c0082a   slt        $at,$t6,$0            
 0001ac  14200003   bne        $at,$0,$110           
 0001b0  00000000   nop                              
 0001b4  10000002   b          $111                  # BB:14,L:1b4,H:1bc
 0001b8  24190001   addiu      $t9,$0,1              
$110:                                                # BB:15,L:1bc,H:1c0
 0001bc  2419ffff   addiu      $t9,$0,-1             
$111:                                                # BB:16,L:1c0,H:1c8
 0001c0  15b90016   bne        $t5,$t9,$112          
 0001c4  00000000   nop                              
                                                     # Line number: 1229
 0001c8  3c010000   lui        $at,0x0000            # BB:17,L:1c8,H:210,R_MIPS_HI16,BallsMovement
 0001cc  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0001d0  00000000   nop                              
 0001d4  44882000   mtc1       $t0,$f4               
 0001d8  00000000   nop                              
 0001dc  468021a0   cvt.s.w    $f6,$f4               
 0001e0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0001e4  8c290010   lw         $t1,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0001e8  00000000   nop                              
 0001ec  44894000   mtc1       $t1,$f8               
 0001f0  00000000   nop                              
 0001f4  468042a0   cvt.s.w    $f10,$f8              
 0001f8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Coefficient
 0001fc  c4240000   lwc1       $f4,0($at)            # Coefficient,R_MIPS_LO16,Coefficient
 000200  00000000   nop                              
 000204  460a2202   mul.s      $f8,$f4,$f10          
 000208  0c000000   jal        __cvt_fp2unsigned     # R_MIPS_26,__cvt_fp2unsigned
 00020c  46083300   add.s      $f12,$f6,$f8          
 000210  3c010000   lui        $at,0x0000            # BB:18,L:210,H:21c,R_MIPS_HI16,BallsMovement
                                                     # Line number: 1230
 000214  10000031   b          $105                  
 000218  ac220010   sw         $v0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
$112:                                                # BB:19,L:21c,H:268
                                                     # Line number: 1233
 00021c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000220  8c2a0010   lw         $t2,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000224  00000000   nop                              
 000228  448a5000   mtc1       $t2,$f10              
 00022c  00000000   nop                              
 000230  46805120   cvt.s.w    $f4,$f10              
 000234  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Coefficient
 000238  c4260000   lwc1       $f6,0($at)            # Coefficient,R_MIPS_LO16,Coefficient
 00023c  00000000   nop                              
 000240  46043202   mul.s      $f8,$f6,$f4           
 000244  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000248  8c2b0010   lw         $t3,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00024c  00000000   nop                              
 000250  000b6022   neg        $t4,$t3               
 000254  448c5000   mtc1       $t4,$f10              
 000258  00000000   nop                              
 00025c  46805120   cvt.s.w    $f4,$f10              
 000260  0c000000   jal        __cvt_fp2unsigned     # R_MIPS_26,__cvt_fp2unsigned
 000264  46082300   add.s      $f12,$f4,$f8          
 000268  3c010000   lui        $at,0x0000            # BB:20,L:268,H:274,R_MIPS_HI16,BallsMovement
                                                     # Line number: 1236
 00026c  1000001b   b          $105                  
 000270  ac220010   sw         $v0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
$103:                                                # BB:21,L:274,H:2d4
                                                     # Line number: 1240
 000274  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000278  8c2d0014   lw         $t5,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00027c  00000000   nop                              
 000280  000d7022   neg        $t6,$t5               
 000284  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000288  ac2e0014   sw         $t6,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1248
 00028c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000290  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000294  00000000   nop                              
 000298  44883000   mtc1       $t0,$f6               
 00029c  00000000   nop                              
 0002a0  46803220   cvt.s.w    $f8,$f6               
 0002a4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 0002a8  8c290010   lw         $t1,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 0002ac  00000000   nop                              
 0002b0  44895000   mtc1       $t1,$f10              
 0002b4  00000000   nop                              
 0002b8  46805120   cvt.s.w    $f4,$f10              
 0002bc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,Coefficient
 0002c0  c4260000   lwc1       $f6,0($at)            # Coefficient,R_MIPS_LO16,Coefficient
 0002c4  00000000   nop                              
 0002c8  46043282   mul.s      $f10,$f6,$f4          
 0002cc  0c000000   jal        __cvt_fp2unsigned     # R_MIPS_26,__cvt_fp2unsigned
 0002d0  460a4300   add.s      $f12,$f8,$f10         
 0002d4  3c010000   lui        $at,0x0000            # BB:22,L:2d4,H:2dc,R_MIPS_HI16,BallsMovement
 0002d8  ac220010   sw         $v0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1249
$105:                                                # BB:23,L:2dc,H:2f4
                                                     # Line number: 1253
 0002dc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0002e0  8c2a0010   lw         $t2,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0002e4  00000000   nop                              
 0002e8  2941fffb   slti       $at,$t2,-5            
 0002ec  10200003   beq        $at,$0,$114           
 0002f0  00000000   nop                              
 0002f4  1000000c   b          $115                  # BB:24,L:2f4,H:2fc
 0002f8  2419fffb   addiu      $t9,$0,-5             
$114:                                                # BB:25,L:2fc,H:314
 0002fc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000300  8c2b0010   lw         $t3,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000304  00000000   nop                              
 000308  29610006   slti       $at,$t3,6             
 00030c  14200003   bne        $at,$0,$117           
 000310  00000000   nop                              
 000314  10000004   b          $118                  # BB:26,L:314,H:31c
 000318  24190005   addiu      $t9,$0,5              
$117:                                                # BB:27,L:31c,H:328
 00031c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000320  8c390010   lw         $t9,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000324  00000000   nop                              
$118:                                                # BB:28,L:328,H:328
$115:                                                # BB:29,L:328,H:348
 000328  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00032c  ac390010   sw         $t9,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1255
 000330  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000334  8c2c0014   lw         $t4,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000338  00000000   nop                              
 00033c  2981fffb   slti       $at,$t4,-5            
 000340  10200003   beq        $at,$0,$120           
 000344  00000000   nop                              
 000348  1000000c   b          $121                  # BB:30,L:348,H:350
 00034c  2419fffb   addiu      $t9,$0,-5             
$120:                                                # BB:31,L:350,H:368
 000350  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000354  8c2d0014   lw         $t5,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000358  00000000   nop                              
 00035c  29a10006   slti       $at,$t5,6             
 000360  14200003   bne        $at,$0,$123           
 000364  00000000   nop                              
 000368  10000004   b          $124                  # BB:32,L:368,H:370
 00036c  24190005   addiu      $t9,$0,5              
$123:                                                # BB:33,L:370,H:37c
 000370  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000374  8c390014   lw         $t9,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000378  00000000   nop                              
$124:                                                # BB:34,L:37c,H:37c
$121:                                                # BB:35,L:37c,H:3d0
 00037c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000380  ac390014   sw         $t9,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1256
 000384  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000388  8c2e0000   lw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 00038c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000390  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000394  00000000   nop                              
 000398  01c84820   add        $t1,$t6,$t0           
 00039c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003a0  ac290000   sw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1257
 0003a4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003a8  8c2a0004   lw         $t2,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0003ac  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003b0  8c2b0014   lw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0003b4  00000000   nop                              
 0003b8  014b6020   add        $t4,$t2,$t3           
 0003bc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003c0  ac2c0004   sw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1258
 0003c4  240d0001   addiu      $t5,$0,1              
                                                     # Line number: 1259
 0003c8  100001a0   b          $125                  
 0003cc  afad0060   sw         $t5,96($sp)           # ballMoved
$101:                                                # BB:36,L:3d0,H:450
                                                     # Line number: 1263
 0003d0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003d4  8c2e0000   lw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0003d8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003dc  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0003e0  00000000   nop                              
 0003e4  01c84820   add        $t1,$t6,$t0           
 0003e8  00095400   sll        $t2,$t1,16            
 0003ec  000a5c03   sra        $t3,$t2,16            
 0003f0  a7ab0070   sh         $t3,112($sp)          # ball
                                                     # Line number: 1264
 0003f4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0003f8  8c2c0004   lw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0003fc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000400  8c2d0014   lw         $t5,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000404  00000000   nop                              
 000408  018d7020   add        $t6,$t4,$t5           
 00040c  000e4400   sll        $t0,$t6,16            
 000410  00084c03   sra        $t1,$t0,16            
 000414  a7a90072   sh         $t1,114($sp)          # ball
                                                     # Line number: 1265
 000418  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00041c  8c2a0008   lw         $t2,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000420  00000000   nop                              
 000424  000a5c00   sll        $t3,$t2,16            
 000428  000b6403   sra        $t4,$t3,16            
 00042c  a7ac0074   sh         $t4,116($sp)          # ball
                                                     # Line number: 1266
 000430  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000434  8c2d000c   lw         $t5,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000438  00000000   nop                              
 00043c  000d7400   sll        $t6,$t5,16            
 000440  000e4403   sra        $t0,$t6,16            
 000444  a7a80076   sh         $t0,118($sp)          # ball
                                                     # Line number: 1267
 000448  1000017b   b          $126                  
 00044c  afa00064   sw         $0,100($sp)           # i
$127:                                                # BB:37,L:450,H:47c
                                                     # Line number: 1269
 000450  8fa90064   lw         $t1,100($sp)          # i
 000454  240a004c   addiu      $t2,$0,76             
 000458  012a0018   mult       $t1,$t2               
 00045c  00005812   mflo       $t3                   
 000460  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 000464  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 000468  018b6821   addu       $t5,$t4,$t3           
 00046c  8dae0004   lw         $t6,4($t5)            
 000470  24010001   addiu      $at,$0,1              
 000474  15c1016c   bne        $t6,$at,$128          
 000478  00000000   nop                              
                                                     # Line number: 1272
 00047c  8fa80064   lw         $t0,100($sp)          # BB:38,L:47c,H:5e0,i
 000480  2409004c   addiu      $t1,$0,76             
 000484  01090018   mult       $t0,$t1               
 000488  00005012   mflo       $t2                   
 00048c  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000490  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000494  016a6021   addu       $t4,$t3,$t2           
 000498  8d8d0034   lw         $t5,52($t4)           
 00049c  00000000   nop                              
 0004a0  000d7400   sll        $t6,$t5,16            
 0004a4  000e4403   sra        $t0,$t6,16            
 0004a8  8fa90064   lw         $t1,100($sp)          # i
 0004ac  240a004c   addiu      $t2,$0,76             
 0004b0  012a0018   mult       $t1,$t2               
 0004b4  00005812   mflo       $t3                   
 0004b8  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 0004bc  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 0004c0  018b6821   addu       $t5,$t4,$t3           
 0004c4  a5a80014   sh         $t0,20($t5)           
                                                     # Line number: 1274
 0004c8  8fae0064   lw         $t6,100($sp)          # i
 0004cc  2408004c   addiu      $t0,$0,76             
 0004d0  01c80018   mult       $t6,$t0               
 0004d4  00004812   mflo       $t1                   
 0004d8  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 0004dc  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 0004e0  01495821   addu       $t3,$t2,$t1           
 0004e4  8d6c0038   lw         $t4,56($t3)           
 0004e8  00000000   nop                              
 0004ec  000c6c00   sll        $t5,$t4,16            
 0004f0  000d7403   sra        $t6,$t5,16            
 0004f4  8fa80064   lw         $t0,100($sp)          # i
 0004f8  2409004c   addiu      $t1,$0,76             
 0004fc  01090018   mult       $t0,$t1               
 000500  00005012   mflo       $t2                   
 000504  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000508  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 00050c  016a6021   addu       $t4,$t3,$t2           
 000510  a58e0016   sh         $t6,22($t4)           
                                                     # Line number: 1278
 000514  8fad0064   lw         $t5,100($sp)          # i
 000518  240e004c   addiu      $t6,$0,76             
 00051c  01ae0018   mult       $t5,$t6               
 000520  00004012   mflo       $t0                   
 000524  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000528  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 00052c  01285021   addu       $t2,$t1,$t0           
 000530  8d4b0034   lw         $t3,52($t2)           
 000534  00000000   nop                              
 000538  000b6400   sll        $t4,$t3,16            
 00053c  000c6c03   sra        $t5,$t4,16            
 000540  a7ad0078   sh         $t5,120($sp)          # brick
                                                     # Line number: 1279
 000544  8fae0064   lw         $t6,100($sp)          # i
 000548  2408004c   addiu      $t0,$0,76             
 00054c  01c80018   mult       $t6,$t0               
 000550  00004812   mflo       $t1                   
 000554  3c0a0000   lui        $t2,0x0000            # R_MIPS_HI16,BricksArray
 000558  214a0000   addi       $t2,$t2,0             # R_MIPS_LO16,BricksArray
 00055c  01495821   addu       $t3,$t2,$t1           
 000560  8d6c0038   lw         $t4,56($t3)           
 000564  00000000   nop                              
 000568  000c6c00   sll        $t5,$t4,16            
 00056c  000d7403   sra        $t6,$t5,16            
 000570  a7ae007a   sh         $t6,122($sp)          # brick
                                                     # Line number: 1280
 000574  8fa80064   lw         $t0,100($sp)          # i
 000578  2409004c   addiu      $t1,$0,76             
 00057c  01090018   mult       $t0,$t1               
 000580  00005012   mflo       $t2                   
 000584  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000588  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 00058c  016a6021   addu       $t4,$t3,$t2           
 000590  8d8d003c   lw         $t5,60($t4)           
 000594  00000000   nop                              
 000598  000d7400   sll        $t6,$t5,16            
 00059c  000e4403   sra        $t0,$t6,16            
 0005a0  a7a8007c   sh         $t0,124($sp)          # brick
                                                     # Line number: 1281
 0005a4  8fa90064   lw         $t1,100($sp)          # i
 0005a8  240a004c   addiu      $t2,$0,76             
 0005ac  012a0018   mult       $t1,$t2               
 0005b0  00005812   mflo       $t3                   
 0005b4  3c0c0000   lui        $t4,0x0000            # R_MIPS_HI16,BricksArray
 0005b8  218c0000   addi       $t4,$t4,0             # R_MIPS_LO16,BricksArray
 0005bc  018b6821   addu       $t5,$t4,$t3           
 0005c0  8dae0040   lw         $t6,64($t5)           
 0005c4  00000000   nop                              
 0005c8  000e4400   sll        $t0,$t6,16            
 0005cc  00084c03   sra        $t1,$t0,16            
 0005d0  a7a9007e   sh         $t1,126($sp)          # brick
                                                     # Line number: 1283
 0005d4  27a40070   addiu      $a0,$sp,112           
 0005d8  0c000000   jal        RectanglesOverlap     # R_MIPS_26,RectanglesOverlap
 0005dc  27a50078   addiu      $a1,$sp,120           
 0005e0  14400006   bne        $v0,$0,$129           # BB:39,L:5e0,H:5e8
 0005e4  00000000   nop                              
 0005e8  27a40070   addiu      $a0,$sp,112           # BB:40,L:5e8,H:5f4
 0005ec  0c000000   jal        RectanglesTouch       # R_MIPS_26,RectanglesTouch
 0005f0  27a50078   addiu      $a1,$sp,120           
 0005f4  1040010c   beq        $v0,$0,$130           # BB:41,L:5f4,H:5fc
 0005f8  00000000   nop                              
$129:                                                # BB:42,L:5fc,H:608
                                                     # Line number: 1286
 0005fc  27a40070   addiu      $a0,$sp,112           
 000600  0c000000   jal        FindWhereRectanglesTou# R_MIPS_26,FindWhereRectanglesTouch
 000604  27a50078   addiu      $a1,$sp,120           
 000608  afa2004c   sw         $v0,76($sp)           # BB:43,L:608,H:620,edge
                                                     # Line number: 1290
 00060c  8faa004c   lw         $t2,76($sp)           # edge
 000610  00000000   nop                              
 000614  2d410009   sltiu      $at,$t2,9             
 000618  102000e0   beq        $at,$0,$139           
 00061c  00000000   nop                              
 000620  3c0b0000   lui        $t3,0x0000            # BB:44,L:620,H:640,R_MIPS_HI16,@327
 000624  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,@327
 000628  000a6080   sll        $t4,$t2,2             
 00062c  018b6821   addu       $t5,$t4,$t3           
 000630  8dae0000   lw         $t6,0($t5)            
 000634  00000000   nop                              
 000638  01c00008   jr         $t6                   
 00063c  00000000   nop                              
                                                     # Line number: 1292
$138:                                                # BB:45,L:640,H:72c
                                                     # Line number: 1293
 000640  8fa80064   lw         $t0,100($sp)          # i
 000644  2409004c   addiu      $t1,$0,76             
 000648  01090018   mult       $t0,$t1               
 00064c  00005012   mflo       $t2                   
 000650  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 000654  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 000658  016a6021   addu       $t4,$t3,$t2           
 00065c  8fad0064   lw         $t5,100($sp)          # i
 000660  240e004c   addiu      $t6,$0,76             
 000664  01ae0018   mult       $t5,$t6               
 000668  00004012   mflo       $t0                   
 00066c  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000670  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000674  01285021   addu       $t2,$t1,$t0           
 000678  8d8b0034   lw         $t3,52($t4)           
 00067c  8d4c003c   lw         $t4,60($t2)           
 000680  00000000   nop                              
 000684  016c6820   add        $t5,$t3,$t4           
 000688  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00068c  8c2e0000   lw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000690  00000000   nop                              
 000694  01cd4022   sub        $t0,$t6,$t5           
 000698  afa80050   sw         $t0,80($sp)           # xInitialDistance
                                                     # Line number: 1296
 00069c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0006a0  8c290010   lw         $t1,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0006a4  00000000   nop                              
 0006a8  00095022   neg        $t2,$t1               
 0006ac  8fab0050   lw         $t3,80($sp)           # xInitialDistance
 0006b0  00000000   nop                              
 0006b4  014b6022   sub        $t4,$t2,$t3           
 0006b8  afac0054   sw         $t4,84($sp)           # xLeftOver
                                                     # Line number: 1298
 0006bc  8fad0054   lw         $t5,84($sp)           # xLeftOver
 0006c0  00000000   nop                              
 0006c4  000d7040   sll        $t6,$t5,1             
 0006c8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0006cc  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0006d0  00000000   nop                              
 0006d4  010e4820   add        $t1,$t0,$t6           
 0006d8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0006dc  8c2a0000   lw         $t2,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0006e0  00000000   nop                              
 0006e4  01495820   add        $t3,$t2,$t1           
 0006e8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0006ec  ac2b0000   sw         $t3,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1300
 0006f0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0006f4  8c2c0010   lw         $t4,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0006f8  00000000   nop                              
 0006fc  000c6822   neg        $t5,$t4               
 000700  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000704  ac2d0010   sw         $t5,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1301
 000708  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00070c  8c2e0004   lw         $t6,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000710  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000714  8c280014   lw         $t0,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000718  00000000   nop                              
 00071c  01c84820   add        $t1,$t6,$t0           
 000720  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
                                                     # Line number: 1302
 000724  1000009d   b          $140                  
 000728  ac290004   sw         $t1,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
$137:                                                # BB:46,L:72c,H:7f0
                                                     # Line number: 1304
 00072c  8faa0064   lw         $t2,100($sp)          # i
 000730  240b004c   addiu      $t3,$0,76             
 000734  014b0018   mult       $t2,$t3               
 000738  00006012   mflo       $t4                   
 00073c  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,BricksArray
 000740  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,BricksArray
 000744  01ac7021   addu       $t6,$t5,$t4           
 000748  8dc80034   lw         $t0,52($t6)           
 00074c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000750  8c290000   lw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000754  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000758  8c2a0008   lw         $t2,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 00075c  00000000   nop                              
 000760  012a5820   add        $t3,$t1,$t2           
 000764  010b6022   sub        $t4,$t0,$t3           
 000768  afac0050   sw         $t4,80($sp)           # xInitialDistance
                                                     # Line number: 1307
 00076c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000770  8c2d0010   lw         $t5,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000774  8fae0050   lw         $t6,80($sp)           # xInitialDistance
 000778  00000000   nop                              
 00077c  01ae4022   sub        $t0,$t5,$t6           
 000780  afa80054   sw         $t0,84($sp)           # xLeftOver
                                                     # Line number: 1309
 000784  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000788  8c290010   lw         $t1,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00078c  8faa0054   lw         $t2,84($sp)           # xLeftOver
 000790  00000000   nop                              
 000794  000a5840   sll        $t3,$t2,1             
 000798  012b6022   sub        $t4,$t1,$t3           
 00079c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007a0  8c2d0000   lw         $t5,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0007a4  00000000   nop                              
 0007a8  01ac7020   add        $t6,$t5,$t4           
 0007ac  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007b0  ac2e0000   sw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1311
 0007b4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007b8  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0007bc  00000000   nop                              
 0007c0  00084822   neg        $t1,$t0               
 0007c4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007c8  ac290010   sw         $t1,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1312
 0007cc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007d0  8c2a0004   lw         $t2,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0007d4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0007d8  8c2b0014   lw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0007dc  00000000   nop                              
 0007e0  014b6020   add        $t4,$t2,$t3           
 0007e4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
                                                     # Line number: 1313
 0007e8  1000006c   b          $140                  
 0007ec  ac2c0004   sw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
$136:                                                # BB:47,L:7f0,H:7f0
                                                     # Line number: 1315
$135:                                                # BB:48,L:7f0,H:7f0
                                                     # Line number: 1316
$134:                                                # BB:49,L:7f0,H:8dc
                                                     # Line number: 1317
 0007f0  8fad0064   lw         $t5,100($sp)          # i
 0007f4  240e004c   addiu      $t6,$0,76             
 0007f8  01ae0018   mult       $t5,$t6               
 0007fc  00004012   mflo       $t0                   
 000800  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,BricksArray
 000804  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,BricksArray
 000808  01285021   addu       $t2,$t1,$t0           
 00080c  8fab0064   lw         $t3,100($sp)          # i
 000810  240c004c   addiu      $t4,$0,76             
 000814  016c0018   mult       $t3,$t4               
 000818  00006812   mflo       $t5                   
 00081c  3c0e0000   lui        $t6,0x0000            # R_MIPS_HI16,BricksArray
 000820  21ce0000   addi       $t6,$t6,0             # R_MIPS_LO16,BricksArray
 000824  01cd4021   addu       $t0,$t6,$t5           
 000828  8d490038   lw         $t1,56($t2)           
 00082c  8d0a0040   lw         $t2,64($t0)           
 000830  00000000   nop                              
 000834  012a5820   add        $t3,$t1,$t2           
 000838  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00083c  8c2c0004   lw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000840  00000000   nop                              
 000844  018b6822   sub        $t5,$t4,$t3           
 000848  afad0058   sw         $t5,88($sp)           # yInitialDistance
                                                     # Line number: 1320
 00084c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000850  8c2e0014   lw         $t6,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000854  00000000   nop                              
 000858  000e4022   neg        $t0,$t6               
 00085c  8fa90058   lw         $t1,88($sp)           # yInitialDistance
 000860  00000000   nop                              
 000864  01095022   sub        $t2,$t0,$t1           
 000868  afaa005c   sw         $t2,92($sp)           # yLeftOver
                                                     # Line number: 1322
 00086c  8fab005c   lw         $t3,92($sp)           # yLeftOver
 000870  00000000   nop                              
 000874  000b6040   sll        $t4,$t3,1             
 000878  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00087c  8c2d0014   lw         $t5,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000880  00000000   nop                              
 000884  01ac7020   add        $t6,$t5,$t4           
 000888  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00088c  8c280004   lw         $t0,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000890  00000000   nop                              
 000894  010e4820   add        $t1,$t0,$t6           
 000898  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00089c  ac290004   sw         $t1,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1324
 0008a0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0008a4  8c2a0014   lw         $t2,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0008a8  00000000   nop                              
 0008ac  000a5822   neg        $t3,$t2               
 0008b0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0008b4  ac2b0014   sw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1325
 0008b8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0008bc  8c2c0000   lw         $t4,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 0008c0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 0008c4  8c2d0010   lw         $t5,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 0008c8  00000000   nop                              
 0008cc  018d7020   add        $t6,$t4,$t5           
 0008d0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
                                                     # Line number: 1326
 0008d4  10000031   b          $140                  
 0008d8  ac2e0000   sw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
$133:                                                # BB:50,L:8dc,H:8dc
                                                     # Line number: 1328
$132:                                                # BB:51,L:8dc,H:8dc
                                                     # Line number: 1329
$131:                                                # BB:52,L:8dc,H:99c
                                                     # Line number: 1330
 0008dc  8fa80064   lw         $t0,100($sp)          # i
 0008e0  2409004c   addiu      $t1,$0,76             
 0008e4  01090018   mult       $t0,$t1               
 0008e8  00005012   mflo       $t2                   
 0008ec  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,BricksArray
 0008f0  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,BricksArray
 0008f4  016a6021   addu       $t4,$t3,$t2           
 0008f8  8d8d0038   lw         $t5,56($t4)           
 0008fc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000900  8c2e0004   lw         $t6,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000904  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000908  8c28000c   lw         $t0,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00090c  00000000   nop                              
 000910  01c84820   add        $t1,$t6,$t0           
 000914  01a95022   sub        $t2,$t5,$t1           
 000918  afaa0058   sw         $t2,88($sp)           # yInitialDistance
                                                     # Line number: 1333
 00091c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000920  8c2b0014   lw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000924  8fac0058   lw         $t4,88($sp)           # yInitialDistance
 000928  00000000   nop                              
 00092c  016c6822   sub        $t5,$t3,$t4           
 000930  afad005c   sw         $t5,92($sp)           # yLeftOver
                                                     # Line number: 1335
 000934  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000938  8c2e0014   lw         $t6,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00093c  8fa8005c   lw         $t0,92($sp)           # yLeftOver
 000940  00000000   nop                              
 000944  00084840   sll        $t1,$t0,1             
 000948  01c95022   sub        $t2,$t6,$t1           
 00094c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000950  8c2b0004   lw         $t3,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000954  00000000   nop                              
 000958  016a6020   add        $t4,$t3,$t2           
 00095c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000960  ac2c0004   sw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1337
 000964  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000968  8c2d0014   lw         $t5,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00096c  00000000   nop                              
 000970  000d7022   neg        $t6,$t5               
 000974  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000978  ac2e0014   sw         $t6,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1338
 00097c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000980  8c280000   lw         $t0,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000984  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000988  8c290010   lw         $t1,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 00098c  00000000   nop                              
 000990  01095020   add        $t2,$t0,$t1           
 000994  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000998  ac2a0000   sw         $t2,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1340
$139:                                                # BB:53,L:99c,H:99c
                                                     # Line number: 1342
$140:                                                # BB:54,L:99c,H:a00
                                                     # Line number: 1344
 00099c  240b0001   addiu      $t3,$0,1              
 0009a0  afab0060   sw         $t3,96($sp)           # ballMoved
                                                     # Line number: 1345
 0009a4  8fac0064   lw         $t4,100($sp)          # i
 0009a8  240d004c   addiu      $t5,$0,76             
 0009ac  018d0018   mult       $t4,$t5               
 0009b0  00007012   mflo       $t6                   
 0009b4  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0009b8  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 0009bc  010e4821   addu       $t1,$t0,$t6           
 0009c0  8d2a000c   lw         $t2,12($t1)           
 0009c4  00000000   nop                              
 0009c8  214bffff   addi       $t3,$t2,-1            
 0009cc  ad2b000c   sw         $t3,12($t1)           
                                                     # Line number: 1346
 0009d0  8fac0064   lw         $t4,100($sp)          # i
 0009d4  240d004c   addiu      $t5,$0,76             
 0009d8  018d0018   mult       $t4,$t5               
 0009dc  00007012   mflo       $t6                   
 0009e0  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,BricksArray
 0009e4  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,BricksArray
 0009e8  010e4821   addu       $t1,$t0,$t6           
 0009ec  8d2a000c   lw         $t2,12($t1)           
 0009f0  00000000   nop                              
 0009f4  000a082a   slt        $at,$0,$t2            
 0009f8  14200014   bne        $at,$0,$141           
 0009fc  00000000   nop                              
                                                     # Line number: 1348
 000a00  8fa40064   lw         $a0,100($sp)          # BB:55,L:a00,H:a0c,i
 000a04  0c000000   jal        DestroyBrick          # R_MIPS_26,DestroyBrick
 000a08  00000000   nop                              
                                                     # Line number: 1349
 000a0c  3c010000   lui        $at,0x0000            # BB:56,L:a0c,H:a28,R_MIPS_HI16,CurrentScore
 000a10  8c2b0010   lw         $t3,16($at)           # CurrentScore,R_MIPS_LO16,CurrentScore
 000a14  00000000   nop                              
 000a18  256c0032   addiu      $t4,$t3,50            
 000a1c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,CurrentScore
                                                     # Line number: 1351
 000a20  1000000a   b          $141                  
 000a24  ac2c0010   sw         $t4,16($at)           # CurrentScore,R_MIPS_LO16,CurrentScore
$130:                                                # BB:57,L:a28,H:a28
                                                     # Line number: 1353
$128:                                                # BB:58,L:a28,H:a38
                                                     # Line number: 1354
 000a28  8fad0064   lw         $t5,100($sp)          # i
 000a2c  00000000   nop                              
 000a30  21ae0001   addi       $t6,$t5,1             
 000a34  afae0064   sw         $t6,100($sp)          # i
$126:                                                # BB:59,L:a38,H:a4c
 000a38  8fa80064   lw         $t0,100($sp)          # i
 000a3c  00000000   nop                              
 000a40  290100c8   slti       $at,$t0,200           
 000a44  1420fe82   bne        $at,$0,$127           
 000a48  00000000   nop                              
$141:                                                # BB:60,L:a4c,H:a4c
                                                     # Line number: 1355
$125:                                                # BB:61,L:a4c,H:a64
                                                     # Line number: 1359
 000a4c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000a50  8c290010   lw         $t1,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000a54  00000000   nop                              
 000a58  2921fffb   slti       $at,$t1,-5            
 000a5c  10200003   beq        $at,$0,$143           
 000a60  00000000   nop                              
 000a64  1000000c   b          $144                  # BB:62,L:a64,H:a6c
 000a68  2419fffb   addiu      $t9,$0,-5             
$143:                                                # BB:63,L:a6c,H:a84
 000a6c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000a70  8c2a0010   lw         $t2,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000a74  00000000   nop                              
 000a78  29410006   slti       $at,$t2,6             
 000a7c  14200003   bne        $at,$0,$146           
 000a80  00000000   nop                              
 000a84  10000004   b          $147                  # BB:64,L:a84,H:a8c
 000a88  24190005   addiu      $t9,$0,5              
$146:                                                # BB:65,L:a8c,H:a98
 000a8c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000a90  8c390010   lw         $t9,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000a94  00000000   nop                              
$147:                                                # BB:66,L:a98,H:a98
$144:                                                # BB:67,L:a98,H:ab8
 000a98  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000a9c  ac390010   sw         $t9,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1361
 000aa0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000aa4  8c2b0014   lw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000aa8  00000000   nop                              
 000aac  2961fffb   slti       $at,$t3,-5            
 000ab0  10200003   beq        $at,$0,$149           
 000ab4  00000000   nop                              
 000ab8  1000000c   b          $150                  # BB:68,L:ab8,H:ac0
 000abc  2419fffb   addiu      $t9,$0,-5             
$149:                                                # BB:69,L:ac0,H:ad8
 000ac0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000ac4  8c2c0014   lw         $t4,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000ac8  00000000   nop                              
 000acc  29810006   slti       $at,$t4,6             
 000ad0  14200003   bne        $at,$0,$152           
 000ad4  00000000   nop                              
 000ad8  10000004   b          $153                  # BB:70,L:ad8,H:ae0
 000adc  24190005   addiu      $t9,$0,5              
$152:                                                # BB:71,L:ae0,H:aec
 000ae0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000ae4  8c390014   lw         $t9,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000ae8  00000000   nop                              
$153:                                                # BB:72,L:aec,H:aec
$150:                                                # BB:73,L:aec,H:b04
 000aec  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000af0  ac390014   sw         $t9,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1364
 000af4  8fad0060   lw         $t5,96($sp)           # ballMoved
 000af8  00000000   nop                              
 000afc  15a00011   bne        $t5,$0,$154           
 000b00  00000000   nop                              
                                                     # Line number: 1367
 000b04  3c010000   lui        $at,0x0000            # BB:74,L:b04,H:b44,R_MIPS_HI16,BallsMovement
 000b08  8c2e0000   lw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b0c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b10  8c280010   lw         $t0,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000b14  00000000   nop                              
 000b18  01c84820   add        $t1,$t6,$t0           
 000b1c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b20  ac290000   sw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1368
 000b24  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b28  8c2a0004   lw         $t2,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b2c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b30  8c2b0014   lw         $t3,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000b34  00000000   nop                              
 000b38  014b6020   add        $t4,$t2,$t3           
 000b3c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b40  ac2c0004   sw         $t4,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1369
$154:                                                # BB:75,L:b44,H:b70
                                                     # Line number: 1372
 000b44  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b48  8c2d0008   lw         $t5,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b4c  00000000   nop                              
 000b50  21aefee0   addi       $t6,$t5,-288          
 000b54  000e4022   neg        $t0,$t6               
 000b58  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b5c  8c290000   lw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b60  00000000   nop                              
 000b64  0109082a   slt        $at,$t0,$t1           
 000b68  10200017   beq        $at,$0,$155           
 000b6c  00000000   nop                              
                                                     # Line number: 1374
 000b70  3c010000   lui        $at,0x0000            # BB:76,L:b70,H:bc8,R_MIPS_HI16,BallsMovement
 000b74  8c2a0008   lw         $t2,8($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b78  00000000   nop                              
 000b7c  214bfee0   addi       $t3,$t2,-288          
 000b80  000b6022   neg        $t4,$t3               
 000b84  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b88  8c2d0000   lw         $t5,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000b8c  00000000   nop                              
 000b90  01ac7022   sub        $t6,$t5,$t4           
 000b94  000e4040   sll        $t0,$t6,1             
 000b98  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000b9c  8c290000   lw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000ba0  00000000   nop                              
 000ba4  01285022   sub        $t2,$t1,$t0           
 000ba8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000bac  ac2a0000   sw         $t2,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1376
 000bb0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000bb4  8c2b0010   lw         $t3,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000bb8  00000000   nop                              
 000bbc  000b6022   neg        $t4,$t3               
 000bc0  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000bc4  ac2c0010   sw         $t4,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1377
$155:                                                # BB:77,L:bc8,H:be0
                                                     # Line number: 1378
 000bc8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000bcc  8c2d0000   lw         $t5,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000bd0  00000000   nop                              
 000bd4  29a10010   slti       $at,$t5,16            
 000bd8  10200013   beq        $at,$0,$156           
 000bdc  00000000   nop                              
                                                     # Line number: 1380
 000be0  3c010000   lui        $at,0x0000            # BB:78,L:be0,H:c28,R_MIPS_HI16,BallsMovement
 000be4  8c2e0000   lw         $t6,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000be8  00000000   nop                              
 000bec  21c8fff0   addi       $t0,$t6,-16           
 000bf0  00084822   neg        $t1,$t0               
 000bf4  00095040   sll        $t2,$t1,1             
 000bf8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000bfc  8c2b0000   lw         $t3,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000c00  00000000   nop                              
 000c04  016a6020   add        $t4,$t3,$t2           
 000c08  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c0c  ac2c0000   sw         $t4,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1381
 000c10  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c14  8c2d0010   lw         $t5,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000c18  00000000   nop                              
 000c1c  000d7022   neg        $t6,$t5               
 000c20  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c24  ac2e0010   sw         $t6,16($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1382
$156:                                                # BB:79,L:c28,H:c40
                                                     # Line number: 1383
 000c28  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c2c  8c280004   lw         $t0,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000c30  00000000   nop                              
 000c34  2901001e   slti       $at,$t0,30            
 000c38  10200013   beq        $at,$0,$157           
 000c3c  00000000   nop                              
                                                     # Line number: 1385
 000c40  3c010000   lui        $at,0x0000            # BB:80,L:c40,H:c88,R_MIPS_HI16,BallsMovement
 000c44  8c290004   lw         $t1,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000c48  00000000   nop                              
 000c4c  212affe2   addi       $t2,$t1,-30           
 000c50  000a5822   neg        $t3,$t2               
 000c54  000b6040   sll        $t4,$t3,1             
 000c58  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c5c  8c2d0004   lw         $t5,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000c60  00000000   nop                              
 000c64  01ac7020   add        $t6,$t5,$t4           
 000c68  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c6c  ac2e0004   sw         $t6,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1386
 000c70  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c74  8c280014   lw         $t0,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000c78  00000000   nop                              
 000c7c  00084822   neg        $t1,$t0               
 000c80  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c84  ac290014   sw         $t1,20($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1387
$157:                                                # BB:81,L:c88,H:cb4
                                                     # Line number: 1388
 000c88  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000c8c  8c2a000c   lw         $t2,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000c90  00000000   nop                              
 000c94  214bff3c   addi       $t3,$t2,-196          
 000c98  000b6022   neg        $t4,$t3               
 000c9c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000ca0  8c2d0004   lw         $t5,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000ca4  00000000   nop                              
 000ca8  018d082a   slt        $at,$t4,$t5           
 000cac  1020000d   beq        $at,$0,$158           
 000cb0  00000000   nop                              
                                                     # Line number: 1390
 000cb4  3c010000   lui        $at,0x0000            # BB:82,L:cb4,H:cd4,R_MIPS_HI16,NumberOfLives
 000cb8  8c2e0000   lw         $t6,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
 000cbc  00000000   nop                              
 000cc0  21c8ffff   addi       $t0,$t6,-1            
 000cc4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,NumberOfLives
 000cc8  ac280000   sw         $t0,0($at)            # NumberOfLives,R_MIPS_LO16,NumberOfLives
                                                     # Line number: 1391
 000ccc  0c000000   jal        CrudestPause          # R_MIPS_26,CrudestPause
 000cd0  2404001e   addiu      $a0,$0,30             
                                                     # Line number: 1392
 000cd4  0c000000   jal        RestartTheBall        # BB:83,L:cd4,H:cdc,R_MIPS_26,RestartTheBall
 000cd8  00000000   nop                              
                                                     # Line number: 1393
 000cdc  0c000000   jal        RestartPlayersBat     # BB:84,L:cdc,H:ce4,R_MIPS_26,RestartPlayersBat
 000ce0  00000000   nop                              
                                                     # Line number: 1394
$158:                                                # BB:85,L:ce4,H:db0
                                                     # Line number: 1397
 000ce4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000ce8  8c290000   lw         $t1,0($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000cec  00000000   nop                              
 000cf0  00095400   sll        $t2,$t1,16            
 000cf4  000a5c03   sra        $t3,$t2,16            
 000cf8  216cffff   addi       $t4,$t3,-1            
 000cfc  000c6c00   sll        $t5,$t4,16            
 000d00  000d7403   sra        $t6,$t5,16            
 000d04  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000d08  a42e0004   sh         $t6,4($at)            # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1398
 000d0c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000d10  8c280004   lw         $t0,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000d14  00000000   nop                              
 000d18  00084c00   sll        $t1,$t0,16            
 000d1c  00095403   sra        $t2,$t1,16            
 000d20  214bffff   addi       $t3,$t2,-1            
 000d24  000b6400   sll        $t4,$t3,16            
 000d28  000c6c03   sra        $t5,$t4,16            
 000d2c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsSprite
 000d30  a42d0006   sh         $t5,6($at)            # BallsSprite,R_MIPS_LO16,BallsSprite
                                                     # Line number: 1405
 000d34  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d38  8c2e0000   lw         $t6,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000d3c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d40  8c280010   lw         $t0,16($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000d44  00000000   nop                              
 000d48  01c84820   add        $t1,$t6,$t0           
 000d4c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d50  ac290000   sw         $t1,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1406
 000d54  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d58  8c2a0004   lw         $t2,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000d5c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d60  8c2b0014   lw         $t3,20($at)           # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000d64  00000000   nop                              
 000d68  014b6020   add        $t4,$t2,$t3           
 000d6c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d70  ac2c0004   sw         $t4,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1409
 000d74  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d78  ac200010   sw         $0,16($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1410
 000d7c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d80  ac200014   sw         $0,20($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1413
 000d84  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d88  8c2d0008   lw         $t5,8($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000d8c  00000000   nop                              
 000d90  21aefee0   addi       $t6,$t5,-288          
 000d94  000e4022   neg        $t0,$t6               
 000d98  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000d9c  8c290000   lw         $t1,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000da0  00000000   nop                              
 000da4  0109082a   slt        $at,$t0,$t1           
 000da8  1020000b   beq        $at,$0,$159           
 000dac  00000000   nop                              
                                                     # Line number: 1415
 000db0  3c010000   lui        $at,0x0000            # BB:86,L:db0,H:dd8,R_MIPS_HI16,PlayersMovement
 000db4  8c2a0008   lw         $t2,8($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000db8  00000000   nop                              
 000dbc  214bfee0   addi       $t3,$t2,-288          
 000dc0  000b6022   neg        $t4,$t3               
 000dc4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000dc8  ac2c0000   sw         $t4,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1416
 000dcc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
                                                     # Line number: 1417
 000dd0  1000000c   b          $160                  
 000dd4  ac200010   sw         $0,16($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
$159:                                                # BB:87,L:dd8,H:df0
                                                     # Line number: 1418
 000dd8  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000ddc  8c2d0000   lw         $t5,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000de0  00000000   nop                              
 000de4  29a10010   slti       $at,$t5,16            
 000de8  10200006   beq        $at,$0,$160           
 000dec  00000000   nop                              
                                                     # Line number: 1420
 000df0  240e0010   addiu      $t6,$0,16             # BB:88,L:df0,H:e04
 000df4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000df8  ac2e0000   sw         $t6,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1421
 000dfc  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000e00  ac200010   sw         $0,16($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
                                                     # Line number: 1422
$160:                                                # BB:89,L:e04,H:e04
$160:                                                # BB:90,L:e04,H:e4c
                                                     # Line number: 1425
 000e04  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000e08  8c280000   lw         $t0,0($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000e0c  00000000   nop                              
 000e10  00084c00   sll        $t1,$t0,16            
 000e14  00095403   sra        $t2,$t1,16            
 000e18  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000e1c  a42a0004   sh         $t2,4($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1426
 000e20  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 000e24  8c2b0004   lw         $t3,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000e28  00000000   nop                              
 000e2c  000b6400   sll        $t4,$t3,16            
 000e30  000c6c03   sra        $t5,$t4,16            
 000e34  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersSprite
 000e38  a42d0006   sh         $t5,6($at)            # PlayersSprite,R_MIPS_LO16,PlayersSprite
                                                     # Line number: 1427
 000e3c  8fbf003c   lw         $ra,60($sp)           
 000e40  00000000   nop                              
 000e44  03e00008   jr         $ra                   
 000e48  27bd0080   addiu      $sp,$sp,128           
                    .end       HandleAllObjects      
 # *****************************************

                    .text                            # BB:0,L:0,H:30
                    .globl     MoveBallOutOfBat      
                    .ent       MoveBallOutOfBat      
MoveBallOutOfBat:                                    # CS:88,ETS:0
                                                     # Line number: 1434
                                                     # Line number: 1435
 000000  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 000004  8c280004   lw         $t0,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
 000008  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00000c  8c29000c   lw         $t1,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000010  00000000   nop                              
 000014  01095020   add        $t2,$t0,$t1           
 000018  3c010000   lui        $at,0x0000            # R_MIPS_HI16,PlayersMovement
 00001c  8c2b0004   lw         $t3,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000020  00000000   nop                              
 000024  016a082a   slt        $at,$t3,$t2           
 000028  10200009   beq        $at,$0,$100           
 00002c  00000000   nop                              
                                                     # Line number: 1438
 000030  3c010000   lui        $at,0x0000            # BB:1,L:30,H:50,R_MIPS_HI16,PlayersMovement
 000034  8c2c0004   lw         $t4,4($at)            # PlayersMovement,R_MIPS_LO16,PlayersMovement
 000038  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00003c  8c2d000c   lw         $t5,12($at)           # BallsMovement,R_MIPS_LO16,BallsMovement
 000040  00000000   nop                              
 000044  018d7022   sub        $t6,$t4,$t5           
 000048  3c010000   lui        $at,0x0000            # R_MIPS_HI16,BallsMovement
 00004c  ac2e0004   sw         $t6,4($at)            # BallsMovement,R_MIPS_LO16,BallsMovement
                                                     # Line number: 1440
$100:                                                # BB:2,L:50,H:58
                                                     # Line number: 1441
 000050  03e00008   jr         $ra                   
 000054  00000000   nop                              
                    .end       MoveBallOutOfBat      
 # *****************************************

                    .text                            # BB:0,L:0,H:20
                    .globl     RectanglesOverlap     
                    .ent       RectanglesOverlap     
RectanglesOverlap:                                   # CS:88,ETS:0
                                                     # Line number: 1452
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
 000008  afa40050   sw         $a0,80($sp)           # first
 00000c  afa50054   sw         $a1,84($sp)           # second
                                                     # Line number: 1455
 000010  8fa40050   lw         $a0,80($sp)           # first
 000014  8fa50054   lw         $a1,84($sp)           # second
 000018  0c000000   jal        XOverlap              # R_MIPS_26,XOverlap
 00001c  00000000   nop                              
 000020  0002402b   sltu       $t0,$0,$v0            # BB:1,L:20,H:2c
 000024  11000006   beq        $t0,$0,$100           
 000028  0100c821   move       $t9,$t0               
 00002c  8fa40050   lw         $a0,80($sp)           # BB:2,L:2c,H:3c,first
 000030  8fa50054   lw         $a1,84($sp)           # second
 000034  0c000000   jal        YOverlap              # R_MIPS_26,YOverlap
 000038  00000000   nop                              
 00003c  0002c82b   sltu       $t9,$0,$v0            # BB:3,L:3c,H:40
$100:                                                # BB:4,L:40,H:58
 000040  afb9004c   sw         $t9,76($sp)           # boolean
                                                     # Line number: 1457
 000044  8fa2004c   lw         $v0,76($sp)           # boolean
                                                     # Line number: 1458
 000048  8fbf003c   lw         $ra,60($sp)           
 00004c  00000000   nop                              
 000050  03e00008   jr         $ra                   
 000054  27bd0050   addiu      $sp,$sp,80            
                    .end       RectanglesOverlap     
 # *****************************************

                    .text                            # BB:0,L:0,H:18
                    .globl     XOverlap              
                    .ent       XOverlap              
XOverlap:                                            # CS:88,ETS:0
                                                     # Line number: 1462
                                                     # Line number: 1463
 000000  84880000   lh         $t0,0($a0)            
 000004  84a90000   lh         $t1,0($a1)            
 000008  00000000   nop                              
 00000c  0109082a   slt        $at,$t0,$t1           
 000010  14200008   bne        $at,$0,$100           
 000014  00000000   nop                              
                                                     # Line number: 1465
 000018  84aa0004   lh         $t2,4($a1)            # BB:1,L:18,H:34
 00001c  84ab0000   lh         $t3,0($a1)            
 000020  00000000   nop                              
 000024  016a6020   add        $t4,$t3,$t2           
 000028  848d0000   lh         $t5,0($a0)            
 00002c  10000008   b          $101                  
 000030  01ac102a   slt        $v0,$t5,$t4           
$100:                                                # BB:2,L:34,H:50
                                                     # Line number: 1469
 000034  848e0004   lh         $t6,4($a0)            
 000038  84880000   lh         $t0,0($a0)            
 00003c  00000000   nop                              
 000040  010e4820   add        $t1,$t0,$t6           
 000044  84aa0000   lh         $t2,0($a1)            
 000048  00000000   nop                              
 00004c  0149102a   slt        $v0,$t2,$t1           
$101:                                                # BB:3,L:50,H:58
                                                     # Line number: 1471
 000050  03e00008   jr         $ra                   
 000054  00000000   nop                              
                    .end       XOverlap              
 # *****************************************

                    .text                            # BB:0,L:0,H:18
                    .globl     YOverlap              
                    .ent       YOverlap              
YOverlap:                                            # CS:88,ETS:0
                                                     # Line number: 1475
                                                     # Line number: 1476
 000000  84880002   lh         $t0,2($a0)            
 000004  84a90002   lh         $t1,2($a1)            
 000008  00000000   nop                              
 00000c  0109082a   slt        $at,$t0,$t1           
 000010  14200008   bne        $at,$0,$100           
 000014  00000000   nop                              
                                                     # Line number: 1478
 000018  84aa0002   lh         $t2,2($a1)            # BB:1,L:18,H:34
 00001c  84ab0006   lh         $t3,6($a1)            
 000020  00000000   nop                              
 000024  014b6020   add        $t4,$t2,$t3           
 000028  848d0002   lh         $t5,2($a0)            
 00002c  10000008   b          $101                  
 000030  01ac102a   slt        $v0,$t5,$t4           
$100:                                                # BB:2,L:34,H:50
                                                     # Line number: 1482
 000034  848e0002   lh         $t6,2($a0)            
 000038  84880006   lh         $t0,6($a0)            
 00003c  00000000   nop                              
 000040  01c84820   add        $t1,$t6,$t0           
 000044  84aa0002   lh         $t2,2($a1)            
 000048  00000000   nop                              
 00004c  0149102a   slt        $v0,$t2,$t1           
$101:                                                # BB:3,L:50,H:58
                                                     # Line number: 1484
 000050  03e00008   jr         $ra                   
 000054  00000000   nop                              
                    .end       YOverlap              
 # *****************************************

                    .text                            # BB:0,L:0,H:20
                    .globl     RectanglesTouch       
                    .ent       RectanglesTouch       
RectanglesTouch:                                     # CS:148,ETS:0
                                                     # Line number: 1488
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
 000008  afa40050   sw         $a0,80($sp)           # first
 00000c  afa50054   sw         $a1,84($sp)           # second
                                                     # Line number: 1491
 000010  8fa40050   lw         $a0,80($sp)           # first
 000014  8fa50054   lw         $a1,84($sp)           # second
 000018  0c000000   jal        XTouch                # R_MIPS_26,XTouch
 00001c  00000000   nop                              
 000020  0002402b   sltu       $t0,$0,$v0            # BB:1,L:20,H:2c
 000024  11000006   beq        $t0,$0,$100           
 000028  0100c821   move       $t9,$t0               
 00002c  8fa40050   lw         $a0,80($sp)           # BB:2,L:2c,H:3c,first
 000030  8fa50054   lw         $a1,84($sp)           # second
 000034  0c000000   jal        YOverlap              # R_MIPS_26,YOverlap
 000038  00000000   nop                              
 00003c  0002c82b   sltu       $t9,$0,$v0            # BB:3,L:3c,H:40
$100:                                                # BB:4,L:40,H:4c
 000040  03204821   move       $t1,$t9               
 000044  1520000d   bne        $t1,$0,$101           
 000048  0120c821   move       $t9,$t1               
 00004c  8fa40050   lw         $a0,80($sp)           # BB:5,L:4c,H:5c,first
 000050  8fa50054   lw         $a1,84($sp)           # second
 000054  0c000000   jal        YTouch                # R_MIPS_26,YTouch
 000058  00000000   nop                              
 00005c  0002502b   sltu       $t2,$0,$v0            # BB:6,L:5c,H:68
 000060  11400006   beq        $t2,$0,$102           
 000064  0140c821   move       $t9,$t2               
 000068  8fa40050   lw         $a0,80($sp)           # BB:7,L:68,H:78,first
 00006c  8fa50054   lw         $a1,84($sp)           # second
 000070  0c000000   jal        XOverlap              # R_MIPS_26,XOverlap
 000074  00000000   nop                              
 000078  0002c82b   sltu       $t9,$0,$v0            # BB:8,L:78,H:7c
$102:                                                # BB:9,L:7c,H:7c
$101:                                                # BB:10,L:7c,H:94
 00007c  afb9004c   sw         $t9,76($sp)           # boolean
                                                     # Line number: 1494
 000080  8fa2004c   lw         $v0,76($sp)           # boolean
                                                     # Line number: 1495
 000084  8fbf003c   lw         $ra,60($sp)           
 000088  00000000   nop                              
 00008c  03e00008   jr         $ra                   
 000090  27bd0050   addiu      $sp,$sp,80            
                    .end       RectanglesTouch       
 # *****************************************

                    .text                            # BB:0,L:0,H:18
                    .globl     XTouch                
                    .ent       XTouch                
XTouch:                                              # CS:128,ETS:0
                                                     # Line number: 1499
                                                     # Line number: 1500
 000000  84880000   lh         $t0,0($a0)            
 000004  84a90000   lh         $t1,0($a1)            
 000008  00000000   nop                              
 00000c  0128082a   slt        $at,$t1,$t0           
 000010  1020000a   beq        $at,$0,$100           
 000014  00000000   nop                              
                                                     # Line number: 1502
 000018  84aa0004   lh         $t2,4($a1)            # BB:1,L:18,H:3c
 00001c  84ab0000   lh         $t3,0($a1)            
 000020  00000000   nop                              
 000024  016a6020   add        $t4,$t3,$t2           
 000028  848d0000   lh         $t5,0($a0)            
 00002c  00000000   nop                              
 000030  01ac1026   xor        $v0,$t5,$t4           
 000034  10000010   b          $101                  
 000038  2c420001   sltiu      $v0,$v0,1             
$100:                                                # BB:2,L:3c,H:50
                                                     # Line number: 1504
 00003c  848e0000   lh         $t6,0($a0)            
 000040  84a80000   lh         $t0,0($a1)            
 000044  00000000   nop                              
 000048  15c80003   bne        $t6,$t0,$102          
 00004c  00000000   nop                              
                                                     # Line number: 1505
 000050  10000009   b          $101                  # BB:3,L:50,H:58
 000054  00001021   move       $v0,$0                
$102:                                                # BB:4,L:58,H:78
                                                     # Line number: 1508
 000058  84890004   lh         $t1,4($a0)            
 00005c  848a0000   lh         $t2,0($a0)            
 000060  00000000   nop                              
 000064  01495820   add        $t3,$t2,$t1           
 000068  84ac0000   lh         $t4,0($a1)            
 00006c  00000000   nop                              
 000070  018b1026   xor        $v0,$t4,$t3           
 000074  2c420001   sltiu      $v0,$v0,1             
$101:                                                # BB:5,L:78,H:80
                                                     # Line number: 1510
 000078  03e00008   jr         $ra                   
 00007c  00000000   nop                              
                    .end       XTouch                
 # *****************************************

                    .text                            # BB:0,L:0,H:18
                    .globl     YTouch                
                    .ent       YTouch                
YTouch:                                              # CS:128,ETS:0
                                                     # Line number: 1515
                                                     # Line number: 1516
 000000  84880002   lh         $t0,2($a0)            
 000004  84a90002   lh         $t1,2($a1)            
 000008  00000000   nop                              
 00000c  0128082a   slt        $at,$t1,$t0           
 000010  1020000a   beq        $at,$0,$100           
 000014  00000000   nop                              
                                                     # Line number: 1518
 000018  84aa0002   lh         $t2,2($a1)            # BB:1,L:18,H:3c
 00001c  84ab0006   lh         $t3,6($a1)            
 000020  00000000   nop                              
 000024  014b6020   add        $t4,$t2,$t3           
 000028  848d0002   lh         $t5,2($a0)            
 00002c  00000000   nop                              
 000030  01ac1026   xor        $v0,$t5,$t4           
 000034  10000010   b          $101                  
 000038  2c420001   sltiu      $v0,$v0,1             
$100:                                                # BB:2,L:3c,H:50
                                                     # Line number: 1520
 00003c  848e0002   lh         $t6,2($a0)            
 000040  84a80002   lh         $t0,2($a1)            
 000044  00000000   nop                              
 000048  15c80003   bne        $t6,$t0,$102          
 00004c  00000000   nop                              
                                                     # Line number: 1521
 000050  10000009   b          $101                  # BB:3,L:50,H:58
 000054  00001021   move       $v0,$0                
$102:                                                # BB:4,L:58,H:78
                                                     # Line number: 1524
 000058  84890002   lh         $t1,2($a0)            
 00005c  848a0006   lh         $t2,6($a0)            
 000060  00000000   nop                              
 000064  012a5820   add        $t3,$t1,$t2           
 000068  84ac0002   lh         $t4,2($a1)            
 00006c  00000000   nop                              
 000070  018b1026   xor        $v0,$t4,$t3           
 000074  2c420001   sltiu      $v0,$v0,1             
$101:                                                # BB:5,L:78,H:80
                                                     # Line number: 1526
 000078  03e00008   jr         $ra                   
 00007c  00000000   nop                              
                    .end       YTouch                
 # *****************************************

                    .text                            # BB:0,L:0,H:1c
                    .globl     FindWhereRectanglesTouch
                    .ent       FindWhereRectanglesTouch
FindWhereRectanglesTouch:                            # CS:896,ETS:0
                                                     # Line number: 1537
 000000  27bdfff0   addiu      $sp,$sp,-16           
                                                     # Line number: 1543
 000004  84880000   lh         $t0,0($a0)            
 000008  84a90000   lh         $t1,0($a1)            
 00000c  00000000   nop                              
 000010  0128082a   slt        $at,$t1,$t0           
 000014  1020001b   beq        $at,$0,$100           
 000018  00000000   nop                              
                                                     # Line number: 1545
 00001c  84aa0004   lh         $t2,4($a1)            # BB:1,L:1c,H:3c
 000020  84ab0000   lh         $t3,0($a1)            
 000024  00000000   nop                              
 000028  016a6020   add        $t4,$t3,$t2           
 00002c  848d0000   lh         $t5,0($a0)            
 000030  00000000   nop                              
 000034  15ac0004   bne        $t5,$t4,$101          
 000038  00000000   nop                              
                                                     # Line number: 1546
 00003c  240e0003   addiu      $t6,$0,3              # BB:2,L:3c,H:48
 000040  10000031   b          $102                  
 000044  afae0008   sw         $t6,8($sp)            # xTouch
$101:                                                # BB:3,L:48,H:6c
                                                     # Line number: 1547
 000048  84a80004   lh         $t0,4($a1)            
 00004c  84a90000   lh         $t1,0($a1)            
 000050  00000000   nop                              
 000054  01285020   add        $t2,$t1,$t0           
 000058  848b0000   lh         $t3,0($a0)            
 00005c  00000000   nop                              
 000060  016a082a   slt        $at,$t3,$t2           
 000064  10200004   beq        $at,$0,$103           
 000068  00000000   nop                              
                                                     # Line number: 1548
 00006c  240c0009   addiu      $t4,$0,9              # BB:4,L:6c,H:78
 000070  10000025   b          $102                  
 000074  afac0008   sw         $t4,8($sp)            # xTouch
$103:                                                # BB:5,L:78,H:84
                                                     # Line number: 1550
 000078  240d7ffe   addiu      $t5,$0,32766          
                                                     # Line number: 1551
 00007c  10000022   b          $102                  
 000080  afad0008   sw         $t5,8($sp)            # xTouch
$100:                                                # BB:6,L:84,H:98
                                                     # Line number: 1552
 000084  848e0000   lh         $t6,0($a0)            
 000088  84a80000   lh         $t0,0($a1)            
 00008c  00000000   nop                              
 000090  15c80004   bne        $t6,$t0,$104          
 000094  00000000   nop                              
                                                     # Line number: 1553
 000098  24090009   addiu      $t1,$0,9              # BB:7,L:98,H:a4
 00009c  1000001a   b          $102                  
 0000a0  afa90008   sw         $t1,8($sp)            # xTouch
$104:                                                # BB:8,L:a4,H:c4
                                                     # Line number: 1556
 0000a4  848a0004   lh         $t2,4($a0)            
 0000a8  848b0000   lh         $t3,0($a0)            
 0000ac  00000000   nop                              
 0000b0  016a6020   add        $t4,$t3,$t2           
 0000b4  84ad0000   lh         $t5,0($a1)            
 0000b8  00000000   nop                              
 0000bc  15ac0004   bne        $t5,$t4,$106          
 0000c0  00000000   nop                              
                                                     # Line number: 1557
 0000c4  240e0004   addiu      $t6,$0,4              # BB:9,L:c4,H:d0
 0000c8  1000000f   b          $107                  
 0000cc  afae0008   sw         $t6,8($sp)            # xTouch
$106:                                                # BB:10,L:d0,H:f4
                                                     # Line number: 1558
 0000d0  84880004   lh         $t0,4($a0)            
 0000d4  84890000   lh         $t1,0($a0)            
 0000d8  00000000   nop                              
 0000dc  01285020   add        $t2,$t1,$t0           
 0000e0  84ab0000   lh         $t3,0($a1)            
 0000e4  00000000   nop                              
 0000e8  016a082a   slt        $at,$t3,$t2           
 0000ec  10200004   beq        $at,$0,$108           
 0000f0  00000000   nop                              
                                                     # Line number: 1559
 0000f4  240c0009   addiu      $t4,$0,9              # BB:11,L:f4,H:100
 0000f8  10000003   b          $107                  
 0000fc  afac0008   sw         $t4,8($sp)            # xTouch
$108:                                                # BB:12,L:100,H:108
                                                     # Line number: 1561
 000100  240d7ffe   addiu      $t5,$0,32766          
 000104  afad0008   sw         $t5,8($sp)            # xTouch
$107:                                                # BB:13,L:108,H:108
$107:                                                # BB:14,L:108,H:108
                                                     # Line number: 1562
$102:                                                # BB:15,L:108,H:108
$102:                                                # BB:16,L:108,H:120
                                                     # Line number: 1564
 000108  848e0002   lh         $t6,2($a0)            
 00010c  84a80002   lh         $t0,2($a1)            
 000110  00000000   nop                              
 000114  010e082a   slt        $at,$t0,$t6           
 000118  1020001b   beq        $at,$0,$110           
 00011c  00000000   nop                              
                                                     # Line number: 1566
 000120  84a90002   lh         $t1,2($a1)            # BB:17,L:120,H:140
 000124  84aa0006   lh         $t2,6($a1)            
 000128  00000000   nop                              
 00012c  012a5820   add        $t3,$t1,$t2           
 000130  848c0002   lh         $t4,2($a0)            
 000134  00000000   nop                              
 000138  158b0004   bne        $t4,$t3,$111          
 00013c  00000000   nop                              
                                                     # Line number: 1567
 000140  240d0001   addiu      $t5,$0,1              # BB:18,L:140,H:14c
 000144  10000031   b          $112                  
 000148  afad000c   sw         $t5,12($sp)           # yTouch
$111:                                                # BB:19,L:14c,H:170
                                                     # Line number: 1568
 00014c  84ae0002   lh         $t6,2($a1)            
 000150  84a80006   lh         $t0,6($a1)            
 000154  00000000   nop                              
 000158  01c84820   add        $t1,$t6,$t0           
 00015c  848a0002   lh         $t2,2($a0)            
 000160  00000000   nop                              
 000164  0149082a   slt        $at,$t2,$t1           
 000168  10200004   beq        $at,$0,$113           
 00016c  00000000   nop                              
                                                     # Line number: 1569
 000170  240b0009   addiu      $t3,$0,9              # BB:20,L:170,H:17c
 000174  10000025   b          $112                  
 000178  afab000c   sw         $t3,12($sp)           # yTouch
$113:                                                # BB:21,L:17c,H:188
                                                     # Line number: 1571
 00017c  240c7ffe   addiu      $t4,$0,32766          
                                                     # Line number: 1572
 000180  10000022   b          $112                  
 000184  afac000c   sw         $t4,12($sp)           # yTouch
$110:                                                # BB:22,L:188,H:19c
                                                     # Line number: 1573
 000188  848d0002   lh         $t5,2($a0)            
 00018c  84ae0002   lh         $t6,2($a1)            
 000190  00000000   nop                              
 000194  15ae0004   bne        $t5,$t6,$114          
 000198  00000000   nop                              
                                                     # Line number: 1574
 00019c  24080009   addiu      $t0,$0,9              # BB:23,L:19c,H:1a8
 0001a0  1000001a   b          $112                  
 0001a4  afa8000c   sw         $t0,12($sp)           # yTouch
$114:                                                # BB:24,L:1a8,H:1c8
                                                     # Line number: 1577
 0001a8  84890002   lh         $t1,2($a0)            
 0001ac  848a0006   lh         $t2,6($a0)            
 0001b0  00000000   nop                              
 0001b4  012a5820   add        $t3,$t1,$t2           
 0001b8  84ac0002   lh         $t4,2($a1)            
 0001bc  00000000   nop                              
 0001c0  158b0004   bne        $t4,$t3,$116          
 0001c4  00000000   nop                              
                                                     # Line number: 1578
 0001c8  240d0002   addiu      $t5,$0,2              # BB:25,L:1c8,H:1d4
 0001cc  1000000f   b          $117                  
 0001d0  afad000c   sw         $t5,12($sp)           # yTouch
$116:                                                # BB:26,L:1d4,H:1f8
                                                     # Line number: 1579
 0001d4  848e0002   lh         $t6,2($a0)            
 0001d8  84880006   lh         $t0,6($a0)            
 0001dc  00000000   nop                              
 0001e0  01c84820   add        $t1,$t6,$t0           
 0001e4  84aa0002   lh         $t2,2($a1)            
 0001e8  00000000   nop                              
 0001ec  0149082a   slt        $at,$t2,$t1           
 0001f0  10200004   beq        $at,$0,$118           
 0001f4  00000000   nop                              
                                                     # Line number: 1580
 0001f8  240b0009   addiu      $t3,$0,9              # BB:27,L:1f8,H:204
 0001fc  10000003   b          $117                  
 000200  afab000c   sw         $t3,12($sp)           # yTouch
$118:                                                # BB:28,L:204,H:20c
                                                     # Line number: 1582
 000204  240c7ffe   addiu      $t4,$0,32766          
 000208  afac000c   sw         $t4,12($sp)           # yTouch
$117:                                                # BB:29,L:20c,H:20c
$117:                                                # BB:30,L:20c,H:20c
                                                     # Line number: 1583
$112:                                                # BB:31,L:20c,H:20c
$112:                                                # BB:32,L:20c,H:21c
                                                     # Line number: 1585
 00020c  8fad0008   lw         $t5,8($sp)            # xTouch
 000210  24017ffe   addiu      $at,$0,32766          
 000214  11a10055   beq        $t5,$at,$120          
 000218  00000000   nop                              
 00021c  24010009   addiu      $at,$0,9              # BB:33,L:21c,H:228
 000220  11a10039   beq        $t5,$at,$121          
 000224  00000000   nop                              
 000228  24010004   addiu      $at,$0,4              # BB:34,L:228,H:234
 00022c  11a1001d   beq        $t5,$at,$122          
 000230  00000000   nop                              
 000234  24010003   addiu      $at,$0,3              # BB:35,L:234,H:240
 000238  15a1004e   bne        $t5,$at,$124          
 00023c  00000000   nop                              
                                                     # Line number: 1588
 000240  8fae000c   lw         $t6,12($sp)           # BB:36,L:240,H:250,yTouch
 000244  24017ffe   addiu      $at,$0,32766          
 000248  11c10013   beq        $t6,$at,$125          
 00024c  00000000   nop                              
 000250  24010009   addiu      $at,$0,9              # BB:37,L:250,H:25c
 000254  11c1000d   beq        $t6,$at,$126          
 000258  00000000   nop                              
 00025c  24010002   addiu      $at,$0,2              # BB:38,L:25c,H:268
 000260  11c10007   beq        $t6,$at,$127          
 000264  00000000   nop                              
 000268  24010001   addiu      $at,$0,1              # BB:39,L:268,H:274
 00026c  15c10041   bne        $t6,$at,$124          
 000270  00000000   nop                              
                                                     # Line number: 1591
 000274  24080005   addiu      $t0,$0,5              # BB:40,L:274,H:280
                                                     # Line number: 1592
 000278  1000003e   b          $124                  
 00027c  afa80004   sw         $t0,4($sp)            # whereTheyTouch
$127:                                                # BB:41,L:280,H:28c
                                                     # Line number: 1594
 000280  24090007   addiu      $t1,$0,7              
                                                     # Line number: 1595
 000284  1000003b   b          $124                  
 000288  afa90004   sw         $t1,4($sp)            # whereTheyTouch
$126:                                                # BB:42,L:28c,H:298
                                                     # Line number: 1597
 00028c  240a0003   addiu      $t2,$0,3              
                                                     # Line number: 1598
 000290  10000038   b          $124                  
 000294  afaa0004   sw         $t2,4($sp)            # whereTheyTouch
$125:                                                # BB:43,L:298,H:2a4
                                                     # Line number: 1600
 000298  240b7ffe   addiu      $t3,$0,32766          
                                                     # Line number: 1603
 00029c  10000035   b          $124                  
 0002a0  afab0004   sw         $t3,4($sp)            # whereTheyTouch
$122:                                                # BB:44,L:2a4,H:2b4
                                                     # Line number: 1605
 0002a4  8fac000c   lw         $t4,12($sp)           # yTouch
 0002a8  24017ffe   addiu      $at,$0,32766          
 0002ac  11810013   beq        $t4,$at,$129          
 0002b0  00000000   nop                              
 0002b4  24010009   addiu      $at,$0,9              # BB:45,L:2b4,H:2c0
 0002b8  1181000d   beq        $t4,$at,$130          
 0002bc  00000000   nop                              
 0002c0  24010002   addiu      $at,$0,2              # BB:46,L:2c0,H:2cc
 0002c4  11810007   beq        $t4,$at,$131          
 0002c8  00000000   nop                              
 0002cc  24010001   addiu      $at,$0,1              # BB:47,L:2cc,H:2d8
 0002d0  15810028   bne        $t4,$at,$124          
 0002d4  00000000   nop                              
                                                     # Line number: 1608
 0002d8  240d0006   addiu      $t5,$0,6              # BB:48,L:2d8,H:2e4
                                                     # Line number: 1609
 0002dc  10000025   b          $124                  
 0002e0  afad0004   sw         $t5,4($sp)            # whereTheyTouch
$131:                                                # BB:49,L:2e4,H:2f0
                                                     # Line number: 1611
 0002e4  240e0008   addiu      $t6,$0,8              
                                                     # Line number: 1612
 0002e8  10000022   b          $124                  
 0002ec  afae0004   sw         $t6,4($sp)            # whereTheyTouch
$130:                                                # BB:50,L:2f0,H:2fc
                                                     # Line number: 1614
 0002f0  24080004   addiu      $t0,$0,4              
                                                     # Line number: 1615
 0002f4  1000001f   b          $124                  
 0002f8  afa80004   sw         $t0,4($sp)            # whereTheyTouch
$129:                                                # BB:51,L:2fc,H:308
                                                     # Line number: 1617
 0002fc  24097ffe   addiu      $t1,$0,32766          
                                                     # Line number: 1620
 000300  1000001c   b          $124                  
 000304  afa90004   sw         $t1,4($sp)            # whereTheyTouch
$121:                                                # BB:52,L:308,H:318
                                                     # Line number: 1622
 000308  8faa000c   lw         $t2,12($sp)           # yTouch
 00030c  24017ffe   addiu      $at,$0,32766          
 000310  11410013   beq        $t2,$at,$133          
 000314  00000000   nop                              
 000318  24010009   addiu      $at,$0,9              # BB:53,L:318,H:324
 00031c  1141000d   beq        $t2,$at,$134          
 000320  00000000   nop                              
 000324  24010002   addiu      $at,$0,2              # BB:54,L:324,H:330
 000328  11410007   beq        $t2,$at,$135          
 00032c  00000000   nop                              
 000330  24010001   addiu      $at,$0,1              # BB:55,L:330,H:33c
 000334  1541000f   bne        $t2,$at,$124          
 000338  00000000   nop                              
                                                     # Line number: 1625
 00033c  240b0001   addiu      $t3,$0,1              # BB:56,L:33c,H:348
                                                     # Line number: 1626
 000340  1000000c   b          $124                  
 000344  afab0004   sw         $t3,4($sp)            # whereTheyTouch
$135:                                                # BB:57,L:348,H:354
                                                     # Line number: 1628
 000348  240c0002   addiu      $t4,$0,2              
                                                     # Line number: 1629
 00034c  10000009   b          $124                  
 000350  afac0004   sw         $t4,4($sp)            # whereTheyTouch
$134:                                                # BB:58,L:354,H:360
                                                     # Line number: 1631
 000354  240d000a   addiu      $t5,$0,10             
                                                     # Line number: 1632
 000358  10000006   b          $124                  
 00035c  afad0004   sw         $t5,4($sp)            # whereTheyTouch
$133:                                                # BB:59,L:360,H:36c
                                                     # Line number: 1634
 000360  240e7ffe   addiu      $t6,$0,32766          
                                                     # Line number: 1637
 000364  10000003   b          $124                  
 000368  afae0004   sw         $t6,4($sp)            # whereTheyTouch
$120:                                                # BB:60,L:36c,H:374
                                                     # Line number: 1639
 00036c  24087ffe   addiu      $t0,$0,32766          
 000370  afa80004   sw         $t0,4($sp)            # whereTheyTouch
                                                     # Line number: 1641
$124:                                                # BB:61,L:374,H:380
                                                     # Line number: 1643
 000374  8fa20004   lw         $v0,4($sp)            # whereTheyTouch
                                                     # Line number: 1644
 000378  03e00008   jr         $ra                   
 00037c  27bd0010   addiu      $sp,$sp,16            
                    .end       FindWhereRectanglesTouch
 # *****************************************

                    .text                            # BB:0,L:0,H:1c
                    .globl     PadInit               
                    .ent       PadInit               
PadInit:                                             # CS:44,ETS:0
                                                     # Line number: 1652
 000000  27bdffc0   addiu      $sp,$sp,-64           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1653
 000008  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,bb0
 00000c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,bb0
 000010  3c050000   lui        $a1,0x0000            # R_MIPS_HI16,bb1
 000014  0c000000   jal        GetPadBuf             # R_MIPS_26,GetPadBuf
 000018  20a50000   addi       $a1,$a1,0             # R_MIPS_LO16,bb1
                                                     # Line number: 1654
 00001c  8fbf003c   lw         $ra,60($sp)           # BB:1,L:1c,H:2c
 000020  00000000   nop                              
 000024  03e00008   jr         $ra                   
 000028  27bd0040   addiu      $sp,$sp,64            
                    .end       PadInit               
 # *****************************************

                    .text                            # BB:0,L:0,H:6c
                    .globl     PadRead               
                    .ent       PadRead               
PadRead:                                             # CS:108,ETS:0
                                                     # Line number: 1662
                                                     # Line number: 1663
 000000  3c010000   lui        $at,0x0000            # R_MIPS_HI16,bb0
 000004  8c280000   lw         $t0,0($at)            # bb0,R_MIPS_LO16,bb0
 000008  00000000   nop                              
 00000c  91090002   lbu        $t1,2($t0)            
 000010  00000000   nop                              
 000014  00095200   sll        $t2,$t1,8             
 000018  3c010000   lui        $at,0x0000            # R_MIPS_HI16,bb0
 00001c  8c2b0000   lw         $t3,0($at)            # bb0,R_MIPS_LO16,bb0
 000020  00000000   nop                              
 000024  916c0003   lbu        $t4,3($t3)            
 000028  00000000   nop                              
 00002c  018a6825   or         $t5,$t4,$t2           
 000030  3c010000   lui        $at,0x0000            # R_MIPS_HI16,bb1
 000034  8c2e0000   lw         $t6,0($at)            # bb1,R_MIPS_LO16,bb1
 000038  00000000   nop                              
 00003c  91c80003   lbu        $t0,3($t6)            
 000040  00000000   nop                              
 000044  00084c00   sll        $t1,$t0,16            
 000048  012d5025   or         $t2,$t1,$t5           
 00004c  3c010000   lui        $at,0x0000            # R_MIPS_HI16,bb1
 000050  8c2b0000   lw         $t3,0($at)            # bb1,R_MIPS_LO16,bb1
 000054  00000000   nop                              
 000058  916c0002   lbu        $t4,2($t3)            
 00005c  00000000   nop                              
 000060  000c6e00   sll        $t5,$t4,24            
                                                     # Line number: 1664
 000064  03e00008   jr         $ra                   
 000068  01aa1027   nor        $v0,$t5,$t2           
                    .end       PadRead               
 # *****************************************

                    .text                            # BB:0,L:0,H:1c
                    .globl     InitSound             
                    .ent       InitSound             
InitSound:                                           # CS:204,ETS:0
                                                     # Line number: 1682
 000000  27bdffc0   addiu      $sp,$sp,-64           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1683
 000008  3c048009   lui        $a0,0x8009            
 00000c  3c05800a   lui        $a1,0x800a            
 000010  2406ffff   addiu      $a2,$0,-1             
 000014  0c000000   jal        SsVabTransfer         # R_MIPS_26,SsVabTransfer
 000018  24070001   addiu      $a3,$0,1              
 00001c  3c010000   lui        $at,0x0000            # BB:1,L:1c,H:3c,R_MIPS_HI16,vab
 000020  a4220000   sh         $v0,0($at)            # vab,R_MIPS_LO16,vab
                                                     # Line number: 1684
 000024  3c010000   lui        $at,0x0000            # R_MIPS_HI16,vab
 000028  84280000   lh         $t0,0($at)            # vab,R_MIPS_LO16,vab
 00002c  00000000   nop                              
 000030  0100082a   slt        $at,$t0,$0            
 000034  10200009   beq        $at,$0,$100           
 000038  00000000   nop                              
                                                     # Line number: 1686
 00003c  3c010000   lui        $at,0x0000            # BB:2,L:3c,H:50,R_MIPS_HI16,vab
 000040  84250000   lh         $a1,0($at)            # vab,R_MIPS_LO16,vab
 000044  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@440
 000048  0c000000   jal        printf                # R_MIPS_26,printf
 00004c  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@440
                                                     # Line number: 1687
 000050  3c010000   lui        $at,0x0000            # BB:3,L:50,H:5c,R_MIPS_HI16,SoundSuccess
                                                     # Line number: 1688
 000054  10000019   b          $101                  
 000058  ac200000   sw         $0,0($at)             # SoundSuccess,R_MIPS_LO16,SoundSuccess
$100:                                                # BB:4,L:5c,H:70
                                                     # Line number: 1691
 00005c  3c048011   lui        $a0,0x8011            
 000060  3c010000   lui        $at,0x0000            # R_MIPS_HI16,vab
 000064  84250000   lh         $a1,0($at)            # vab,R_MIPS_LO16,vab
 000068  0c000000   jal        SsSeqOpen             # R_MIPS_26,SsSeqOpen
 00006c  00000000   nop                              
 000070  3c010000   lui        $at,0x0000            # BB:5,L:70,H:90,R_MIPS_HI16,seq
 000074  a4220000   sh         $v0,0($at)            # seq,R_MIPS_LO16,seq
                                                     # Line number: 1692
 000078  3c010000   lui        $at,0x0000            # R_MIPS_HI16,seq
 00007c  84290000   lh         $t1,0($at)            # seq,R_MIPS_LO16,seq
 000080  00000000   nop                              
 000084  0120082a   slt        $at,$t1,$0            
 000088  10200009   beq        $at,$0,$102           
 00008c  00000000   nop                              
                                                     # Line number: 1694
 000090  3c010000   lui        $at,0x0000            # BB:6,L:90,H:a4,R_MIPS_HI16,seq
 000094  84250000   lh         $a1,0($at)            # seq,R_MIPS_LO16,seq
 000098  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@441
 00009c  0c000000   jal        printf                # R_MIPS_26,printf
 0000a0  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@441
                                                     # Line number: 1695
 0000a4  3c010000   lui        $at,0x0000            # BB:7,L:a4,H:b0,R_MIPS_HI16,SoundSuccess
                                                     # Line number: 1696
 0000a8  10000004   b          $103                  
 0000ac  ac200000   sw         $0,0($at)             # SoundSuccess,R_MIPS_LO16,SoundSuccess
$102:                                                # BB:8,L:b0,H:bc
                                                     # Line number: 1698
 0000b0  240a0001   addiu      $t2,$0,1              
 0000b4  3c010000   lui        $at,0x0000            # R_MIPS_HI16,SoundSuccess
 0000b8  ac2a0000   sw         $t2,0($at)            # SoundSuccess,R_MIPS_LO16,SoundSuccess
$103:                                                # BB:9,L:bc,H:bc
                                                     # Line number: 1699
$101:                                                # BB:10,L:bc,H:cc
 0000bc  8fbf003c   lw         $ra,60($sp)           
 0000c0  00000000   nop                              
 0000c4  03e00008   jr         $ra                   
 0000c8  27bd0040   addiu      $sp,$sp,64            
                    .end       InitSound             
 # *****************************************

                    .text                            # BB:0,L:0,H:1c
                    .globl     PlaySound             
                    .ent       PlaySound             
PlaySound:                                           # CS:96,ETS:0
                                                     # Line number: 1706
 000000  27bdffc0   addiu      $sp,$sp,-64           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1707
 000008  3c010000   lui        $at,0x0000            # R_MIPS_HI16,SoundSuccess
 00000c  8c280000   lw         $t0,0($at)            # SoundSuccess,R_MIPS_LO16,SoundSuccess
 000010  24010001   addiu      $at,$0,1              
 000014  1501000e   bne        $t0,$at,$100          
 000018  00000000   nop                              
                                                     # Line number: 1709
 00001c  2404007f   addiu      $a0,$0,127            # BB:1,L:1c,H:28
 000020  0c000000   jal        SsSetMVol             # R_MIPS_26,SsSetMVol
 000024  2405007f   addiu      $a1,$0,127            
                                                     # Line number: 1710
 000028  3c010000   lui        $at,0x0000            # BB:2,L:28,H:3c,R_MIPS_HI16,seq
 00002c  84240000   lh         $a0,0($at)            # seq,R_MIPS_LO16,seq
 000030  2405007f   addiu      $a1,$0,127            
 000034  0c000000   jal        SsSeqSetVol           # R_MIPS_26,SsSeqSetVol
 000038  2406007f   addiu      $a2,$0,127            
                                                     # Line number: 1711
 00003c  3c010000   lui        $at,0x0000            # BB:3,L:3c,H:50,R_MIPS_HI16,seq
 000040  84240000   lh         $a0,0($at)            # seq,R_MIPS_LO16,seq
 000044  24050001   addiu      $a1,$0,1              
 000048  0c000000   jal        SsSeqPlay             # R_MIPS_26,SsSeqPlay
 00004c  00003021   move       $a2,$0                
                                                     # Line number: 1712
$100:                                                # BB:4,L:50,H:60
                                                     # Line number: 1713
 000050  8fbf003c   lw         $ra,60($sp)           
 000054  00000000   nop                              
 000058  03e00008   jr         $ra                   
 00005c  27bd0040   addiu      $sp,$sp,64            
                    .end       PlaySound             
 # *****************************************

                    .text                            # BB:0,L:0,H:1c
                    .globl     StopSound             
                    .ent       StopSound             
StopSound:                                           # CS:108,ETS:0
                                                     # Line number: 1718
 000000  27bdffc0   addiu      $sp,$sp,-64           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1719
 000008  3c010000   lui        $at,0x0000            # R_MIPS_HI16,SoundSuccess
 00000c  8c280000   lw         $t0,0($at)            # SoundSuccess,R_MIPS_LO16,SoundSuccess
 000010  24010001   addiu      $at,$0,1              
 000014  15010011   bne        $t0,$at,$100          
 000018  00000000   nop                              
                                                     # Line number: 1721
 00001c  3c010000   lui        $at,0x0000            # BB:1,L:1c,H:2c,R_MIPS_HI16,seq
 000020  84240000   lh         $a0,0($at)            # seq,R_MIPS_LO16,seq
 000024  0c000000   jal        SsSeqStop             # R_MIPS_26,SsSeqStop
 000028  00000000   nop                              
                                                     # Line number: 1722
 00002c  0c000000   jal        VSync                 # BB:2,L:2c,H:34,R_MIPS_26,VSync
 000030  00002021   move       $a0,$0                
                                                     # Line number: 1723
 000034  0c000000   jal        VSync                 # BB:3,L:34,H:3c,R_MIPS_26,VSync
 000038  00002021   move       $a0,$0                
                                                     # Line number: 1724
 00003c  3c010000   lui        $at,0x0000            # BB:4,L:3c,H:4c,R_MIPS_HI16,seq
 000040  84240000   lh         $a0,0($at)            # seq,R_MIPS_LO16,seq
 000044  0c000000   jal        SsSeqClose            # R_MIPS_26,SsSeqClose
 000048  00000000   nop                              
                                                     # Line number: 1725
 00004c  3c010000   lui        $at,0x0000            # BB:5,L:4c,H:5c,R_MIPS_HI16,vab
 000050  84240000   lh         $a0,0($at)            # vab,R_MIPS_LO16,vab
 000054  0c000000   jal        SsVabClose            # R_MIPS_26,SsVabClose
 000058  00000000   nop                              
                                                     # Line number: 1726
$100:                                                # BB:6,L:5c,H:6c
                                                     # Line number: 1727
 00005c  8fbf003c   lw         $ra,60($sp)           
 000060  00000000   nop                              
 000064  03e00008   jr         $ra                   
 000068  27bd0040   addiu      $sp,$sp,64            
                    .end       StopSound             
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     DatafileSearch        
                    .ent       DatafileSearch        
DatafileSearch:                                      # CS:224,ETS:0
                                                     # Line number: 1732
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1735
 000008  1000002c   b          $100                  
 00000c  afa00048   sw         $0,72($sp)            # i
$101:                                                # BB:1,L:10,H:18
                                                     # Line number: 1737
 000010  10000021   b          $102                  
 000014  afa0004c   sw         $0,76($sp)            # j
$103:                                                # BB:2,L:18,H:58
                                                     # Line number: 1739
 000018  8fa80048   lw         $t0,72($sp)           # i
 00001c  2409005c   addiu      $t1,$0,92             
 000020  01090018   mult       $t0,$t1               
 000024  00005012   mflo       $t2                   
 000028  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,dfile
 00002c  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,dfile
 000030  016a6021   addu       $t4,$t3,$t2           
 000034  8fad0048   lw         $t5,72($sp)           # i
 000038  240e005c   addiu      $t6,$0,92             
 00003c  01ae0018   mult       $t5,$t6               
 000040  00004012   mflo       $t0                   
 000044  3c090000   lui        $t1,0x0000            # R_MIPS_HI16,dfile
 000048  21290000   addi       $t1,$t1,0             # R_MIPS_LO16,dfile
 00004c  01282821   addu       $a1,$t1,$t0           
 000050  0c000000   jal        CdSearchFile          # R_MIPS_26,CdSearchFile
 000054  25840044   addiu      $a0,$t4,68            
 000058  14400014   bne        $v0,$0,$104           # BB:3,L:58,H:60
 00005c  00000000   nop                              
                                                     # Line number: 1742
 000060  8faa0048   lw         $t2,72($sp)           # BB:4,L:60,H:88,i
 000064  240b005c   addiu      $t3,$0,92             
 000068  014b0018   mult       $t2,$t3               
 00006c  00006012   mflo       $t4                   
 000070  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,dfile
 000074  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,dfile
 000078  01ac2821   addu       $a1,$t5,$t4           
 00007c  3c040000   lui        $a0,0x0000            # R_MIPS_HI16,@462
 000080  0c000000   jal        printf                # R_MIPS_26,printf
 000084  20840000   addi       $a0,$a0,0             # R_MIPS_LO16,@462
                                                     # Line number: 1743
 000088  8fae004c   lw         $t6,76($sp)           # BB:5,L:88,H:98,j
 00008c  00000000   nop                              
 000090  21c80001   addi       $t0,$t6,1             
 000094  afa8004c   sw         $t0,76($sp)           # j
$102:                                                # BB:6,L:98,H:ac
 000098  8fa9004c   lw         $t1,76($sp)           # j
 00009c  00000000   nop                              
 0000a0  2921000a   slti       $at,$t1,10            
 0000a4  1420ffdc   bne        $at,$0,$103           
 0000a8  00000000   nop                              
$104:                                                # BB:7,L:ac,H:bc
                                                     # Line number: 1744
 0000ac  8faa0048   lw         $t2,72($sp)           # i
 0000b0  00000000   nop                              
 0000b4  214b0001   addi       $t3,$t2,1             
 0000b8  afab0048   sw         $t3,72($sp)           # i
$100:                                                # BB:8,L:bc,H:d0
 0000bc  8fac0048   lw         $t4,72($sp)           # i
 0000c0  00000000   nop                              
 0000c4  29810003   slti       $at,$t4,3             
 0000c8  1420ffd1   bne        $at,$0,$101           
 0000cc  00000000   nop                              
                                                     # Line number: 1745
 0000d0  8fbf003c   lw         $ra,60($sp)           # BB:9,L:d0,H:e0
 0000d4  00000000   nop                              
 0000d8  03e00008   jr         $ra                   
 0000dc  27bd0050   addiu      $sp,$sp,80            
                    .end       DatafileSearch        
 # *****************************************

                    .text                            # BB:0,L:0,H:10
                    .globl     DatafileRead          
                    .ent       DatafileRead          
DatafileRead:                                        # CS:272,ETS:0
                                                     # Line number: 1752
 000000  27bdffb0   addiu      $sp,$sp,-80           
 000004  afbf003c   sw         $ra,60($sp)           
                                                     # Line number: 1756
 000008  10000038   b          $100                  
 00000c  afa00044   sw         $0,68($sp)            # i
$101:                                                # BB:1,L:10,H:18
                                                     # Line number: 1758
 000010  1000002d   b          $102                  
 000014  afa00048   sw         $0,72($sp)            # j
$103:                                                # BB:2,L:18,H:7c
                                                     # Line number: 1760
 000018  8fa80044   lw         $t0,68($sp)           # i
 00001c  2409005c   addiu      $t1,$0,92             
 000020  01090018   mult       $t0,$t1               
 000024  00005012   mflo       $t2                   
 000028  3c0b0000   lui        $t3,0x0000            # R_MIPS_HI16,dfile
 00002c  216b0000   addi       $t3,$t3,0             # R_MIPS_LO16,dfile
 000030  016a2021   addu       $a0,$t3,$t2           
 000034  8fac0044   lw         $t4,68($sp)           # i
 000038  240d005c   addiu      $t5,$0,92             
 00003c  018d0018   mult       $t4,$t5               
 000040  00007012   mflo       $t6                   
 000044  3c080000   lui        $t0,0x0000            # R_MIPS_HI16,dfile
 000048  21080000   addi       $t0,$t0,0             # R_MIPS_LO16,dfile
 00004c  010e4821   addu       $t1,$t0,$t6           
 000050  8faa0044   lw         $t2,68($sp)           # i
 000054  240b005c   addiu      $t3,$0,92             
 000058  014b0018   mult       $t2,$t3               
 00005c  00006012   mflo       $t4                   
 000060  3c0d0000   lui        $t5,0x0000            # R_MIPS_HI16,dfile
 000064  21ad0000   addi       $t5,$t5,0             # R_MIPS_LO16,dfile
 000068  01ac7021   addu       $t6,$t5,$t4           
 00006c  8d250040   lw         $a1,64($t1)           
 000070  8dc60048   lw         $a2,72($t6)           
 000074  0c000000   jal        CdReadFile            # R_MIPS_26,CdReadFile
 000078  00000000   nop                              
                                                     # Line number: 1762
 00007c  10000003   b          $104                  # BB:3,L:7c,H:84
 000080  00000000   nop                              
$105:                                                # BB:4,L:84,H:8c
                                                     # Line number: 1763
 000084  0c000000   jal        VSync                 # R_MIPS_26,VSync
 000088  00002021   move       $a0,$0                
$104:                                                # BB:5,L:8c,H:98
 00008c  24040001   addiu      $a0,$0,1              
 000090  0c000000   jal        CdReadSync            # R_MIPS_26,CdReadSync
 000094  00002821   move       $a1,$0                
 000098  afa2004c   sw         $v0,76($sp)           # BB:6,L:98,H:a8,cnt
 00009c  0002082a   slt        $at,$0,$v0            
 0000a0  1420fff8   bne        $at,$0,$105           
 0000a4  00000000   nop                              
                                                     # Line number: 1765
 0000a8  8fa8004c   lw         $t0,76($sp)           # BB:7,L:a8,H:b8,cnt
 0000ac  00000000   nop                              
 0000b0  1100000a   beq        $t0,$0,$106           
 0000b4  00000000   nop                              
                                                     # Line number: 1767
 0000b8  8fa90048   lw         $t1,72($sp)           # BB:8,L:b8,H:c8,j
 0000bc  00000000   nop                              
 0000c0  212a0001   addi       $t2,$t1,1             
 0000c4  afaa0048   sw         $t2,72($sp)           # j
$102:                                                # BB:9,L:c8,H:dc
 0000c8  8fab0048   lw         $t3,72($sp)           # j
 0000cc  00000000   nop                              
 0000d0  2961000a   slti       $at,$t3,10            
 0000d4  1420ffd0   bne        $at,$0,$103           
 0000d8  00000000   nop                              
$106:                                                # BB:10,L:dc,H:ec
                                                     # Line number: 1768
 0000dc  8fac0044   lw         $t4,68($sp)           # i
 0000e0  00000000   nop                              
 0000e4  218d0001   addi       $t5,$t4,1             
 0000e8  afad0044   sw         $t5,68($sp)           # i
$100:                                                # BB:11,L:ec,H:100
 0000ec  8fae0044   lw         $t6,68($sp)           # i
 0000f0  00000000   nop                              
 0000f4  29c10003   slti       $at,$t6,3             
 0000f8  1420ffc5   bne        $at,$0,$101           
 0000fc  00000000   nop                              
                                                     # Line number: 1769
 000100  8fbf003c   lw         $ra,60($sp)           # BB:12,L:100,H:110
 000104  00000000   nop                              
 000108  03e00008   jr         $ra                   
 00010c  27bd0050   addiu      $sp,$sp,80            
                    .end       DatafileRead          
 # *****************************************

                    .data                            
ball16x8:                                            
                    .word      0x42110000,0x00055554,0x54321000,0x00677665,0x77654210,0x9aa99988,0x87765420,0xabbbaa99,
                    .word      0xa9876530,0xdeeeddcb,0xa9887500,0x0effeddb,0xba970000,0x000eeedc,0xa9000000,0x00000dcb
                    .data                            
ball16x16:                                           
                    .word      0x00000000,0x00000000,0x21000000,0x00000332,0x42110000,0x00055554,0x54321000,0x00677665,
                    .word      0x65432100,0x07888776,0x76543100,0x09998887,0x77654210,0x9aa99988,0x87765420,0xabbbaa99,
                    .word      0x98765420,0xbcccbba9,0x98876530,0xcddddcba,0xa9876530,0xdeeeddcb,0xa9887500,0x0effeddb,
                    .word      0xb9987500,0x0efffedc,0xba986000,0x00fffedc,0xba970000,0x000eeedc,0xa9000000,0x00000dcb
                    .data                            
ballcolor:                                           
                    .word      0x88670000,0x8cab8caa,0x94ef90cd,0x99329511,0x9d769954,0xa199a198,0xa5bba1ba,0xa5dda5dc,
                    .word      0x90c00000,0x99409520,0xa1a19d80,0xa601a5e1,0xae81aa41,0xb6c1b2a1,0xbb01b6e1,0xbf41bb21,
                    .word      0xa0400000,0xb060ac60,0xc0a0b880,0xccc0c8a0,0xdce0d4c0,0xe900e500,0xf120ed00,0xf920f520,
                    .word      0x84a40000,0x84e684e5,0x89488527,0x89898969,0x89cb89aa,0x8e0d8e0c,0x8e4e8e2d,0x8e6f8e4e,
                    .word      0x9ce00000,0xad60a940,0xbde0b5a0,0xca40c620,0xdac0d280,0xe720e300,0xef60eb40,0xf7a0f380,
                    .word      0x88250000,0x8c478c47,0x946a9049,0x986c946b,0x9c8e988d,0xa0b0a090,0xa4b2a0b1,0xa4b3a4b2,
                    .word      0xa0670000,0xb0abacaa,0xc0eeb8cd,0xcd31c910,0xdd75d553,0xe998e597,0xf1baedb9,0xf9dcf5db,
                    .word      0x91000000,0x99809960,0xa2019dc0,0xaa61a641,0xb2e1aea1,0xbb41b721,0xbf81bb61,0xc3c1bfa1,
                    .word      0xa0220000,0xb023ac23,0xc044b824,0xcc45c845,0xdc46d446,0xe867e467,0xf068ec67,0xf868f468,
                    .word      0x94850000,0x9cc89cc8,0xa92ba50a,0xb16ead4d,0xb9b1b58f,0xc1f3c1d2,0xca14c5f4,0xce36ca15,
                    .word      0x84c00000,0x89218901,0x8d818961,0x8de18dc1,0x92429202,0x96829282,0x96c296a2,0x970296e2,
                    .word      0x98450000,0xa888a467,0xb4aaac89,0xbcccbccc,0xcd0fc4ee,0xd531d110,0xdd32d932,0xe554e153,
                    .word      0x80860000,0x80c980c8,0x812c810a,0x816e814d,0x81b18190,0x81f481d3,0x821581f4,0x82378216,
                    .word      0x98410000,0xa862a462,0xb483b083,0xc0a4bca4,0xd0c5c8c4,0xd8e5d4e5,0xe106dce6,0xe906e506,
                    .word      0x98a70000,0xa8eaa4e9,0xb54eb12c,0xc191bd70,0xd1d4c9b2,0xda17d616,0xe259de38,0xea7be65a,
                    .word      0x80a30000,0x81048104,0x81668145,0x81c781a6,0x822881e8,0x826a8249,0x828a828a,0x82cb82ab,
                    .word      0x90a00000,0x9d0098e0,0xa540a120,0xad80ad80,0xb9e0b1c0,0xbe20be00,0xc640c240,0xca80c660,
                    .word      0x8c060000,0x940a9009,0x982d940b,0x9c2f9c2f,0xa433a031,0xa835a834,0xac37ac36,0xb039b038,
                    .word      0x98c00000,0xa520a100,0xb180ad40,0xbdc0b9a0,0xca20c200,0xd280d260,0xdaa0d680,0xe2e0dec0,
                    .word      0x80c00000,0x81208100,0x81808160,0x81e081c0,0x82408200,0x82808280,0x82c082a0,0x830082e0,
                    .word      0x98e60000,0xa56aa149,0xb1eda9ab,0xba4fb62f,0xc6d3c291,0xd335cf14,0xd777d356,0xdfb9db98,
                    .word      0x80e20000,0x81448123,0x85c58184,0x86268606,0x86888647,0x86e986c8,0x87298709,0x876a874a,
                    .word      0x98260000,0xa429a028,0xb04cac2a,0xbc4eb84d,0xc851c050,0xd074d073,0xd875d474,0xe077dc76,
                    .word      0x9cc70000,0xa94ba52a,0xb9afb16d,0xc5f2c1f1,0xd276ca34,0xdeb9da98,0xe6fbe2da,0xef3deb1c,
                    .word      0x84480000,0x886c886b,0x8cb0888e,0x8cd38cb2,0x90f790d5,0x951a9119,0x953c951b,0x953e953d,
                    .word      0x98e20000,0xa964a543,0xb5c5b1a4,0xc226be06,0xd2a8ca67,0xdb09d6e8,0xe349df29,0xeb8ae76a,
                    .word      0x84870000,0x88eb88ca,0x8d2f8d0d,0x91729171,0x95d69194,0x95f995f8,0x9a3b9a1a,0x9a5d9a3c,
                    .word      0x98060000,0xa429a028,0xb02cac2a,0xbc2eb82d,0xc851c050,0xd054d053,0xd855d454,0xe057dc56,
                    .word      0x9ce50000,0xa967a547,0xb9eab1a9,0xc64cc22b,0xd2ceca8d,0xdf30db10,0xe772e351,0xefb3eb92,
                    .word      0x80e50000,0x81688147,0x81eb81a9,0x824d822c,0x82d0828e,0x83328311,0x83738353,0x83b58394,
                    .word      0x9cc40000,0xad47a926,0xbda9b568,0xc9ebc5eb,0xda6ed22c,0xe6afe28f,0xeef1ead0,0xf732f311,
                    .word      0x80c30000,0x81458124,0x81a68185,0x820781e7,0x82898248,0x82ca82aa,0x830b82eb,0x834c832c
                    .data                            
dfile:                                               
                    .byte      0x5c,0x44,0x41,0x54,0x41,0x5c,0x53,0x4f,0x55,0x4e,0x44,0x5c,0x53,0x54,0x44,0x30,0x2e,0x56,0x48,0x3b,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    .byte      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    .byte      0x00,0x00,0x09,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x44,0x41,0x54,
                    .byte      0x41,0x5c,0x53,0x4f,0x55,0x4e,0x44,0x5c,0x53,0x54,0x44,0x30,0x2e,0x56,0x42,0x3b,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    .byte      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x80,
                    .byte      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x44,0x41,0x54,0x41,0x5c,0x53,0x4f,
                    .byte      0x55,0x4e,0x44,0x5c,0x47,0x4f,0x47,0x4f,0x2e,0x53,0x45,0x51,0x3b,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    .byte      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x80,0x00,0x00,0x00,0x00,
                    .byte      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
                    .data                            
                    .globl     vab                   
vab:         0000   .half      0x0000:1              
                    .data                            
                    .globl     seq                   
seq:         0000   .half      0x0000:1              
                    .data                            
                    .globl     bb0                   
bb0:     00000000   .word      0:1                   
                    .data                            
                    .globl     bb1                   
bb1:     00000000   .word      0:1                   
                    .data                            
                    .globl     PlayersMovement       
PlayersMovement:                                     
               00   .byte      0:24                  
                    .data                            
                    .globl     PlayersSprite         
PlayersSprite:                                       
               00   .byte      0:36                  
                    .data                            
                    .globl     PlayerSpeed           
PlayerSpe00000000   .word      0:1                   
                    .data                            
                    .globl     BallsMovement         
BallsMovement:                                       
               00   .byte      0:24                  
                    .data                            
                    .globl     BallsSprite           
BallsSprite:                                         
               00   .byte      0:36                  
                    .data                            
                    .globl     BackgroundRectangle   
BackgroundRectangle:                                 
               00   .byte      0:16                  
                    .data                            
                    .globl     WorldOrderingTable    
WorldOrderingTable:                                  
               00   .byte      0:40                  
                    .data                            
                    .globl     OrderingTableTags     
OrderingTableTags:                                   
               00   .byte      0:32                  
                    .data                            
                    .globl     GpuPacketArea         
GpuPacketArea:                                       
               00   .byte      0:9696                
                    .data                            
                    .globl     PadStatus             
PadStatus00000000   .word      0:1                   
                    .data                            
                    .globl     QuitFlag              
QuitFlag:00000000   .word      0:1                   
                    .data                            
                    .globl     BricksArray           
BricksArray:                                         
               00   .byte      0:15200               
                    .data                            
                    .globl     Level                 
Level:   00000000   .word      0:1                   
                    .data                            
                    .globl     NumberOfLives         
NumberOfL00000000   .word      0:1                   
                    .data                            
                    .globl     HighScoreTable        
HighScoreTable:                                      
               00   .byte      0:100                 
                    .data                            
                    .globl     CurrentScore          
CurrentScore:                                        
               00   .byte      0:20                  
                    .data                            
                    .globl     Coefficient           
Coefficient:        .float     7.5000000000000e-001:1
                    .data                            
$$$26:              .asciiz    "oden3.tim"           
                                                     
                    .data                            
$$$27:              .asciiz    "break3.tim"          
                                                     
                    .data                            
$$$28:              .asciiz    "Breakout!\x0a"       
                                                     
                    .data                            
$$$29:              .asciiz    "Frame number: %d\x0a"
                                                     
                    .data                            
$$$30:              .asciiz    "Lives: %d\x0a"       
                                                     
                    .data                            
$$$31:              .asciiz    "Score: %d\x0a"       
                                                     
                    .data                            
$$$91:              .asciiz    "You have died\x0a\x0a"
                                                     
                    .data                            
$$$92:              .asciiz    "Your final score was: %d\x0a\x0a"
                                                     
                    .data                            
$$$93:              .asciiz    "New high score! position %d\x0a\x0a"
                                                     
                    .data                            
$$$94:              .asciiz    "High Score Table\x0a"
                                                     
                    .data                            
$$$95:              .asciiz    "%s\x09%d\x0a"        
                                                     
                    .data                            
$$$96:              .asciiz    "\x0apress <R-down> for new game\x0a"
                                                     
                    .data                            
$$$97:              .asciiz    "press L1 and R1 to quit\x0a"
                                                     
                    .data                            
$$$122:             .asciiz    "You have finished ten levels\x0a\x0a"
                                                     
                    .data                            
$$$123:             .asciiz    "Your final score was: %d\x0a\x0a"
                                                     
                    .data                            
$$$124:             .asciiz    "New high score! position %d\x0a\x0a"
                                                     
                    .data                            
$$$125:             .asciiz    "High Score Table\x0a"
                                                     
                    .data                            
$$$126:             .asciiz    "%s\x09%d\x0a"        
                                                     
                    .data                            
$$$127:             .asciiz    "\x0apress <R-down> for new game\x0a"
                                                     
                    .data                            
$$$128:             .asciiz    "press L1 and R1 to quit\x0a"
                                                     
                    .data                            
$$$140:             .asciiz    "Player One"          
                                                     
                    .extern    GsDISPENV 20          
$$$327:                                              
         0000099c   .word      $139                  # R_MIPS_32,HandleAllObjects
         000007f0   .word      $136                  # R_MIPS_32,HandleAllObjects
         000008dc   .word      $133                  # R_MIPS_32,HandleAllObjects
         00000640   .word      $138                  # R_MIPS_32,HandleAllObjects
         0000072c   .word      $137                  # R_MIPS_32,HandleAllObjects
         000007f0   .word      $135                  # R_MIPS_32,HandleAllObjects
         000007f0   .word      $134                  # R_MIPS_32,HandleAllObjects
         000008dc   .word      $131                  # R_MIPS_32,HandleAllObjects
         000008dc   .word      $132                  # R_MIPS_32,HandleAllObjects
                    .data                            
                    .globl     SoundSuccess          
SoundSucc00000000   .word      0:1                   
                    .data                            
$$$440:             .asciiz    "SsVabTransfer failed (%d)\x0a"
                                                     
                    .data                            
$$$441:             .asciiz    "SsSeqOpen failed (%d)\x0a"
                                                     
                    .data                            
$$$462:             .asciiz    "%s not found.\x0a"   
                                                     

********* DWARF Information *********
00000000:<102>TAG_compile_unit
00000006    AT_sibling(00002c06)
0000000c    AT_producer(MW WIN32 MIPS C-Compiler)
00000027    AT_name(C:\\blackpsx\newcode\breakout\main.c)
0000004e    AT_language(LANG_C)
00000054    AT_low_pc(00000000)
0000005a    AT_high_pc(00000000)
00000060    AT_stmt_list(00000000)
00000066:<32>TAG_array_type
0000006c    AT_sibling(00000086)
00000072    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:15] FMT_ET: AT_<unknown>Unknown element type: 786517
)
00000086:<38>TAG_local_variable
0000008c    AT_sibling(000000ac)
00000092    AT_name(ball16x8)
0000009d    AT_location(<5> OP_ADDR(00000000))
000000a6    AT_user_def_type(00000066)
000000ac:<32>TAG_array_type
000000b2    AT_sibling(000000cc)
000000b8    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:31] FMT_ET: AT_<unknown>Unknown element type: 786517
)
000000cc:<39>TAG_local_variable
000000d2    AT_sibling(000000f3)
000000d8    AT_name(ball16x16)
000000e4    AT_location(<5> OP_ADDR(00000000))
000000ed    AT_user_def_type(000000ac)
000000f3:<32>TAG_array_type
000000f9    AT_sibling(00000113)
000000ff    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:7] FMT_ET: AT_<unknown>Unknown element type: 786517
)
00000113:<34>TAG_array_type
00000119    AT_sibling(00000135)
0000011f    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:31] FMT_ET: AT_<unknown>Unknown element type: 15925362
00 00 )
00000135:<39>TAG_local_variable
0000013b    AT_sibling(0000015c)
00000141    AT_name(ballcolor)
0000014d    AT_location(<5> OP_ADDR(00000000))
00000156    AT_user_def_type(00000113)
0000015c:<32>TAG_array_type
00000162    AT_sibling(0000017c)
00000168    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:63] FMT_ET: AT_<unknown>Unknown element type: 65621
)
0000017c:<27>TAG_structure_type
00000182    AT_sibling(00000226)
00000188    AT_name(CdlLOC)
00000191    AT_byte_size(00000004)
00000197:<35>TAG_member
0000019d    AT_sibling(000001ba)
000001a3    AT_name(minute)
000001ac    AT_fund_type(FT_unsigned_char)
000001b0    AT_location(<6> OP_CONST(0) OP_ADD)
000001ba:<35>TAG_member
000001c0    AT_sibling(000001dd)
000001c6    AT_name(second)
000001cf    AT_fund_type(FT_unsigned_char)
000001d3    AT_location(<6> OP_CONST(1) OP_ADD)
000001dd:<35>TAG_member
000001e3    AT_sibling(00000200)
000001e9    AT_name(sector)
000001f2    AT_fund_type(FT_unsigned_char)
000001f6    AT_location(<6> OP_CONST(2) OP_ADD)
00000200:<34>TAG_member
00000206    AT_sibling(00000222)
0000020c    AT_name(track)
00000214    AT_fund_type(FT_unsigned_char)
00000218    AT_location(<6> OP_CONST(3) OP_ADD)
00000222:<4>
00000226:<32>TAG_array_type
0000022c    AT_sibling(00000246)
00000232    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:15] FMT_ET: AT_<unknown>Unknown element type: 65621
)
00000246:<28>TAG_structure_type
0000024c    AT_sibling(000002cc)
00000252    AT_name(CdlFILE)
0000025c    AT_byte_size(00000018)
00000262:<34>TAG_member
00000268    AT_sibling(00000284)
0000026e    AT_name(pos)
00000274    AT_user_def_type(0000017c)
0000027a    AT_location(<6> OP_CONST(0) OP_ADD)
00000284:<33>TAG_member
0000028a    AT_sibling(000002a5)
00000290    AT_name(size)
00000297    AT_fund_type(FT_unsigned_long)
0000029b    AT_location(<6> OP_CONST(4) OP_ADD)
000002a5:<35>TAG_member
000002ab    AT_sibling(000002c8)
000002b1    AT_name(name)
000002b8    AT_user_def_type(00000226)
000002be    AT_location(<6> OP_CONST(8) OP_ADD)
000002c8:<4>
000002cc:<30>TAG_structure_type
000002d2    AT_sibling(00000357)
000002d8    AT_name(FILE_INFO)
000002e4    AT_byte_size(0000005c)
000002ea:<36>TAG_member
000002f0    AT_sibling(0000030e)
000002f6    AT_name(fname)
000002fe    AT_user_def_type(0000015c)
00000304    AT_location(<6> OP_CONST(0) OP_ADD)
0000030e:<33>TAG_member
00000314    AT_sibling(0000032f)
0000031a    AT_name(addr)
00000321    AT_fund_type(FT_pointer)
00000325    AT_location(<6> OP_CONST(64) OP_ADD)
0000032f:<36>TAG_member
00000335    AT_sibling(00000353)
0000033b    AT_name(finfo)
00000343    AT_user_def_type(00000246)
00000349    AT_location(<6> OP_CONST(68) OP_ADD)
00000353:<4>
00000357:<34>TAG_array_type
0000035d    AT_sibling(00000379)
00000363    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:2] FMT_ET: AT_<unknown>Unknown element type: 46923890
00 00 )
00000379:<35>TAG_local_variable
0000037f    AT_sibling(0000039c)
00000385    AT_name(dfile)
0000038d    AT_location(<5> OP_ADDR(00000000))
00000396    AT_user_def_type(00000357)
0000039c:<31>TAG_global_variable
000003a2    AT_sibling(000003bb)
000003a8    AT_name(vab)
000003ae    AT_location(<5> OP_ADDR(00000000))
000003b7    AT_fund_type(FT_signed_short)
000003bb:<31>TAG_global_variable
000003c1    AT_sibling(000003da)
000003c7    AT_name(seq)
000003cd    AT_location(<5> OP_ADDR(00000000))
000003d6    AT_fund_type(FT_signed_short)
000003da:<34>TAG_global_variable
000003e0    AT_sibling(000003fc)
000003e6    AT_name(bb0)
000003ec    AT_location(<5> OP_ADDR(00000000))
000003f5    AT_mod_fund_type(<3>MOD_pointer_to FT_unsigned_char)
000003fc:<34>TAG_global_variable
00000402    AT_sibling(0000041e)
00000408    AT_name(bb1)
0000040e    AT_location(<5> OP_ADDR(00000000))
00000417    AT_mod_fund_type(<3>MOD_pointer_to FT_unsigned_char)
0000041e:<33>TAG_structure_type
00000424    AT_sibling(00000502)
0000042a    AT_name(MovementData)
00000439    AT_byte_size(00000018)
0000043f:<30>TAG_member
00000445    AT_sibling(0000045d)
0000044b    AT_name(x)
0000044f    AT_fund_type(FT_integer)
00000453    AT_location(<6> OP_CONST(0) OP_ADD)
0000045d:<30>TAG_member
00000463    AT_sibling(0000047b)
00000469    AT_name(y)
0000046d    AT_fund_type(FT_integer)
00000471    AT_location(<6> OP_CONST(4) OP_ADD)
0000047b:<34>TAG_member
00000481    AT_sibling(0000049d)
00000487    AT_name(width)
0000048f    AT_fund_type(FT_integer)
00000493    AT_location(<6> OP_CONST(8) OP_ADD)
0000049d:<35>TAG_member
000004a3    AT_sibling(000004c0)
000004a9    AT_name(height)
000004b2    AT_fund_type(FT_integer)
000004b6    AT_location(<6> OP_CONST(12) OP_ADD)
000004c0:<31>TAG_member
000004c6    AT_sibling(000004df)
000004cc    AT_name(dx)
000004d1    AT_fund_type(FT_integer)
000004d5    AT_location(<6> OP_CONST(16) OP_ADD)
000004df:<31>TAG_member
000004e5    AT_sibling(000004fe)
000004eb    AT_name(dy)
000004f0    AT_fund_type(FT_integer)
000004f4    AT_location(<6> OP_CONST(20) OP_ADD)
000004fe:<4>
00000502:<45>TAG_global_variable
00000508    AT_sibling(0000052f)
0000050e    AT_name(PlayersMovement)
00000520    AT_location(<5> OP_ADDR(00000000))
00000529    AT_user_def_type(0000041e)
0000052f:<29>TAG_structure_type
00000535    AT_sibling(0000078b)
0000053b    AT_name(GsSPRITE)
00000546    AT_byte_size(00000024)
0000054c:<38>TAG_member
00000552    AT_sibling(00000572)
00000558    AT_name(attribute)
00000564    AT_fund_type(FT_unsigned_long)
00000568    AT_location(<6> OP_CONST(0) OP_ADD)
00000572:<30>TAG_member
00000578    AT_sibling(00000590)
0000057e    AT_name(x)
00000582    AT_fund_type(FT_signed_short)
00000586    AT_location(<6> OP_CONST(4) OP_ADD)
00000590:<30>TAG_member
00000596    AT_sibling(000005ae)
0000059c    AT_name(y)
000005a0    AT_fund_type(FT_signed_short)
000005a4    AT_location(<6> OP_CONST(6) OP_ADD)
000005ae:<30>TAG_member
000005b4    AT_sibling(000005cc)
000005ba    AT_name(w)
000005be    AT_fund_type(FT_unsigned_short)
000005c2    AT_location(<6> OP_CONST(8) OP_ADD)
000005cc:<30>TAG_member
000005d2    AT_sibling(000005ea)
000005d8    AT_name(h)
000005dc    AT_fund_type(FT_unsigned_short)
000005e0    AT_location(<6> OP_CONST(10) OP_ADD)
000005ea:<34>TAG_member
000005f0    AT_sibling(0000060c)
000005f6    AT_name(tpage)
000005fe    AT_fund_type(FT_unsigned_short)
00000602    AT_location(<6> OP_CONST(12) OP_ADD)
0000060c:<30>TAG_member
00000612    AT_sibling(0000062a)
00000618    AT_name(u)
0000061c    AT_fund_type(FT_unsigned_char)
00000620    AT_location(<6> OP_CONST(14) OP_ADD)
0000062a:<30>TAG_member
00000630    AT_sibling(00000648)
00000636    AT_name(v)
0000063a    AT_fund_type(FT_unsigned_char)
0000063e    AT_location(<6> OP_CONST(15) OP_ADD)
00000648:<31>TAG_member
0000064e    AT_sibling(00000667)
00000654    AT_name(cx)
00000659    AT_fund_type(FT_signed_short)
0000065d    AT_location(<6> OP_CONST(16) OP_ADD)
00000667:<31>TAG_member
0000066d    AT_sibling(00000686)
00000673    AT_name(cy)
00000678    AT_fund_type(FT_signed_short)
0000067c    AT_location(<6> OP_CONST(18) OP_ADD)
00000686:<30>TAG_member
0000068c    AT_sibling(000006a4)
00000692    AT_name(r)
00000696    AT_fund_type(FT_unsigned_char)
0000069a    AT_location(<6> OP_CONST(20) OP_ADD)
000006a4:<30>TAG_member
000006aa    AT_sibling(000006c2)
000006b0    AT_name(g)
000006b4    AT_fund_type(FT_unsigned_char)
000006b8    AT_location(<6> OP_CONST(21) OP_ADD)
000006c2:<30>TAG_member
000006c8    AT_sibling(000006e0)
000006ce    AT_name(b)
000006d2    AT_fund_type(FT_unsigned_char)
000006d6    AT_location(<6> OP_CONST(22) OP_ADD)
000006e0:<31>TAG_member
000006e6    AT_sibling(000006ff)
000006ec    AT_name(mx)
000006f1    AT_fund_type(FT_signed_short)
000006f5    AT_location(<6> OP_CONST(24) OP_ADD)
000006ff:<31>TAG_member
00000705    AT_sibling(0000071e)
0000070b    AT_name(my)
00000710    AT_fund_type(FT_signed_short)
00000714    AT_location(<6> OP_CONST(26) OP_ADD)
0000071e:<35>TAG_member
00000724    AT_sibling(00000741)
0000072a    AT_name(scalex)
00000733    AT_fund_type(FT_signed_short)
00000737    AT_location(<6> OP_CONST(28) OP_ADD)
00000741:<35>TAG_member
00000747    AT_sibling(00000764)
0000074d    AT_name(scaley)
00000756    AT_fund_type(FT_signed_short)
0000075a    AT_location(<6> OP_CONST(30) OP_ADD)
00000764:<35>TAG_member
0000076a    AT_sibling(00000787)
00000770    AT_name(rotate)
00000779    AT_fund_type(FT_long)
0000077d    AT_location(<6> OP_CONST(32) OP_ADD)
00000787:<4>
0000078b:<43>TAG_global_variable
00000791    AT_sibling(000007b6)
00000797    AT_name(PlayersSprite)
000007a7    AT_location(<5> OP_ADDR(00000000))
000007b0    AT_user_def_type(0000052f)
000007b6:<39>TAG_global_variable
000007bc    AT_sibling(000007dd)
000007c2    AT_name(PlayerSpeed)
000007d0    AT_location(<5> OP_ADDR(00000000))
000007d9    AT_fund_type(FT_integer)
000007dd:<43>TAG_global_variable
000007e3    AT_sibling(00000808)
000007e9    AT_name(BallsMovement)
000007f9    AT_location(<5> OP_ADDR(00000000))
00000802    AT_user_def_type(0000041e)
00000808:<41>TAG_global_variable
0000080e    AT_sibling(00000831)
00000814    AT_name(BallsSprite)
00000822    AT_location(<5> OP_ADDR(00000000))
0000082b    AT_user_def_type(0000052f)
00000831:<27>TAG_structure_type
00000837    AT_sibling(00000948)
0000083d    AT_name(GsBOXF)
00000846    AT_byte_size(00000010)
0000084c:<38>TAG_member
00000852    AT_sibling(00000872)
00000858    AT_name(attribute)
00000864    AT_fund_type(FT_unsigned_long)
00000868    AT_location(<6> OP_CONST(0) OP_ADD)
00000872:<30>TAG_member
00000878    AT_sibling(00000890)
0000087e    AT_name(x)
00000882    AT_fund_type(FT_signed_short)
00000886    AT_location(<6> OP_CONST(4) OP_ADD)
00000890:<30>TAG_member
00000896    AT_sibling(000008ae)
0000089c    AT_name(y)
000008a0    AT_fund_type(FT_signed_short)
000008a4    AT_location(<6> OP_CONST(6) OP_ADD)
000008ae:<30>TAG_member
000008b4    AT_sibling(000008cc)
000008ba    AT_name(w)
000008be    AT_fund_type(FT_unsigned_short)
000008c2    AT_location(<6> OP_CONST(8) OP_ADD)
000008cc:<30>TAG_member
000008d2    AT_sibling(000008ea)
000008d8    AT_name(h)
000008dc    AT_fund_type(FT_unsigned_short)
000008e0    AT_location(<6> OP_CONST(10) OP_ADD)
000008ea:<30>TAG_member
000008f0    AT_sibling(00000908)
000008f6    AT_name(r)
000008fa    AT_fund_type(FT_unsigned_char)
000008fe    AT_location(<6> OP_CONST(12) OP_ADD)
00000908:<30>TAG_member
0000090e    AT_sibling(00000926)
00000914    AT_name(g)
00000918    AT_fund_type(FT_unsigned_char)
0000091c    AT_location(<6> OP_CONST(13) OP_ADD)
00000926:<30>TAG_member
0000092c    AT_sibling(00000944)
00000932    AT_name(b)
00000936    AT_fund_type(FT_unsigned_char)
0000093a    AT_location(<6> OP_CONST(14) OP_ADD)
00000944:<4>
00000948:<49>TAG_global_variable
0000094e    AT_sibling(00000979)
00000954    AT_name(BackgroundRectangle)
0000096a    AT_location(<5> OP_ADDR(00000000))
00000973    AT_user_def_type(00000831)
00000979:<29>TAG_structure_type
0000097f    AT_sibling(000009ec)
00000985    AT_name(GsOT_TAG)
00000990    AT_byte_size(00000008)
00000996:<40>TAG_member
0000099c    AT_sibling(000009be)
000009a2    AT_name(p)
000009a6    AT_fund_type(FT_unsigned_integer)
000009aa    AT_bit_offset(00)
000009ae    AT_bit_size(00000018)
000009b4    AT_location(<6> OP_CONST(0) OP_ADD)
000009be:<42>TAG_member
000009c4    AT_sibling(000009e8)
000009ca    AT_name(num)
000009d0    AT_fund_type(FT_unsigned_char)
000009d4    AT_bit_offset(00)
000009d8    AT_bit_size(00000008)
000009de    AT_location(<6> OP_CONST(4) OP_ADD)
000009e8:<4>
000009ec:<25>TAG_structure_type
000009f2    AT_sibling(00000abb)
000009f8    AT_name(GsOT)
000009ff    AT_byte_size(00000014)
00000a05:<35>TAG_member
00000a0b    AT_sibling(00000a28)
00000a11    AT_name(length)
00000a1a    AT_fund_type(FT_unsigned_long)
00000a1e    AT_location(<6> OP_CONST(0) OP_ADD)
00000a28:<37>TAG_member
00000a2e    AT_sibling(00000a4d)
00000a34    AT_name(org)
00000a3a    AT_mod_u_d_type(<5>MOD_pointer_to (00000979))
00000a43    AT_location(<6> OP_CONST(4) OP_ADD)
00000a4d:<35>TAG_member
00000a53    AT_sibling(00000a70)
00000a59    AT_name(offset)
00000a62    AT_fund_type(FT_unsigned_long)
00000a66    AT_location(<6> OP_CONST(8) OP_ADD)
00000a70:<34>TAG_member
00000a76    AT_sibling(00000a92)
00000a7c    AT_name(point)
00000a84    AT_fund_type(FT_unsigned_long)
00000a88    AT_location(<6> OP_CONST(12) OP_ADD)
00000a92:<37>TAG_member
00000a98    AT_sibling(00000ab7)
00000a9e    AT_name(tag)
00000aa4    AT_mod_u_d_type(<5>MOD_pointer_to (00000979))
00000aad    AT_location(<6> OP_CONST(16) OP_ADD)
00000ab7:<4>
00000abb:<34>TAG_array_type
00000ac1    AT_sibling(00000add)
00000ac7    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:1] FMT_ET: AT_<unknown>Unknown element type: 166461554
00 00 )
00000add:<48>TAG_global_variable
00000ae3    AT_sibling(00000b0d)
00000ae9    AT_name(WorldOrderingTable)
00000afe    AT_location(<5> OP_ADDR(00000000))
00000b07    AT_user_def_type(00000abb)
00000b0d:<34>TAG_array_type
00000b13    AT_sibling(00000b2f)
00000b19    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:1] FMT_ET: AT_<unknown>Unknown element type: 158924914
00 00 )
00000b2f:<34>TAG_array_type
00000b35    AT_sibling(00000b51)
00000b3b    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:1] FMT_ET: AT_<unknown>Unknown element type: 185401458
00 00 )
00000b51:<47>TAG_global_variable
00000b57    AT_sibling(00000b80)
00000b5d    AT_name(OrderingTableTags)
00000b71    AT_location(<5> OP_ADDR(00000000))
00000b7a    AT_user_def_type(00000b2f)
00000b80:<32>TAG_array_type
00000b86    AT_sibling(00000ba0)
00000b8c    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:4847] FMT_ET: AT_<unknown>Unknown element type: 196693
)
00000ba0:<34>TAG_array_type
00000ba6    AT_sibling(00000bc2)
00000bac    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:1] FMT_ET: AT_<unknown>Unknown element type: 192938098
00 00 )
00000bc2:<43>TAG_global_variable
00000bc8    AT_sibling(00000bed)
00000bce    AT_name(GpuPacketArea)
00000bde    AT_location(<5> OP_ADDR(00000000))
00000be7    AT_user_def_type(00000ba0)
00000bed:<37>TAG_global_variable
00000bf3    AT_sibling(00000c12)
00000bf9    AT_name(PadStatus)
00000c05    AT_location(<5> OP_ADDR(00000000))
00000c0e    AT_fund_type(FT_unsigned_long)
00000c12:<36>TAG_global_variable
00000c18    AT_sibling(00000c36)
00000c1e    AT_name(QuitFlag)
00000c29    AT_location(<5> OP_ADDR(00000000))
00000c32    AT_fund_type(FT_integer)
00000c36:<26>TAG_structure_type
00000c3c    AT_sibling(00000d28)
00000c42    AT_name(Brick)
00000c4a    AT_byte_size(0000004c)
00000c50:<31>TAG_member
00000c56    AT_sibling(00000c6f)
00000c5c    AT_name(id)
00000c61    AT_fund_type(FT_integer)
00000c65    AT_location(<6> OP_CONST(0) OP_ADD)
00000c6f:<34>TAG_member
00000c75    AT_sibling(00000c91)
00000c7b    AT_name(alive)
00000c83    AT_fund_type(FT_integer)
00000c87    AT_location(<6> OP_CONST(4) OP_ADD)
00000c91:<33>TAG_member
00000c97    AT_sibling(00000cb2)
00000c9d    AT_name(type)
00000ca4    AT_fund_type(FT_integer)
00000ca8    AT_location(<6> OP_CONST(8) OP_ADD)
00000cb2:<38>TAG_member
00000cb8    AT_sibling(00000cd8)
00000cbe    AT_name(toughness)
00000cca    AT_fund_type(FT_integer)
00000cce    AT_location(<6> OP_CONST(12) OP_ADD)
00000cd8:<37>TAG_member
00000cde    AT_sibling(00000cfd)
00000ce4    AT_name(sprite)
00000ced    AT_user_def_type(0000052f)
00000cf3    AT_location(<6> OP_CONST(16) OP_ADD)
00000cfd:<39>TAG_member
00000d03    AT_sibling(00000d24)
00000d09    AT_name(movement)
00000d14    AT_user_def_type(0000041e)
00000d1a    AT_location(<6> OP_CONST(52) OP_ADD)
00000d24:<4>
00000d28:<34>TAG_array_type
00000d2e    AT_sibling(00000d4a)
00000d34    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:199] FMT_ET: AT_<unknown>Unknown element type: 204865650
00 00 )
00000d4a:<41>TAG_global_variable
00000d50    AT_sibling(00000d73)
00000d56    AT_name(BricksArray)
00000d64    AT_location(<5> OP_ADDR(00000000))
00000d6d    AT_user_def_type(00000d28)
00000d73:<33>TAG_global_variable
00000d79    AT_sibling(00000d94)
00000d7f    AT_name(Level)
00000d87    AT_location(<5> OP_ADDR(00000000))
00000d90    AT_fund_type(FT_integer)
00000d94:<41>TAG_global_variable
00000d9a    AT_sibling(00000dbd)
00000da0    AT_name(NumberOfLives)
00000db0    AT_location(<5> OP_ADDR(00000000))
00000db9    AT_fund_type(FT_integer)
00000dbd:<32>TAG_array_type
00000dc3    AT_sibling(00000ddd)
00000dc9    AT_subscr_data(<16> FMT_FT_C_C: FT_long [0:15] FMT_ET: AT_<unknown>Unknown element type: 65621
)
00000ddd:<26>TAG_structure_type
00000de3    AT_sibling(00000e41)
00000de9    AT_name(Score)
00000df1    AT_byte_size(00000014)
00000df7:<35>TAG_member
00000dfd    AT_sibling(00000e1a)
00000e03    AT_name(name)
00000e0a    AT_user_def_type(00000dbd)
00000e10    AT_location(<6> OP_CONST(0) OP_ADD)
00000e1a:<35>TAG_member
00000e20    AT_sibling(00000e3d)
00000e26    AT_name(points)
00000e2f    AT_fund_type(FT_unsigned_long)
00000e33    AT_location(<6> OP_CONST(16) OP_ADD)
00000e3d:<4>
00000e41:<34>TAG_array_type
00000e47    AT_sibling(00000e63)
00000e4d    AT_subscr_data(<18> FMT_FT_C_C: FT_long [0:4] FMT_ET: AT_<unknown>Unknown element type: 232587378
00 00 )
00000e63:<44>TAG_global_variable
00000e69    AT_sibling(00000e8f)
00000e6f    AT_name(HighScoreTable)
00000e80    AT_location(<5> OP_ADDR(00000000))
00000e89    AT_user_def_type(00000e41)
00000e8f:<42>TAG_global_variable
00000e95    AT_sibling(00000eb9)
00000e9b    AT_name(CurrentScore)
00000eaa    AT_location(<5> OP_ADDR(00000000))
00000eb3    AT_user_def_type(00000ddd)
00000eb9:<39>TAG_global_variable
00000ebf    AT_sibling(00000ee0)
00000ec5    AT_name(Coefficient)
00000ed3    AT_location(<5> OP_ADDR(00000000))
00000edc    AT_fund_type(FT_float)
00000ee0:<128>TAG_subroutine
00000ee6    AT_sibling(00001041)
00000eec    AT_name(main)
00000ef3    AT_low_pc(00000000)
00000ef9    AT_high_pc(000003e8)
00000eff    AT_stmt_list(00000000)
00000f05    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00000f15    AT_restore_SP(<11> OP_REG(29) OP_CONST(96) OP_ADD)
00000f24    AT_global_ref(00000c12)
00000f2a    AT_global_ref(00000e8f)
00000f30    AT_global_ref(00000d94)
00000f36    AT_global_ref(00000d73)
00000f3c    AT_global_ref(00000add)
00000f42    AT_global_ref(00000d4a)
00000f48    AT_global_ref(00000808)
00000f4e    AT_global_ref(0000078b)
00000f54    AT_global_ref(00000948)
00000f5a    AT_global_ref(00000bc2)
00000f60:<35>TAG_local_variable
00000f66    AT_sibling(00000f83)
00000f6c    AT_name(i)
00000f70    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
00000f7f    AT_fund_type(FT_integer)
00000f83:<45>TAG_local_variable
00000f89    AT_sibling(00000fb0)
00000f8f    AT_name(frameNumber)
00000f9d    AT_location(<11> OP_BASEREG(29) OP_CONST(88) OP_ADD)
00000fac    AT_fund_type(FT_unsigned_long)
00000fb0:<39>TAG_local_variable
00000fb6    AT_sibling(00000fd7)
00000fbc    AT_name(count)
00000fc4    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
00000fd3    AT_fund_type(FT_integer)
00000fd7:<46>TAG_local_variable
00000fdd    AT_sibling(00001005)
00000fe3    AT_name(activeBuffer)
00000ff2    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
00001001    AT_fund_type(FT_integer)
00001005:<56>TAG_local_variable
0000100b    AT_sibling(0000103d)
00001011    AT_name(playerWantsAnotherGame)
0000102a    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001039    AT_fund_type(FT_integer)
0000103d:<4>
00001041:<76>TAG_subroutine
00001047    AT_sibling(000010d7)
0000104d    AT_name(CrudestPause)
0000105c    AT_low_pc(00000000)
00001062    AT_high_pc(00000054)
00001068    AT_stmt_list(00000288)
0000106e    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
0000107e    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
0000108d:<35>TAG_formal_parameter
00001093    AT_sibling(000010b0)
00001099    AT_name(n)
0000109d    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
000010ac    AT_fund_type(FT_integer)
000010b0:<35>TAG_local_variable
000010b6    AT_sibling(000010d3)
000010bc    AT_name(i)
000010c0    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
000010cf    AT_fund_type(FT_integer)
000010d3:<4>
000010d7:<82>TAG_subroutine
000010dd    AT_sibling(00001129)
000010e3    AT_name(PauseUntilL1)
000010f2    AT_low_pc(00000000)
000010f8    AT_high_pc(00000048)
000010fe    AT_stmt_list(000002c2)
00001104    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00001114    AT_restore_SP(<11> OP_REG(29) OP_CONST(64) OP_ADD)
00001123    AT_global_ref(00000bed)
00001129:<81>TAG_subroutine
0000112f    AT_sibling(000011e8)
00001135    AT_name(SetUpLevel1)
00001143    AT_low_pc(00000000)
00001149    AT_high_pc(00000298)
0000114f    AT_stmt_list(00000310)
00001155    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00001165    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
00001174    AT_global_ref(00000d4a)
0000117a:<35>TAG_local_variable
00001180    AT_sibling(0000119d)
00001186    AT_name(j)
0000118a    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001199    AT_fund_type(FT_integer)
0000119d:<35>TAG_local_variable
000011a3    AT_sibling(000011c0)
000011a9    AT_name(i)
000011ad    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
000011bc    AT_fund_type(FT_integer)
000011c0:<36>TAG_local_variable
000011c6    AT_sibling(000011e4)
000011cc    AT_name(id)
000011d1    AT_location(<11> OP_BASEREG(29) OP_CONST(68) OP_ADD)
000011e0    AT_fund_type(FT_integer)
000011e4:<4>
000011e8:<120>TAG_subroutine
000011ee    AT_sibling(000013b3)
000011f4    AT_name(DealWithPlayersDeath)
0000120b    AT_low_pc(00000000)
00001211    AT_high_pc(000002f4)
00001217    AT_stmt_list(00000412)
0000121d    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
0000122d    AT_restore_SP(<11> OP_REG(29) OP_CONST(96) OP_ADD)
0000123c    AT_global_ref(00000e63)
00001242    AT_global_ref(00000e8f)
00001248    AT_global_ref(00000add)
0000124e    AT_global_ref(00000948)
00001254    AT_global_ref(00000bc2)
0000125a    AT_global_ref(00000bed)
00001260:<35>TAG_local_variable
00001266    AT_sibling(00001283)
0000126c    AT_name(i)
00001270    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
0000127f    AT_fund_type(FT_integer)
00001283:<41>TAG_local_variable
00001289    AT_sibling(000012ac)
0000128f    AT_name(exitSet)
00001299    AT_location(<11> OP_BASEREG(29) OP_CONST(88) OP_ADD)
000012a8    AT_fund_type(FT_integer)
000012ac:<49>TAG_local_variable
000012b2    AT_sibling(000012dd)
000012b8    AT_name(whenToLeaveLoop)
000012ca    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
000012d9    AT_fund_type(FT_integer)
000012dd:<42>TAG_local_variable
000012e3    AT_sibling(00001307)
000012e9    AT_name(frameNow)
000012f4    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
00001303    AT_fund_type(FT_integer)
00001307:<46>TAG_local_variable
0000130d    AT_sibling(00001335)
00001313    AT_name(activeBuffer)
00001322    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001331    AT_fund_type(FT_integer)
00001335:<39>TAG_local_variable
0000133b    AT_sibling(0000135c)
00001341    AT_name(count)
00001349    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
00001358    AT_fund_type(FT_integer)
0000135c:<42>TAG_local_variable
00001362    AT_sibling(00001386)
00001368    AT_name(position)
00001373    AT_location(<11> OP_BASEREG(29) OP_CONST(68) OP_ADD)
00001382    AT_fund_type(FT_integer)
00001386:<41>TAG_local_variable
0000138c    AT_sibling(000013af)
00001392    AT_name(newGame)
0000139c    AT_location(<11> OP_BASEREG(29) OP_CONST(64) OP_ADD)
000013ab    AT_fund_type(FT_integer)
000013af:<4>
000013b3:<132>TAG_subroutine
000013b9    AT_sibling(0000158a)
000013bf    AT_name(DealWithPlayerFinishingTenLevels)
000013e2    AT_low_pc(00000000)
000013e8    AT_high_pc(000002f4)
000013ee    AT_stmt_list(000005d2)
000013f4    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00001404    AT_restore_SP(<11> OP_REG(29) OP_CONST(96) OP_ADD)
00001413    AT_global_ref(00000e63)
00001419    AT_global_ref(00000e8f)
0000141f    AT_global_ref(00000add)
00001425    AT_global_ref(00000948)
0000142b    AT_global_ref(00000bc2)
00001431    AT_global_ref(00000bed)
00001437:<35>TAG_local_variable
0000143d    AT_sibling(0000145a)
00001443    AT_name(i)
00001447    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
00001456    AT_fund_type(FT_integer)
0000145a:<41>TAG_local_variable
00001460    AT_sibling(00001483)
00001466    AT_name(exitSet)
00001470    AT_location(<11> OP_BASEREG(29) OP_CONST(88) OP_ADD)
0000147f    AT_fund_type(FT_integer)
00001483:<49>TAG_local_variable
00001489    AT_sibling(000014b4)
0000148f    AT_name(whenToLeaveLoop)
000014a1    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
000014b0    AT_fund_type(FT_integer)
000014b4:<42>TAG_local_variable
000014ba    AT_sibling(000014de)
000014c0    AT_name(frameNow)
000014cb    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
000014da    AT_fund_type(FT_integer)
000014de:<46>TAG_local_variable
000014e4    AT_sibling(0000150c)
000014ea    AT_name(activeBuffer)
000014f9    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001508    AT_fund_type(FT_integer)
0000150c:<39>TAG_local_variable
00001512    AT_sibling(00001533)
00001518    AT_name(count)
00001520    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
0000152f    AT_fund_type(FT_integer)
00001533:<42>TAG_local_variable
00001539    AT_sibling(0000155d)
0000153f    AT_name(position)
0000154a    AT_location(<11> OP_BASEREG(29) OP_CONST(68) OP_ADD)
00001559    AT_fund_type(FT_integer)
0000155d:<41>TAG_local_variable
00001563    AT_sibling(00001586)
00001569    AT_name(newGame)
00001573    AT_location(<11> OP_BASEREG(29) OP_CONST(64) OP_ADD)
00001582    AT_fund_type(FT_integer)
00001586:<4>
0000158a:<87>TAG_subroutine
00001590    AT_sibling(0000162b)
00001596    AT_name(ClearScores)
000015a4    AT_low_pc(00000000)
000015aa    AT_high_pc(00000128)
000015b0    AT_stmt_list(00000792)
000015b6    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
000015c6    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
000015d5    AT_global_ref(00000e63)
000015db    AT_global_ref(00000e8f)
000015e1:<35>TAG_local_variable
000015e7    AT_sibling(00001604)
000015ed    AT_name(j)
000015f1    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001600    AT_fund_type(FT_integer)
00001604:<35>TAG_local_variable
0000160a    AT_sibling(00001627)
00001610    AT_name(i)
00001614    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
00001623    AT_fund_type(FT_integer)
00001627:<4>
0000162b:<80>TAG_subroutine
00001631    AT_sibling(0000178a)
00001637    AT_name(DealWithNewScore)
0000164a    AT_low_pc(00000000)
00001650    AT_high_pc(00000218)
00001656    AT_stmt_list(00000812)
0000165c    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
0000166c    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
0000167b:<43>TAG_formal_parameter
00001681    AT_sibling(000016a6)
00001687    AT_name(last)
0000168e    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
0000169d    AT_mod_u_d_type(<5>MOD_pointer_to (00000ddd))
000016a6:<44>TAG_formal_parameter
000016ac    AT_sibling(000016d2)
000016b2    AT_name(table)
000016ba    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
000016c9    AT_mod_u_d_type(<5>MOD_pointer_to (00000ddd))
000016d2:<45>TAG_formal_parameter
000016d8    AT_sibling(000016ff)
000016de    AT_name(tableLength)
000016ec    AT_location(<11> OP_BASEREG(29) OP_CONST(88) OP_ADD)
000016fb    AT_fund_type(FT_integer)
000016ff:<51>TAG_formal_parameter
00001705    AT_sibling(00001732)
0000170b    AT_name(outputPosition)
0000171c    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
0000172b    AT_mod_fund_type(<3>MOD_pointer_to FT_integer)
00001732:<35>TAG_local_variable
00001738    AT_sibling(00001755)
0000173e    AT_name(i)
00001742    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00001751    AT_fund_type(FT_integer)
00001755:<49>TAG_local_variable
0000175b    AT_sibling(00001786)
00001761    AT_name(highestPosition)
00001773    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
00001782    AT_fund_type(FT_integer)
00001786:<4>
0000178a:<113>TAG_subroutine
00001790    AT_sibling(00001822)
00001796    AT_name(InitialiseAll)
000017a6    AT_low_pc(00000000)
000017ac    AT_high_pc(00000170)
000017b2    AT_stmt_list(000008e2)
000017b8    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(76) OP_ADD OP_DEREF4)
000017c8    AT_restore_SP(<11> OP_REG(29) OP_CONST(96) OP_ADD)
000017d7    AT_global_ref(00000d94)
000017dd    AT_global_ref(00000d73)
000017e3    AT_global_ref(00000add)
000017e9    AT_global_ref(00000b51)
000017ef    AT_global_ref_by_name(GsDISPENV)
000017fb:<35>TAG_local_variable
00001801    AT_sibling(0000181e)
00001807    AT_name(i)
0000180b    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
0000181a    AT_fund_type(FT_integer)
0000181e:<4>
00001822:<117>TAG_subroutine
00001828    AT_sibling(000018be)
0000182e    AT_name(InitialiseObjects)
00001842    AT_low_pc(00000000)
00001848    AT_high_pc(00000248)
0000184e    AT_stmt_list(000009e4)
00001854    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00001864    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
00001873    AT_global_ref(00000d4a)
00001879    AT_global_ref(00000808)
0000187f    AT_global_ref(000007dd)
00001885    AT_global_ref(000007b6)
0000188b    AT_global_ref(0000078b)
00001891    AT_global_ref(00000502)
00001897:<35>TAG_local_variable
0000189d    AT_sibling(000018ba)
000018a3    AT_name(i)
000018a7    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
000018b6    AT_fund_type(FT_integer)
000018ba:<4>
000018be:<45>TAG_subroutine
000018c4    AT_sibling(00001916)
000018ca    AT_name(InitGsSprite)
000018d9    AT_low_pc(00000000)
000018df    AT_high_pc(00000060)
000018e5    AT_stmt_list(00000b22)
000018eb:<39>TAG_formal_parameter
000018f1    AT_sibling(00001912)
000018f7    AT_name(sprite)
00001900    AT_location(<5> OP_REG(4))
00001909    AT_mod_u_d_type(<5>MOD_pointer_to (0000052f))
00001912:<4>
00001916:<68>TAG_subroutine
0000191c    AT_sibling(00001a11)
00001922    AT_name(CreateNewBrick)
00001933    AT_low_pc(00000000)
00001939    AT_high_pc(000001c8)
0000193f    AT_stmt_list(00000bfc)
00001945    AT_restore_SP(<11> OP_REG(29) OP_CONST(16) OP_ADD)
00001954    AT_global_ref(00000d4a)
0000195a:<32>TAG_formal_parameter
00001960    AT_sibling(0000197a)
00001966    AT_name(type)
0000196d    AT_location(<5> OP_REG(4))
00001976    AT_fund_type(FT_integer)
0000197a:<37>TAG_formal_parameter
00001980    AT_sibling(0000199f)
00001986    AT_name(toughness)
00001992    AT_location(<5> OP_REG(5))
0000199b    AT_fund_type(FT_integer)
0000199f:<35>TAG_local_variable
000019a5    AT_sibling(000019c2)
000019ab    AT_name(i)
000019af    AT_location(<11> OP_BASEREG(29) OP_CONST(12) OP_ADD)
000019be    AT_fund_type(FT_integer)
000019c2:<39>TAG_local_variable
000019c8    AT_sibling(000019e9)
000019ce    AT_name(found)
000019d6    AT_location(<11> OP_BASEREG(29) OP_CONST(8) OP_ADD)
000019e5    AT_fund_type(FT_integer)
000019e9:<36>TAG_local_variable
000019ef    AT_sibling(00001a0d)
000019f5    AT_name(id)
000019fa    AT_location(<11> OP_BASEREG(29) OP_CONST(4) OP_ADD)
00001a09    AT_fund_type(FT_integer)
00001a0d:<4>
00001a11:<51>TAG_subroutine
00001a17    AT_sibling(00001a6b)
00001a1d    AT_name(DestroyBrick)
00001a2c    AT_low_pc(00000000)
00001a32    AT_high_pc(00000060)
00001a38    AT_stmt_list(00000cf4)
00001a3e    AT_global_ref(00000d4a)
00001a44:<35>TAG_formal_parameter
00001a4a    AT_sibling(00001a67)
00001a50    AT_name(brickID)
00001a5a    AT_location(<5> OP_REG(4))
00001a63    AT_fund_type(FT_integer)
00001a67:<4>
00001a6b:<73>TAG_subroutine
00001a71    AT_sibling(00001b02)
00001a77    AT_name(CountNumberOfBricks)
00001a8d    AT_low_pc(00000000)
00001a93    AT_high_pc(0000007c)
00001a99    AT_stmt_list(00000d38)
00001a9f    AT_restore_SP(<11> OP_REG(29) OP_CONST(16) OP_ADD)
00001aae    AT_global_ref(00000d4a)
00001ab4:<35>TAG_local_variable
00001aba    AT_sibling(00001ad7)
00001ac0    AT_name(i)
00001ac4    AT_location(<11> OP_BASEREG(29) OP_CONST(12) OP_ADD)
00001ad3    AT_fund_type(FT_integer)
00001ad7:<39>TAG_local_variable
00001add    AT_sibling(00001afe)
00001ae3    AT_name(count)
00001aeb    AT_location(<11> OP_BASEREG(29) OP_CONST(8) OP_ADD)
00001afa    AT_fund_type(FT_integer)
00001afe:<4>
00001b02:<70>TAG_subroutine
00001b08    AT_sibling(00001b6f)
00001b0e    AT_name(DestroyAllBricks)
00001b21    AT_low_pc(00000000)
00001b27    AT_high_pc(000001b0)
00001b2d    AT_stmt_list(00000da4)
00001b33    AT_restore_SP(<11> OP_REG(29) OP_CONST(16) OP_ADD)
00001b42    AT_global_ref(00000d4a)
00001b48:<35>TAG_local_variable
00001b4e    AT_sibling(00001b6b)
00001b54    AT_name(i)
00001b58    AT_location(<11> OP_BASEREG(29) OP_CONST(12) OP_ADD)
00001b67    AT_fund_type(FT_integer)
00001b6b:<4>
00001b6f:<53>TAG_subroutine
00001b75    AT_sibling(00001ba4)
00001b7b    AT_name(RestartTheBall)
00001b8c    AT_low_pc(00000000)
00001b92    AT_high_pc(00000070)
00001b98    AT_stmt_list(00000e56)
00001b9e    AT_global_ref(000007dd)
00001ba4:<56>TAG_subroutine
00001baa    AT_sibling(00001bdc)
00001bb0    AT_name(RestartPlayersBat)
00001bc4    AT_low_pc(00000000)
00001bca    AT_high_pc(0000005c)
00001bd0    AT_stmt_list(00000ea4)
00001bd6    AT_global_ref(00000502)
00001bdc:<122>TAG_subroutine
00001be2    AT_sibling(000020ab)
00001be8    AT_name(InitialiseTexturesAndSprites)
00001c07    AT_low_pc(00000000)
00001c0d    AT_high_pc(00000498)
00001c13    AT_stmt_list(00000ef2)
00001c19    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00001c29    AT_restore_SP(<11> OP_REG(29) OP_CONST(160) OP_ADD)
00001c38    AT_global_ref(00000808)
00001c3e    AT_global_ref(00000135)
00001c44    AT_global_ref(000000cc)
00001c4a    AT_global_ref(00000d4a)
00001c50    AT_global_ref(0000078b)
00001c56:<35>TAG_local_variable
00001c5c    AT_sibling(00001c79)
00001c62    AT_name(i)
00001c66    AT_location(<11> OP_BASEREG(29) OP_CONST(140) OP_ADD)
00001c75    AT_fund_type(FT_integer)
00001c79:<39>TAG_local_variable
00001c7f    AT_sibling(00001ca0)
00001c85    AT_name(mask2)
00001c8d    AT_location(<11> OP_BASEREG(29) OP_CONST(136) OP_ADD)
00001c9c    AT_fund_type(FT_unsigned_long)
00001ca0:<39>TAG_local_variable
00001ca6    AT_sibling(00001cc7)
00001cac    AT_name(mask1)
00001cb4    AT_location(<11> OP_BASEREG(29) OP_CONST(132) OP_ADD)
00001cc3    AT_fund_type(FT_unsigned_long)
00001cc7:<64>TAG_local_variable
00001ccd    AT_sibling(00001d07)
00001cd3    AT_name(sixteenBitTextureAttributeMask)
00001cf4    AT_location(<11> OP_BASEREG(29) OP_CONST(128) OP_ADD)
00001d03    AT_fund_type(FT_unsigned_long)
00001d07:<62>TAG_local_variable
00001d0d    AT_sibling(00001d45)
00001d13    AT_name(eightBitTextureAttributeMask)
00001d32    AT_location(<11> OP_BASEREG(29) OP_CONST(124) OP_ADD)
00001d41    AT_fund_type(FT_unsigned_long)
00001d45:<48>TAG_local_variable
00001d4b    AT_sibling(00001d75)
00001d51    AT_name(texturePageID3)
00001d62    AT_location(<11> OP_BASEREG(29) OP_CONST(158) OP_ADD)
00001d71    AT_fund_type(FT_unsigned_short)
00001d75:<48>TAG_local_variable
00001d7b    AT_sibling(00001da5)
00001d81    AT_name(texturePageID2)
00001d92    AT_location(<11> OP_BASEREG(29) OP_CONST(156) OP_ADD)
00001da1    AT_fund_type(FT_unsigned_short)
00001da5:<48>TAG_local_variable
00001dab    AT_sibling(00001dd5)
00001db1    AT_name(texturePageID1)
00001dc2    AT_location(<11> OP_BASEREG(29) OP_CONST(154) OP_ADD)
00001dd1    AT_fund_type(FT_unsigned_short)
00001dd5:<25>TAG_structure_type
00001ddb    AT_sibling(00001e6a)
00001de1    AT_name(RECT)
00001de8    AT_byte_size(00000008)
00001dee:<30>TAG_member
00001df4    AT_sibling(00001e0c)
00001dfa    AT_name(x)
00001dfe    AT_fund_type(FT_signed_short)
00001e02    AT_location(<6> OP_CONST(0) OP_ADD)
00001e0c:<30>TAG_member
00001e12    AT_sibling(00001e2a)
00001e18    AT_name(y)
00001e1c    AT_fund_type(FT_signed_short)
00001e20    AT_location(<6> OP_CONST(2) OP_ADD)
00001e2a:<30>TAG_member
00001e30    AT_sibling(00001e48)
00001e36    AT_name(w)
00001e3a    AT_fund_type(FT_signed_short)
00001e3e    AT_location(<6> OP_CONST(4) OP_ADD)
00001e48:<30>TAG_member
00001e4e    AT_sibling(00001e66)
00001e54    AT_name(h)
00001e58    AT_fund_type(FT_signed_short)
00001e5c    AT_location(<6> OP_CONST(6) OP_ADD)
00001e66:<4>
00001e6a:<55>TAG_local_variable
00001e70    AT_sibling(00001ea1)
00001e76    AT_name(frameBufferPosition)
00001e8c    AT_location(<11> OP_BASEREG(29) OP_CONST(146) OP_ADD)
00001e9b    AT_user_def_type(00001dd5)
00001ea1:<28>TAG_structure_type
00001ea7    AT_sibling(00002024)
00001ead    AT_name(GsIMAGE)
00001eb7    AT_byte_size(0000001c)
00001ebd:<34>TAG_member
00001ec3    AT_sibling(00001edf)
00001ec9    AT_name(pmode)
00001ed1    AT_fund_type(FT_unsigned_long)
00001ed5    AT_location(<6> OP_CONST(0) OP_ADD)
00001edf:<31>TAG_member
00001ee5    AT_sibling(00001efe)
00001eeb    AT_name(px)
00001ef0    AT_fund_type(FT_signed_short)
00001ef4    AT_location(<6> OP_CONST(4) OP_ADD)
00001efe:<31>TAG_member
00001f04    AT_sibling(00001f1d)
00001f0a    AT_name(py)
00001f0f    AT_fund_type(FT_signed_short)
00001f13    AT_location(<6> OP_CONST(6) OP_ADD)
00001f1d:<31>TAG_member
00001f23    AT_sibling(00001f3c)
00001f29    AT_name(pw)
00001f2e    AT_fund_type(FT_unsigned_short)
00001f32    AT_location(<6> OP_CONST(8) OP_ADD)
00001f3c:<31>TAG_member
00001f42    AT_sibling(00001f5b)
00001f48    AT_name(ph)
00001f4d    AT_fund_type(FT_unsigned_short)
00001f51    AT_location(<6> OP_CONST(10) OP_ADD)
00001f5b:<37>TAG_member
00001f61    AT_sibling(00001f80)
00001f67    AT_name(pixel)
00001f6f    AT_mod_fund_type(<3>MOD_pointer_to FT_unsigned_long)
00001f76    AT_location(<6> OP_CONST(12) OP_ADD)
00001f80:<31>TAG_member
00001f86    AT_sibling(00001f9f)
00001f8c    AT_name(cx)
00001f91    AT_fund_type(FT_signed_short)
00001f95    AT_location(<6> OP_CONST(16) OP_ADD)
00001f9f:<31>TAG_member
00001fa5    AT_sibling(00001fbe)
00001fab    AT_name(cy)
00001fb0    AT_fund_type(FT_signed_short)
00001fb4    AT_location(<6> OP_CONST(18) OP_ADD)
00001fbe:<31>TAG_member
00001fc4    AT_sibling(00001fdd)
00001fca    AT_name(cw)
00001fcf    AT_fund_type(FT_unsigned_short)
00001fd3    AT_location(<6> OP_CONST(20) OP_ADD)
00001fdd:<31>TAG_member
00001fe3    AT_sibling(00001ffc)
00001fe9    AT_name(ch)
00001fee    AT_fund_type(FT_unsigned_short)
00001ff2    AT_location(<6> OP_CONST(22) OP_ADD)
00001ffc:<36>TAG_member
00002002    AT_sibling(00002020)
00002008    AT_name(clut)
0000200f    AT_mod_fund_type(<3>MOD_pointer_to FT_unsigned_long)
00002016    AT_location(<6> OP_CONST(24) OP_ADD)
00002020:<4>
00002024:<42>TAG_local_variable
0000202a    AT_sibling(0000204e)
00002030    AT_name(image2)
00002039    AT_location(<11> OP_BASEREG(29) OP_CONST(96) OP_ADD)
00002048    AT_user_def_type(00001ea1)
0000204e:<42>TAG_local_variable
00002054    AT_sibling(00002078)
0000205a    AT_name(image1)
00002063    AT_location(<11> OP_BASEREG(29) OP_CONST(68) OP_ADD)
00002072    AT_user_def_type(00001ea1)
00002078:<47>TAG_local_variable
0000207e    AT_sibling(000020a7)
00002084    AT_name(timPointer)
00002091    AT_location(<11> OP_BASEREG(29) OP_CONST(64) OP_ADD)
000020a0    AT_mod_fund_type(<3>MOD_pointer_to FT_unsigned_long)
000020a7:<4>
000020ab:<69>TAG_subroutine
000020b1    AT_sibling(00002169)
000020b7    AT_name(ClearBackground)
000020c9    AT_low_pc(00000000)
000020cf    AT_high_pc(00000088)
000020d5    AT_stmt_list(000011d4)
000020db    AT_restore_SP(<11> OP_REG(29) OP_CONST(16) OP_ADD)
000020ea    AT_global_ref(00000948)
000020f0:<39>TAG_local_variable
000020f6    AT_sibling(00002117)
000020fc    AT_name(mask3)
00002104    AT_location(<11> OP_BASEREG(29) OP_CONST(12) OP_ADD)
00002113    AT_fund_type(FT_unsigned_long)
00002117:<39>TAG_local_variable
0000211d    AT_sibling(0000213e)
00002123    AT_name(mask2)
0000212b    AT_location(<11> OP_BASEREG(29) OP_CONST(8) OP_ADD)
0000213a    AT_fund_type(FT_unsigned_long)
0000213e:<39>TAG_local_variable
00002144    AT_sibling(00002165)
0000214a    AT_name(mask1)
00002152    AT_location(<11> OP_BASEREG(29) OP_CONST(4) OP_ADD)
00002161    AT_fund_type(FT_unsigned_long)
00002165:<4>
00002169:<109>TAG_subroutine
0000216f    AT_sibling(000021d6)
00002175    AT_name(DealWithControllerPad)
0000218d    AT_low_pc(00000000)
00002193    AT_high_pc(00000220)
00002199    AT_stmt_list(00001272)
0000219f    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(44) OP_ADD OP_DEREF4)
000021af    AT_restore_SP(<11> OP_REG(29) OP_CONST(48) OP_ADD)
000021be    AT_global_ref(000007b6)
000021c4    AT_global_ref(00000bed)
000021ca    AT_global_ref(00000502)
000021d0    AT_global_ref(00000c12)
000021d6:<128>TAG_subroutine
000021dc    AT_sibling(00002403)
000021e2    AT_name(HandleAllObjects)
000021f5    AT_low_pc(00000000)
000021fb    AT_high_pc(00000e4c)
00002201    AT_stmt_list(0000136a)
00002207    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002217    AT_restore_SP(<11> OP_REG(29) OP_CONST(128) OP_ADD)
00002226    AT_global_ref(0000078b)
0000222c    AT_global_ref(00000502)
00002232    AT_global_ref(00000808)
00002238    AT_global_ref(000007dd)
0000223e    AT_global_ref(00000d94)
00002244    AT_global_ref(00000e8f)
0000224a    AT_global_ref(00000d4a)
00002250    AT_global_ref(00000eb9)
00002256:<35>TAG_local_variable
0000225c    AT_sibling(00002279)
00002262    AT_name(i)
00002266    AT_location(<11> OP_BASEREG(29) OP_CONST(100) OP_ADD)
00002275    AT_fund_type(FT_integer)
00002279:<43>TAG_local_variable
0000227f    AT_sibling(000022a4)
00002285    AT_name(ballMoved)
00002291    AT_location(<11> OP_BASEREG(29) OP_CONST(96) OP_ADD)
000022a0    AT_fund_type(FT_integer)
000022a4:<43>TAG_local_variable
000022aa    AT_sibling(000022cf)
000022b0    AT_name(yLeftOver)
000022bc    AT_location(<11> OP_BASEREG(29) OP_CONST(92) OP_ADD)
000022cb    AT_fund_type(FT_integer)
000022cf:<50>TAG_local_variable
000022d5    AT_sibling(00002301)
000022db    AT_name(yInitialDistance)
000022ee    AT_location(<11> OP_BASEREG(29) OP_CONST(88) OP_ADD)
000022fd    AT_fund_type(FT_integer)
00002301:<43>TAG_local_variable
00002307    AT_sibling(0000232c)
0000230d    AT_name(xLeftOver)
00002319    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
00002328    AT_fund_type(FT_integer)
0000232c:<50>TAG_local_variable
00002332    AT_sibling(0000235e)
00002338    AT_name(xInitialDistance)
0000234b    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
0000235a    AT_fund_type(FT_integer)
0000235e:<38>TAG_local_variable
00002364    AT_sibling(00002384)
0000236a    AT_name(edge)
00002371    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00002380    AT_fund_type(FT_integer)
00002384:<41>TAG_local_variable
0000238a    AT_sibling(000023ad)
00002390    AT_name(brick)
00002398    AT_location(<11> OP_BASEREG(29) OP_CONST(120) OP_ADD)
000023a7    AT_user_def_type(00001dd5)
000023ad:<40>TAG_local_variable
000023b3    AT_sibling(000023d5)
000023b9    AT_name(ball)
000023c0    AT_location(<11> OP_BASEREG(29) OP_CONST(112) OP_ADD)
000023cf    AT_user_def_type(00001dd5)
000023d5:<42>TAG_local_variable
000023db    AT_sibling(000023ff)
000023e1    AT_name(player)
000023ea    AT_location(<11> OP_BASEREG(29) OP_CONST(104) OP_ADD)
000023f9    AT_user_def_type(00001dd5)
000023ff:<4>
00002403:<61>TAG_subroutine
00002409    AT_sibling(00002440)
0000240f    AT_name(MoveBallOutOfBat)
00002422    AT_low_pc(00000000)
00002428    AT_high_pc(00000058)
0000242e    AT_stmt_list(00001868)
00002434    AT_global_ref(000007dd)
0000243a    AT_global_ref(00000502)
00002440:<81>TAG_subroutine
00002446    AT_sibling(00002517)
0000244c    AT_name(RectanglesOverlap)
00002460    AT_low_pc(00000000)
00002466    AT_high_pc(00000058)
0000246c    AT_stmt_list(000018ac)
00002472    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002482    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
00002491:<44>TAG_formal_parameter
00002497    AT_sibling(000024bd)
0000249d    AT_name(first)
000024a5    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
000024b4    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000024bd:<45>TAG_formal_parameter
000024c3    AT_sibling(000024ea)
000024c9    AT_name(second)
000024d2    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
000024e1    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000024ea:<41>TAG_local_variable
000024f0    AT_sibling(00002513)
000024f6    AT_name(boolean)
00002500    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
0000250f    AT_fund_type(FT_integer)
00002513:<4>
00002517:<41>TAG_subroutine
0000251d    AT_sibling(00002591)
00002523    AT_name(XOverlap)
0000252e    AT_low_pc(00000000)
00002534    AT_high_pc(00000058)
0000253a    AT_stmt_list(000018e6)
00002540:<38>TAG_formal_parameter
00002546    AT_sibling(00002566)
0000254c    AT_name(first)
00002554    AT_location(<5> OP_REG(4))
0000255d    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
00002566:<39>TAG_formal_parameter
0000256c    AT_sibling(0000258d)
00002572    AT_name(second)
0000257b    AT_location(<5> OP_REG(5))
00002584    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
0000258d:<4>
00002591:<41>TAG_subroutine
00002597    AT_sibling(0000260b)
0000259d    AT_name(YOverlap)
000025a8    AT_low_pc(00000000)
000025ae    AT_high_pc(00000058)
000025b4    AT_stmt_list(0000192a)
000025ba:<38>TAG_formal_parameter
000025c0    AT_sibling(000025e0)
000025c6    AT_name(first)
000025ce    AT_location(<5> OP_REG(4))
000025d7    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000025e0:<39>TAG_formal_parameter
000025e6    AT_sibling(00002607)
000025ec    AT_name(second)
000025f5    AT_location(<5> OP_REG(5))
000025fe    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
00002607:<4>
0000260b:<79>TAG_subroutine
00002611    AT_sibling(000026e0)
00002617    AT_name(RectanglesTouch)
00002629    AT_low_pc(00000000)
0000262f    AT_high_pc(00000094)
00002635    AT_stmt_list(0000196e)
0000263b    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
0000264b    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
0000265a:<44>TAG_formal_parameter
00002660    AT_sibling(00002686)
00002666    AT_name(first)
0000266e    AT_location(<11> OP_BASEREG(29) OP_CONST(80) OP_ADD)
0000267d    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
00002686:<45>TAG_formal_parameter
0000268c    AT_sibling(000026b3)
00002692    AT_name(second)
0000269b    AT_location(<11> OP_BASEREG(29) OP_CONST(84) OP_ADD)
000026aa    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000026b3:<41>TAG_local_variable
000026b9    AT_sibling(000026dc)
000026bf    AT_name(boolean)
000026c9    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
000026d8    AT_fund_type(FT_integer)
000026dc:<4>
000026e0:<39>TAG_subroutine
000026e6    AT_sibling(00002758)
000026ec    AT_name(XTouch)
000026f5    AT_low_pc(00000000)
000026fb    AT_high_pc(00000080)
00002701    AT_stmt_list(000019a8)
00002707:<38>TAG_formal_parameter
0000270d    AT_sibling(0000272d)
00002713    AT_name(first)
0000271b    AT_location(<5> OP_REG(4))
00002724    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
0000272d:<39>TAG_formal_parameter
00002733    AT_sibling(00002754)
00002739    AT_name(second)
00002742    AT_location(<5> OP_REG(5))
0000274b    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
00002754:<4>
00002758:<39>TAG_subroutine
0000275e    AT_sibling(000027d0)
00002764    AT_name(YTouch)
0000276d    AT_low_pc(00000000)
00002773    AT_high_pc(00000080)
00002779    AT_stmt_list(00001a00)
0000277f:<38>TAG_formal_parameter
00002785    AT_sibling(000027a5)
0000278b    AT_name(first)
00002793    AT_location(<5> OP_REG(4))
0000279c    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000027a5:<39>TAG_formal_parameter
000027ab    AT_sibling(000027cc)
000027b1    AT_name(second)
000027ba    AT_location(<5> OP_REG(5))
000027c3    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
000027cc:<4>
000027d0:<72>TAG_subroutine
000027d6    AT_sibling(000028e9)
000027dc    AT_name(FindWhereRectanglesTouch)
000027f7    AT_low_pc(00000000)
000027fd    AT_high_pc(00000380)
00002803    AT_stmt_list(00001a58)
00002809    AT_restore_SP(<11> OP_REG(29) OP_CONST(16) OP_ADD)
00002818:<38>TAG_formal_parameter
0000281e    AT_sibling(0000283e)
00002824    AT_name(first)
0000282c    AT_location(<5> OP_REG(4))
00002835    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
0000283e:<39>TAG_formal_parameter
00002844    AT_sibling(00002865)
0000284a    AT_name(second)
00002853    AT_location(<5> OP_REG(5))
0000285c    AT_mod_u_d_type(<5>MOD_pointer_to (00001dd5))
00002865:<40>TAG_local_variable
0000286b    AT_sibling(0000288d)
00002871    AT_name(yTouch)
0000287a    AT_location(<11> OP_BASEREG(29) OP_CONST(12) OP_ADD)
00002889    AT_fund_type(FT_integer)
0000288d:<40>TAG_local_variable
00002893    AT_sibling(000028b5)
00002899    AT_name(xTouch)
000028a2    AT_location(<11> OP_BASEREG(29) OP_CONST(8) OP_ADD)
000028b1    AT_fund_type(FT_integer)
000028b5:<48>TAG_local_variable
000028bb    AT_sibling(000028e5)
000028c1    AT_name(whereTheyTouch)
000028d2    AT_location(<11> OP_BASEREG(29) OP_CONST(4) OP_ADD)
000028e1    AT_fund_type(FT_integer)
000028e5:<4>
000028e9:<83>TAG_subroutine
000028ef    AT_sibling(0000293c)
000028f5    AT_name(PadInit)
000028ff    AT_low_pc(00000000)
00002905    AT_high_pc(0000002c)
0000290b    AT_stmt_list(00001ce0)
00002911    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002921    AT_restore_SP(<11> OP_REG(29) OP_CONST(64) OP_ADD)
00002930    AT_global_ref(000003fc)
00002936    AT_global_ref(000003da)
0000293c:<52>TAG_subroutine
00002942    AT_sibling(00002970)
00002948    AT_name(PadRead)
00002952    AT_low_pc(00000000)
00002958    AT_high_pc(0000006c)
0000295e    AT_stmt_list(00001d10)
00002964    AT_global_ref(000003fc)
0000296a    AT_global_ref(000003da)
00002970:<40>TAG_global_variable
00002976    AT_sibling(00002998)
0000297c    AT_name(SoundSuccess)
0000298b    AT_location(<5> OP_ADDR(00000000))
00002994    AT_fund_type(FT_integer)
00002998:<91>TAG_subroutine
0000299e    AT_sibling(000029f3)
000029a4    AT_name(InitSound)
000029b0    AT_low_pc(00000000)
000029b6    AT_high_pc(000000cc)
000029bc    AT_stmt_list(00001d40)
000029c2    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
000029d2    AT_restore_SP(<11> OP_REG(29) OP_CONST(64) OP_ADD)
000029e1    AT_global_ref(00002970)
000029e7    AT_global_ref(000003bb)
000029ed    AT_global_ref(0000039c)
000029f3:<85>TAG_subroutine
000029f9    AT_sibling(00002a48)
000029ff    AT_name(PlaySound)
00002a0b    AT_low_pc(00000000)
00002a11    AT_high_pc(00000060)
00002a17    AT_stmt_list(00001dd4)
00002a1d    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002a2d    AT_restore_SP(<11> OP_REG(29) OP_CONST(64) OP_ADD)
00002a3c    AT_global_ref(000003bb)
00002a42    AT_global_ref(00002970)
00002a48:<91>TAG_subroutine
00002a4e    AT_sibling(00002aa3)
00002a54    AT_name(StopSound)
00002a60    AT_low_pc(00000000)
00002a66    AT_high_pc(0000006c)
00002a6c    AT_stmt_list(00001e2c)
00002a72    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002a82    AT_restore_SP(<11> OP_REG(29) OP_CONST(64) OP_ADD)
00002a91    AT_global_ref(0000039c)
00002a97    AT_global_ref(000003bb)
00002a9d    AT_global_ref(00002970)
00002aa3:<84>TAG_subroutine
00002aa9    AT_sibling(00002b41)
00002aaf    AT_name(DatafileSearch)
00002ac0    AT_low_pc(00000000)
00002ac6    AT_high_pc(000000e0)
00002acc    AT_stmt_list(00001e98)
00002ad2    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002ae2    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
00002af1    AT_global_ref(00000379)
00002af7:<35>TAG_local_variable
00002afd    AT_sibling(00002b1a)
00002b03    AT_name(j)
00002b07    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00002b16    AT_fund_type(FT_integer)
00002b1a:<35>TAG_local_variable
00002b20    AT_sibling(00002b3d)
00002b26    AT_name(i)
00002b2a    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
00002b39    AT_fund_type(FT_integer)
00002b3d:<4>
00002b41:<82>TAG_subroutine
00002b47    AT_sibling(00002c02)
00002b4d    AT_name(DatafileRead)
00002b5c    AT_low_pc(00000000)
00002b62    AT_high_pc(00000110)
00002b68    AT_stmt_list(00001efa)
00002b6e    AT_return_addr(<12> OP_BASEREG(29) OP_CONST(60) OP_ADD OP_DEREF4)
00002b7e    AT_restore_SP(<11> OP_REG(29) OP_CONST(80) OP_ADD)
00002b8d    AT_global_ref(00000379)
00002b93:<37>TAG_local_variable
00002b99    AT_sibling(00002bb8)
00002b9f    AT_name(cnt)
00002ba5    AT_location(<11> OP_BASEREG(29) OP_CONST(76) OP_ADD)
00002bb4    AT_fund_type(FT_integer)
00002bb8:<35>TAG_local_variable
00002bbe    AT_sibling(00002bdb)
00002bc4    AT_name(j)
00002bc8    AT_location(<11> OP_BASEREG(29) OP_CONST(72) OP_ADD)
00002bd7    AT_fund_type(FT_integer)
00002bdb:<35>TAG_local_variable
00002be1    AT_sibling(00002bfe)
00002be7    AT_name(i)
00002beb    AT_location(<11> OP_BASEREG(29) OP_CONST(68) OP_ADD)
00002bfa    AT_fund_type(FT_integer)
00002bfe:<4>
00002c02:<4>
