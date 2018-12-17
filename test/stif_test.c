#include <check.h>
#include <stdlib.h>

#include "../include/stif.h"

// Check if buffer is null
START_TEST(read_stif_block_buffer_null)
{
    size_t bytes_read = 0;
    stif_block_t *block = NULL;
    block = read_stif_block(NULL, 100, &bytes_read);

    ck_assert(block == NULL);

    stif_block_free(block);
}
END_TEST

// Check if buffer is too small
START_TEST(read_stif_block_buffer_too_small)
{
    size_t bytes_read = 0;
    unsigned char buffer[] = "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00";
    stif_block_t *block = NULL;
    block = read_stif_block(buffer, 4, &bytes_read);

    ck_assert(block == NULL);

    stif_block_free(block);
}
END_TEST

// Test several features of read_stif_block
START_TEST(read_stif_block_general)
{
    stif_block_t *block;

    size_t bytes_read = 0;
    unsigned char header[] = "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00";
    block = read_stif_block(header, 100, &bytes_read);

    ck_assert(block->block_type == STIF_BLOCK_TYPE_HEADER);
    ck_assert(block->block_size == 9);
    ck_assert(bytes_read == 14);
    stif_block_free(block);

    unsigned char data[] = "\x01\x01\x00\x00\x00\xFF";
    block = read_stif_block(data, 10, &bytes_read);

    ck_assert(block->block_type == STIF_BLOCK_TYPE_DATA);
    ck_assert(block->block_size == 1);
    ck_assert(block->data[0] == 0xFF);
    ck_assert(bytes_read == 6);
    stif_block_free(block);
}
END_TEST

START_TEST(parse_stif_buffer_null)
{
    stif_t *stif = NULL;
    stif = parse_stif(NULL, 100);
    ck_assert(stif == NULL);
    stif_free(stif);
}
END_TEST

START_TEST(parse_stif_magic)
{
    stif_t *stif;

    unsigned char good_buffer[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00"
        "\x01\x01\x00\x00\x00\xFF";

    stif = parse_stif(good_buffer, 22);
    ck_assert(stif != NULL);

    unsigned char bad_buffer[] = "\xCA\xFE"
        "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00"
        "\x01\x01\x00\x00\x00\xFF";

    stif = parse_stif(bad_buffer, 22);
    ck_assert(stif == NULL);
}
END_TEST

START_TEST(parse_stif_color_type)
{
    stif_t *stif;

    unsigned char gray_buffer[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00"
        "\x01\x01\x00\x00\x00\xFF";

    stif = parse_stif(gray_buffer, 22);
    ck_assert(stif != NULL);
    ck_assert((stif->header).color_type == STIF_COLOR_TYPE_GRAYSCALE);
    stif_free(stif);

    unsigned char rgb_buffer[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x00\x01"
        "\x01\x09\x00\x00\x00\xff\x00\x00\x00\xff\x00\x00\x00\xff";

    stif = parse_stif(rgb_buffer, 30);
    ck_assert(stif != NULL);
    ck_assert((stif->header).color_type == STIF_COLOR_TYPE_RGB);
    stif_free(stif);

/*    unsigned char bad_buffer[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x00\x02"
        "\x01\x09\x00\x00\x00\xff\x00\x00\x00\xff\x00\x00\x00\xff";

    stif = parse_stif(bad_buffer, 30);
    ck_assert(stif == NULL);
    stif_free(stif);*/
}
END_TEST

START_TEST(parse_stif_size)
{
    stif_t *stif;

    unsigned char good_size[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00"
        "\x01\x01\x00\x00\x00\x01";

    stif = parse_stif(good_size, 22);
    ck_assert_int_eq((stif->header).width, 1);
    ck_assert_int_eq((stif->header).height, 1);

    unsigned char other_good_size[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x05\x00\x00\x00\x02\x00\x00\x00\x00"
        "\x01\x05\x00\x00\x00\x01\x02\x03\x04\x05"
        "\x01\x05\x00\x00\x00\x01\x02\x03\x04\x05";

    stif = parse_stif(other_good_size, 36);
    ck_assert_int_eq((stif->header).width, 5);
    ck_assert_int_eq((stif->header).height, 2);

   unsigned char bad_size[] = "\xFE\xCA"
        "\x00\x09\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00"
        "\x01\x01\x00\x00\x00\x01";

    stif = parse_stif(bad_size, 22);
    ck_assert(stif == NULL);
}
END_TEST

static Suite *parse_suite(void)
{
    Suite *s;
    TCase *tc_read_stif;
    TCase *tc_parse_stif;

    /* Core test case */
    s = suite_create("stif");

    /* read_stif */
    tc_read_stif = tcase_create("read_stif");

    tcase_add_test(tc_read_stif, read_stif_block_buffer_null);
    tcase_add_test(tc_read_stif, read_stif_block_buffer_too_small);
    tcase_add_test(tc_read_stif, read_stif_block_general);

    /* parse_stif */
    tc_parse_stif = tcase_create("parse_stif");

    tcase_add_test(tc_parse_stif, parse_stif_buffer_null);
    tcase_add_test(tc_parse_stif, parse_stif_magic);
    tcase_add_test(tc_parse_stif, parse_stif_color_type);
    tcase_add_test(tc_parse_stif, parse_stif_size);

    suite_add_tcase(s, tc_read_stif);
    suite_add_tcase(s, tc_parse_stif);

    return s;
}

int main(void) {
    int number_failed = 0;
    Suite *s;
    SRunner *runner;

    s = parse_suite();
    runner = srunner_create(s);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
