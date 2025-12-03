#include "slog.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdbool.h>
#include <time.h>
// TEST_WRITE_NODE(slog_node_create(TYPE,key,val),expected)
static char handler_buffer[4096];
#define TEST_WRITE_NODE(NODE, EXPECTED)                                        \
	do {                                                                   \
		struct slog_node *_node = NODE;                                \
		CU_ASSERT_PTR_NOT_NULL_FATAL(_node);                           \
		slog_write_node(_node);                                        \
		CU_ASSERT_STRING_EQUAL(slog_buffer_write(NULL), EXPECTED);     \
		SLOG_FREE();                                                   \
	} while (0)
// endif
//
// TEST_STRING_ESCAPE(KEY,VALUE,EXPECTED)
#define TEST_STRING_ESCAPE(KEY, VALUE, EXPECTED)                               \
	do {                                                                   \
		struct slog_node *_node =                                      \
			slog_node_create(SLOG_TYPE_STRING, KEY, VALUE);        \
		CU_ASSERT_PTR_NOT_NULL_FATAL(_node);                           \
		slog_write_node(_node);                                        \
		CU_ASSERT_STRING_EQUAL(slog_buffer_write(NULL), EXPECTED);     \
		SLOG_FREE();                                                   \
	} while (0)
// endif
int init_suite(void) {
	SLOG_FREE();
	return 0;
}
int clean_suite(void) {
	SLOG_FREE();
	return 0;
}

void test_basic_node(void) {

	// test SLOG_TYPE_INT

	TEST_WRITE_NODE(slog_node_create(SLOG_TYPE_INT, "age", (long long) 10),
			"\"age\":10");

	// zero
	TEST_WRITE_NODE(slog_node_create(SLOG_TYPE_INT, "age", (long long) 0),
			"\"age\":0");

	// negative
	TEST_WRITE_NODE(slog_node_create(SLOG_TYPE_INT, "age", (long long) -5),
			"\"age\":-5");

	// bool
	TEST_WRITE_NODE(slog_node_create(SLOG_TYPE_BOOL, "bool", true),
			"\"bool\":true");
	// float

	TEST_WRITE_NODE(SLOG_FLOAT("pi", 3.14159), "\"pi\":3.141590");

	// .000000
	TEST_WRITE_NODE(SLOG_FLOAT("height", 180.0), "\"height\":180.000000");

	// 3.1415926 -> 3.141593
	TEST_WRITE_NODE(SLOG_FLOAT("round", 3.1415926), "\"round\":3.141593");
	// test array
	TEST_WRITE_NODE(
		slog_node_create(
			SLOG_TYPE_ARRAY, "array",
			slog_node_create(SLOG_TYPE_INT, "int", (long long) 10),
			slog_node_create(SLOG_TYPE_BOOL, "bool", true), NULL),
		"\"array\":[10,true]");
	// test object
	TEST_WRITE_NODE(
		slog_node_create(
			SLOG_TYPE_OBJECT, "object",
			slog_node_create(SLOG_TYPE_INT, "int", (long long) 10),
			slog_node_create(SLOG_TYPE_BOOL, "bool", true), NULL),
		"\"object\":{\"int\":10,\"bool\":true}");
}
void test_escape(void) {
	SLOG_FREE();

	// 1. Test Double Quote (")
	// Input: "  -> Output: \"
	TEST_STRING_ESCAPE("test", "\"", "\"test\":\"\\\"\"");

	// 2. Test Backslash (\)
	// Input: \  -> Output: \\ .
	TEST_STRING_ESCAPE("path", "\\", "\"path\":\"\\\\\"");

	// 3. Test Newline (\n)
	TEST_STRING_ESCAPE("msg", "\n", "\"msg\":\"\\n\"");

	// 4. Test Carriage Return (\r)
	TEST_STRING_ESCAPE("key", "\r", "\"key\":\"\\r\"");

	// 5. Test Horizontal Tab (\t)
	TEST_STRING_ESCAPE("key", "\t", "\"key\":\"\\t\"");

	// 6. Test Backspace (\b)
	TEST_STRING_ESCAPE("key", "\b", "\"key\":\"\\b\"");

	// 7. Test Form Feed (\f)
	TEST_STRING_ESCAPE("key", "\f", "\"key\":\"\\f\"");
	// 8. Test ASCII CODE 32 less
	TEST_STRING_ESCAPE("key", "\v", "\"key\":\"\\u000b\"");
	TEST_STRING_ESCAPE("key", "\x1B", "\"key\":\"\\u001b\"");
	// normal char
	TEST_STRING_ESCAPE("key", "abc", "\"key\":\"abc\"");
}
void test_time(void) {
	SLOG_FREE();

	struct slog_node *node = slog_node_create(SLOG_TYPE_TIME, "time");
	CU_ASSERT_PTR_NOT_NULL_FATAL(node);

	node->value.time.tv_sec = 1763456783;
	node->value.time.tv_nsec = 899468000;

	slog_buffer_write(NULL);
	slog_write_node(node);

	CU_ASSERT_STRING_EQUAL(slog_buffer_write(NULL),
			       "\"time\":\"1763456783.899468\"");

	node = slog_node_create(SLOG_TYPE_TIME, "time");
	CU_ASSERT_PTR_NOT_NULL_FATAL(node);

	node->value.time.tv_sec = 1763456783;
	node->value.time.tv_nsec = 5000;
	slog_buffer_write(NULL);
	slog_write_node(node);
	CU_ASSERT_STRING_EQUAL(slog_buffer_write(NULL),
			       "\"time\":\"1763456783.000005\"");

	SLOG_FREE();
}
void for_handler(const char *log) {
	strncpy(handler_buffer, log, sizeof(handler_buffer) - 1);
}
void test_macro_and_handler(void) {
	memset(handler_buffer, 0, sizeof(handler_buffer));
	SLOG_SET_HANDLER(for_handler);
	SLOG(INFO, "User login", SLOG_INT("uid", 10086),
	     SLOG_STRING("status", "active"));
	CU_ASSERT_TRUE(strlen(handler_buffer) > 0);
	CU_ASSERT_PTR_NOT_NULL(strstr(handler_buffer, "\"level\":\"INFO\""));
	CU_ASSERT_PTR_NOT_NULL(
		strstr(handler_buffer, "\"msg\":\"User login\""));

	CU_ASSERT_PTR_NOT_NULL(
		strstr(handler_buffer, "\"func\":\"test_macro_and_handler\""));

	CU_ASSERT_PTR_NOT_NULL(strstr(handler_buffer, "\"uid\":10086"));
	CU_ASSERT_PTR_NOT_NULL(strstr(handler_buffer, "\"status\":\"active\""));

	SLOG_FREE();
}
int main(void) {
	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}

	pSuite = CU_add_suite("format_output", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(pSuite, "test basic node construction",
				 test_basic_node)) ||
	    (NULL == CU_add_test(pSuite, "test string escape sequences",
				 test_escape)) ||
	    (NULL == CU_add_test(pSuite, "test time formatting", test_time)) ||
	    (NULL == CU_add_test(pSuite, "test macro and handler integration",
				 test_macro_and_handler))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
