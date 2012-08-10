/**
 * @file stats.c
 *
 * @brief Statistics.
 *
 * The purpose of these functions is to gather performance
 * statistics on some algorithms or code.
 *
 * @date 1998 - 2012
 * @author Richard Delorme
 * @version 4.3
 */

#include "stats.h"
#include "options.h"
#include "search.h"
#include "ybwc.h"

#include <stdio.h>

Statistics statistics;

/**
 * @brief Intialization of the statistics.
 */
void statistics_init(void)
{
	int i, j;

	for (i = 0; i < MAX_THREADS; ++i) {
		statistics.n_task_nodes[i] = 0;
		statistics.n_task[i] = 0;
	}

	statistics.n_hash_upgrade = 0;
	statistics.n_hash_update = 0;
	statistics.n_hash_new = 0;
	statistics.n_hash_remove = 0;
	statistics.n_hash_search = 0;
	statistics.n_hash_found = 0;

	statistics.n_nodes = 0;
	statistics.n_split_try = 0;
	statistics.n_split_success = 0;
	statistics.n_master_helper = 0;
	statistics.n_stopped_slave = 0;
	statistics.n_stopped_master = 0;
	statistics.n_waited_slave = 0;
	statistics.n_wake_up = 0;

	statistics.n_PVS_root = 0;
	statistics.n_PVS_midgame = 0;
	statistics.n_NWS_midgame = 0;
	statistics.n_NWS_endgame = 0;
	statistics.n_PVS_shallow = 0;
	statistics.n_NWS_shallow = 0;
	statistics.n_search_solve = 0;
	statistics.n_search_solve_0 = 0;
	statistics.n_board_solve_2 = 0;
	statistics.n_search_solve_3 = 0;
	statistics.n_search_solve_4 = 0;
	statistics.n_search_eval_0 = 0;
	statistics.n_search_eval_1 = 0;
	statistics.n_search_eval_2 = 0;

	statistics.n_concurrent_garbage = 0;
	statistics.n_hash_try = 0;
	statistics.n_hash_low_cutoff = 0;
	statistics.n_hash_high_cutoff = 0;
	statistics.n_stability_try = 0;
	statistics.n_stability_low_cutoff = 0;
	statistics.n_probcut_try = 0;
	statistics.n_probcut_low_try = 0;
	statistics.n_probcut_high_try = 0;
	statistics.n_probcut_low_cutoff = 0;
	statistics.n_probcut_high_cutoff = 0;
	statistics.n_etc_try = 0;
	statistics.n_etc_high_cutoff = 0;
	statistics.n_esc_high_cutoff = 0;

	for (j = 0; j < BOARD_SIZE; ++j)
	for (i = 0; i < 10; ++i) {
		statistics.n_played_square[j][i] = 0;
		statistics.n_good_square[j][i] = 0;
	}

	statistics.n_NWS_candidate = 0;
	statistics.n_NWS_bad_candidate = 0;
}

/**
 * @brief Cumulate node counts from the last search.
 *
 * @param search Last search.
 */
void statistics_sum_nodes(Search *search)
{
	int i;

	statistics.n_nodes += search->n_nodes;
	for (i = 0; i < search->tasks->n; ++i) {
		statistics.n_task_nodes[i] += search->tasks->task[i].n_nodes;
		statistics.n_task[i] = search->tasks->task[i].n_calls;
	}
}

/**
 * @brief Print statistics.
 */
