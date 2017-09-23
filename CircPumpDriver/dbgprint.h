/*
 * print.h
 *
 *  Created on: 12 sty 2017
 *      Author: ark036
 */

#ifndef _DBG_PRINT_H_
#define _DBG_PRINT_H_

/**
 * NOTE: This file requires 2 macros:
 *       #define PRINT(_s_)   do_something(_s_)
 *       #define PRINTLN(_s_) do_something_and_issue_new_line(_s_)
 */

#define PRINT2(_s1_,_s2_)             do{ PRINT(_s1_); PRINT(_s2_); } while(0)
#define PRINTLN2(_s1_,_s2_)           do{ PRINT(_s1_); PRINTLN(_s2_); } while(0)
#define PRINT3(_s1_,_s2_,_s3_)        do{ PRINT2(_s1_,_s2_); PRINT(_s3_); } while(0)
#define PRINTLN3(_s1_,_s2_,_s3_)      do{ PRINT2(_s1_,_s2_); PRINTLN(_s3_); } while(0)
#define PRINT4(_s1_,_s2_,_s3_,_s4_)   do{ PRINT2(_s1_,_s2_); PRINT2(_s3_,_s4_); } while(0)
#define PRINTLN4(_s1_,_s2_,_s3_,_s4_) do{ PRINT2(_s1_,_s2_); PRINTLN2(_s3_,_s4_); } while(0)
#define PRINT5(_s1_,_s2_,_s3_,_s4_,_s5_)   do{ PRINT2(_s1_,_s2_); PRINT3(_s3_,_s4_,_s5_); } while(0)
#define PRINTLN5(_s1_,_s2_,_s3_,_s4_,_s5_) do{ PRINT2(_s1_,_s2_); PRINTLN3(_s3_,_s4_,_s5_); } while(0)

#if defined( DEBUG ) || defined( _DEBUG )
# define IFDEBUG(_run_) do {_run_;} while(0)
#else
# define IFDEBUG(_run_) 
#endif

#define DBGPRINT(_s_)                    IFDEBUG( PRINT(_s1_) )
#define DBGPRINTLN(_s_)                  IFDEBUG( PRINTLN(_s1_) )
#define DBGPRINT2(_s1_,_s2_)             IFDEBUG( PRINT2(_s1_,_s2_) )
#define DBGPRINTLN2(_s1_,_s2_)           IFDEBUG( PRINTLN2(_s1_,_s2_) )
#define DBGPRINT3(_s1_,_s2_,_s3_)        IFDEBUG( PRINT3(_s1_,_s2_,_s3_) )
#define DBGPRINTLN3(_s1_,_s2_,_s3_)      IFDEBUG( PRINTLN3(_s1_,_s2_,_s3_) )
#define DBGPRINT4(_s1_,_s2_,_s3_,_s4_)   IFDEBUG( PRINT4(_s1_,_s2_,_s3_,_s4_) )
#define DBGPRINTLN4(_s1_,_s2_,_s3_,_s4_) IFDEBUG( PRINTLN4(_s1_,_s2_,_s3_,_s4_) )
#define DBGPRINT5(_s1_,_s2_,_s3_,_s4_,_s5_)   IFDEBUG( PRINT5(_s1_,_s2_,_s3_,_s4_,_s5) )
#define DBGPRINTLN5(_s1_,_s2_,_s3_,_s4_,_s5_) IFDEBUG( PRINTLN5(_s1_,_s2_,_s3_,_s4_,_s5_) )

#endif /* _DBG_PRINT_H_ */
