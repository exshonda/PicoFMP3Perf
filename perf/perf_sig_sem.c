/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2009-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  @(#) $Id: perf_sig_sem.c 1203 2016-07-18 07:05:08Z ertl-honda $
 */

/*
 *  sig_sem 性能測定プログラム
 */

#include "perf.h"
#include "perf_sig_sem.h"
#include <stdbool.h>

/*
 *  計測回数と実行時間分布を記録する最大時間
 */
#define NO_MEASURE	20000U			/* 計測回数 */
#define MAX_TIME	400000U			/* 実行時間分布を記録する最大時間 */

/*
 *  計測の前後でのフックルーチン
 */
#ifndef CPU1_PERF_PRE_HOOK
#define CPU1_PERF_PRE_HOOK
#endif  /* CPU1_PERF_PRE_HOOK */
#ifndef CPU1_PERF_POST_HOOK
#define CPU1_PERF_POST_HOOK
#endif  /* CPU1_PERF_POST_HOOK */
#ifndef CPU2_PERF_PRE_HOOK
#define CPU2_PERF_PRE_HOOK
#endif  /* CPU2_PERF_PRE_HOOK */
#ifndef CPU2_PERF_POST_HOOK
#define CPU2_PERF_POST_HOOK
#endif  /* CPU2_PERF_POST_HOOK */

/*
 *  変数
 */
volatile bool task2_1_running;
volatile bool task2_3_running;

/*
 *  case1
 */
void task1_1(intptr_t exinf)
{
	begin_measure(1);
	wai_sem(SEM1);
}

/*
 *  case2
 */
void task2_1(intptr_t exinf)
{
	task2_1_running = true;
	wai_sem(SEM1);
	end_measure(2);
	task2_1_running = false;
}

/*
 *  case3:高優先度タスク
 */
void task2_2(intptr_t exinf)
{
	wai_sem(SEM1);
	end_measure(3);
	ter_tsk(TASK2_3);
	task2_3_running = false;
}

/*
 *  case3:低優先度タスク
 */
void task2_3(intptr_t exinf)
{
	task2_3_running = true;
	while(1);
}

/*
 * 計測ルーチン
 */
void perf_eval(uint_t n)
{
	uint_t	i;

	init_hist(1);
	init_hist(2);
	init_hist(3);        
	syslog_fls_log();

	dly_tsk(1000000);
	CPU1_PERF_PRE_HOOK;

	for ( i = 0; i < NO_MEASURE; i++ ) {
		switch (n) {
			//オーバーヘッドの測定
		case 0:
			//begin_measure(1);
			//end_measure(1);
			break;
			//【１】セマフォに対する待ちタスクが存在する．sig_semを実行
			//      するタスク（実行タスク）と同じプロセッサに割り付けら
			//      れており，優先度は実行タスクより高いため，ディスパッチが発生する
			//      case2のためにwai_semの実行時間を計測する
		case 1:
			chg_pri(0, MID_PRIORITY);
			act_tsk(TASK1_1);
			end_measure(1);
			sig_sem(SEM1);
			chg_pri(0, MAIN_PRIORITY);
			break;
			//【２】セマフォに対する待ちタスクが存在する．sig_semを実行
			//      するタスク（実行タスク）と異なるプロセッサに割り付けら
			//      れており，ディスパッチが発生する．
			//      アイドルタスクは未使用．
		case 2:
			task2_1_running = false;
			act_tsk(TASK2_1);
			while (task2_1_running == false);
			dly_tsk(100);
			begin_measure(2);
			sig_sem(SEM1);
			while (task2_1_running == true);
			break;
			//【３】セマフォに対する待ちタスクが存在する．sig_semを実行
			//      するタスク（実行タスク）と異なるプロセッサに割り付けら
			//      れており，ディスパッチが発生する．
			//      アイドルタスクを使用．
		case 3:
			task2_3_running = false;
			act_tsk(TASK2_2);
			act_tsk(TASK2_3);
			while (task2_3_running == false);
			begin_measure(3);
			sig_sem(SEM1);
			while (task2_3_running == true);
			break;		
		}
	}

	CPU1_PERF_POST_HOOK;

	if (n == 1) {
		syslog(LOG_NOTICE, "==================================");
		syslog(LOG_NOTICE, "(%d)", 1);
		syslog(LOG_NOTICE, "----------------------------------");
		print_hist(1);
	}

	if (n == 2) {
		syslog(LOG_NOTICE, "==================================");
		syslog(LOG_NOTICE, "(%d)", 2);
		syslog(LOG_NOTICE, "----------------------------------");
		print_hist(2);
	}


	if (n == 3) {
		syslog(LOG_NOTICE, "==================================");
		syslog(LOG_NOTICE, "(%d)", 3);
		syslog(LOG_NOTICE, "----------------------------------");
		print_hist(3);
	}
}

/*
 *  PE1 メインタスク
 */
void main_task1(intptr_t exinf)
{
	syslog(LOG_NOTICE, "perf_sig_sem for fmp3");

	/* GPIO Init */
	slp_tsk();

//	perf_eval(0);
//	perf_eval(1);
//	perf_eval(2);
	perf_eval(3);

	wup_tsk(MAIN_TASK2);
}

/*
 *  PE2 メインタスク
 */
void main_task2(intptr_t exinf)
{
	CPU2_PERF_PRE_HOOK;
	wup_tsk(MAIN_TASK1);
	slp_tsk();
	CPU2_PERF_POST_HOOK;
}