void statistics_print(void)
{
	int i, j;

	statistics.n_split_success += statistics.n_master_helper;

	if (statistics.n_split_success) {
		printf("YBWC:\n");
		printf("nodes splitted:      %12llu (%6.2f%%)\n", statistics.n_split_success, 100.0 * statistics.n_split_success / statistics.n_split_try);
		printf("master helper tasks: %12llu (%6.2f%%)\n", statistics.n_master_helper, 100.0 * statistics.n_master_helper / statistics.n_split_success);
		printf("slave nodes stopped: %12llu (%6.2f%%)\n", statistics.n_stopped_slave, 100.0 * statistics.n_stopped_slave / statistics.n_split_success);
		printf("slave master stopped:%12llu (%6.2f%%) = %12llu\n", statistics.n_stopped_master, 100.0 * statistics.n_stopped_master / statistics.n_split_success, statistics.n_wake_up);
		printf("slave nodes waited:  %12llu (%6.2f%%)\n", statistics.n_waited_slave, 100.0 * statistics.n_waited_slave / statistics.n_split_success);
		printf("main thread (%llu nodes)\n", statistics.n_nodes);
		for (i = 0; i < options.n_task - 1; ++i) {
			printf("task %d called %llu times (%llu nodes)\n", i, statistics.n_task[i], statistics.n_task_nodes[i]);
		}
		printf("\n\n");
	}

	if (statistics.n_PVS_root) {
		printf("Search:\n");
		printf("PVS_root          = %12llu\n", statistics.n_PVS_root);
		printf("PVS+NWS_midgame   = %12llu + %12llu\n", statistics.n_PVS_midgame, statistics.n_NWS_midgame);
		printf("PVS+NWS_shallow   = %12llu + %12llu\n", statistics.n_PVS_shallow, statistics.n_NWS_shallow);
		printf("search_eval_2     = %12llu\n", statistics.n_search_eval_2);
		printf("search_eval_1     = %12llu\n", statistics.n_search_eval_1);
		printf("search_eval_0     = %12llu\n\n", statistics.n_search_eval_0);
		printf("NWS_endgame       = %12llu\n", statistics.n_NWS_endgame);
		printf("NWS_solve_4       = %12llu\n", statistics.n_search_solve_4);
		printf("NWS_solve_3       = %12llu\n", statistics.n_search_solve_3);
		printf("NWS_solve_2       = %12llu\n", statistics.n_board_solve_2);
		printf("search_solve_0    = %12llu\n", statistics.n_search_solve_0);
		printf("search_solve      = %12llu\n\n\n", statistics.n_search_solve);
	}

	if (statistics.n_hash_found) {
		printf("HashTable:\n");
		printf("Probe: %llu   found: %llu (%6.2f%%)\n", statistics.n_hash_search, statistics.n_hash_found, 100.0 * statistics.n_hash_found / statistics.n_hash_search);
		printf("New: %llu   Update: %llu   Ugrade: %llu   Remove: %llu\n",
			statistics.n_hash_new, statistics.n_hash_update, statistics.n_hash_upgrade, statistics.n_hash_remove);
	}

	if (SQUARE_STATS(1) +0) {
		for (j = 0; j < BOARD_SIZE; ++j) {
			printf("\n%2d: ", j);
			for (i = 0; i < 9; ++i) {
				if (statistics.n_played_square[j][i])
					printf("[%d] = %.1f, ", i, 100.0 * statistics.n_good_square[j][i]/statistics.n_played_square[j][i]);
			}
		}
		printf("\n\n");
	}

	if (CUTOFF_STATS(1) +0) {
		if (statistics.n_hash_try) {
			printf("Transposition cutoff:\n");
			printf("try = %llu, low cutoff = %llu (%6.2f%%), high cutoff = %llu (%6.2f%%)\n",
				statistics.n_hash_try,
				statistics.n_hash_low_cutoff, 100.0 * statistics.n_hash_low_cutoff / statistics.n_hash_try,
				statistics.n_hash_high_cutoff, 100.0 * statistics.n_hash_high_cutoff / statistics.n_hash_try);
		}
		if (statistics.n_stability_try) {
			printf("Stability cutoff:\n");
			printf("try = %llu, low cutoff = %llu (%6.2f%%)\n",
				statistics.n_stability_try,
				statistics.n_stability_low_cutoff, 100.0 * statistics.n_stability_low_cutoff / statistics.n_stability_try);
		}
		if (statistics.n_etc_try) {
			printf("(E)nhance (T)ransposition & (S)tability (C)utoff:\n");
			printf("try = %llu, high ETC = %llu (%6.2f%%), high ESC = %llu (%6.2f%%)\n",
				statistics.n_etc_try,
				statistics.n_etc_high_cutoff, 100.0 * statistics.n_etc_high_cutoff / statistics.n_etc_try,
				statistics.n_esc_high_cutoff, 100.0 * statistics.n_esc_high_cutoff / statistics.n_etc_try);
		}
		printf("\n\n");
	}

	if (statistics.n_probcut_try) {
		printf("Probcut:\n");
		printf("\ttry = %llu,\n\tlow cutoff = %llu try (%6.2f%%) %llu success (%6.2f%% (%6.2f%%)),\n\thigh cutoff = %llu try (%6.2f%%) %llu success (%6.2f%% (%6.2f%%))\n",
			statistics.n_probcut_try,
			statistics.n_probcut_low_try, 100.0 * statistics.n_probcut_low_try / statistics.n_probcut_try,
			statistics.n_probcut_low_cutoff, 100.0 * statistics.n_probcut_low_cutoff / statistics.n_probcut_try, 100.0 * statistics.n_probcut_low_cutoff / statistics.n_probcut_low_try,
			statistics.n_probcut_high_try, 100.0 * statistics.n_probcut_high_try / statistics.n_probcut_try,
			statistics.n_probcut_high_cutoff, 100.0 * statistics.n_probcut_high_cutoff / statistics.n_probcut_try, 100.0 * statistics.n_probcut_high_cutoff / statistics.n_probcut_high_try);
	}

	if (statistics.n_concurrent_garbage) {
		printf("Concurrency garbage detected = %llu\n\n\n", statistics.n_concurrent_garbage);
	}

	if (statistics.n_NWS_candidate) {
		printf("NWS candidate as best root move:\n");
		printf("Candidate: %llu, Best Move: %llu (%6.2f%%), Bad Candidate: %llu (%6.2f%%)\n",
			statistics.n_NWS_candidate,
			statistics.n_NWS_candidate - statistics.n_NWS_bad_candidate, (100.0 * (statistics.n_NWS_candidate - statistics.n_NWS_bad_candidate) / statistics.n_NWS_candidate),
			statistics.n_NWS_bad_candidate, (100.0 * statistics.n_NWS_bad_candidate / statistics.n_NWS_candidate)
		);
	}
}

