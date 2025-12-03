#include "slog.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
void test_is_allocate() {
	SLOG_FREE();
	// test allocate
	struct slog_node *node = slog_node_get();
	CU_ASSERT_PTR_NOT_NULL_FATAL(node);
	// test recycle
	struct slog_node *firstNode = node;
	slog_node_put(node);
	node = slog_node_get();
	CU_ASSERT_PTR_EQUAL(firstNode, node);
	// test free
	slog_node_put(node);
	SLOG_FREE();
	CU_ASSERT_PTR_NULL(slog_node_thread_local);
}
void test_reset() {
	SLOG_FREE();
	// Test node reset after reuse
	struct slog_node *node = slog_node_get();
	node->key = "hacker";
	slog_node_put(node);
	node = slog_node_get();
	CU_ASSERT_PTR_NULL(node->key);
	slog_node_put(node);
	SLOG_FREE();
}
void test_nullPtr_safety() {
	SLOG_FREE();
	slog_node_put(NULL);
	struct slog_node *node = slog_node_get();
	CU_ASSERT_PTR_NOT_NULL(node);
	slog_node_put(node);
	SLOG_FREE();
}

int init_suite() {
	SLOG_FREE();
	return 0;
}
int clean_suite(void) {
	SLOG_FREE();
	return 0;
}

int main() {
	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}
	pSuite = CU_add_suite("mem", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL ==
	     CU_add_test(pSuite, "test node allocate", test_is_allocate)) ||
	    (NULL == CU_add_test(pSuite, "test node reset", test_reset)) ||
	    (NULL == CU_add_test(pSuite, "test null ptr safety",
				 test_nullPtr_safety))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
