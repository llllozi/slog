#include "../slog.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <time.h>
typedef struct slog_node slog_node;
void test_basic_types() {
	SLOG_FREE();
	slog_node *node;
	// 1. Test INT
	node = SLOG_INT("age", 12);
	CU_ASSERT_EQUAL(12, node->value.integer);
	CU_ASSERT_EQUAL(SLOG_TYPE_INT, node->type);
	CU_ASSERT_STRING_EQUAL("age", node->key);
	slog_node_put(node);
	// 2. Test BOOL
	node = SLOG_BOOL("is_active", true);
	CU_ASSERT_STRING_EQUAL("is_active", node->key);
	CU_ASSERT_EQUAL(SLOG_TYPE_BOOL, node->type);
	CU_ASSERT_TRUE(node->value.boolean);
	slog_node_put(node);

	// 3. Test FLOAT
	node = SLOG_FLOAT("pi", 3.14159);
	CU_ASSERT_EQUAL(SLOG_TYPE_FLOAT, node->type);
	CU_ASSERT_DOUBLE_EQUAL(3.14159, node->value.number, 0.00001);
	slog_node_put(node);

	// 4. Test STRING
	node = SLOG_STRING("name", "archlinux");
	CU_ASSERT_STRING_EQUAL("archlinux", node->value.string);
	slog_node_put(node);
	SLOG_FREE();
}
void test_others_type() {
	SLOG_FREE();
	slog_node *node;
	// Test array
	node = SLOG_ARRAY("scores", SLOG_INT("a", 100), SLOG_INT("b", 98));
	// head
	CU_ASSERT_PTR_NOT_NULL_FATAL(node);
	CU_ASSERT_STRING_EQUAL("scores", node->key);
	CU_ASSERT_EQUAL(SLOG_TYPE_ARRAY, node->type);
	// son1

	CU_ASSERT_PTR_NOT_NULL_FATAL(node->value.array);
	CU_ASSERT_PTR_NULL(node->value.array->key);
	CU_ASSERT_EQUAL(100, node->value.array->value.integer);
	// son2
	CU_ASSERT_PTR_NOT_NULL_FATAL(node->value.array->next);
	CU_ASSERT_PTR_NULL(node->value.array->next->key);
	CU_ASSERT_EQUAL(98, node->value.array->next->value.integer);
	slog_node_put(node);
	// test object node
	node = SLOG_OBJECT("scores", SLOG_INT("math", 80),
			   SLOG_INT("english", 90));

	CU_ASSERT_PTR_NOT_NULL_FATAL(node);
	CU_ASSERT_STRING_EQUAL("scores", node->key);
	// son1
	CU_ASSERT_PTR_NOT_NULL_FATAL(node->value.object);
	CU_ASSERT_STRING_EQUAL("math", node->value.object->key);
	CU_ASSERT_EQUAL(80, node->value.object->value.integer);
	// son2
	CU_ASSERT_PTR_NOT_NULL_FATAL(node->value.object->next);
	CU_ASSERT_STRING_EQUAL("english", node->value.object->next->key);
	CU_ASSERT_EQUAL(90, node->value.object->next->value.integer);
	CU_ASSERT_PTR_NULL(node->value.object->next->next);
	// test end
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
	if (NULL == CU_add_test(pSuite, "test basic node", test_basic_types) ||
	    (NULL == CU_add_test(pSuite, "test array and object node ",
				 test_others_type))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
