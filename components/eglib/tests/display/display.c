#define _GNU_SOURCE
#include <check.h>
#include <eglib.h>
#include <eglib/display.h>
#include <eglib/display/frame_buffer.h>
#include <eglib/hal/four_wire_spi/stream.h>
#include <eglib/hal/i2c/stream.h>
#include <eglib/hal/parallel_8_bit_8080/stream.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../common.h"

#define EGLIB_UPDATE_EXPECTATIONS_MSG "If you trust the code is legit set "\
	"EGLIB_UPDATE_EXPECTATIONS=true to have the expectation updated.\n"

eglib_t eglib;
const hal_t *hal_driver = NULL;
four_wire_spi_stream_config_t four_wire_spi_stream_config;
i2c_stream_config_t i2c_stream_config;
parallel_8_bit_8080_stream_config_t parallel_8_bit_8080_stream_config;
void *hal_config_ptr = NULL;
FILE *stream = NULL;
char **buffer = NULL;
char *test_case_name = NULL;
char *test_name = NULL;

extern const display_t *display_driver;
extern void *display_config_ptr;
extern coordinate_t width;
extern coordinate_t height;
extern enum pixel_format_t pixel_format;
extern char driver_name[];
char *reldir = RELDIR;

void setup_four_wire_spi(void);
void setup_i2c(void);
void setup_parallel_8_bit_8080(void);
void teardown(void);
void tcase_add_common_tests(TCase *tcase);
void tcase_add_extra_tests(TCase *tcase);
Suite * build_suite(void);

void setup_four_wire_spi(void) {
	test_case_name = "four_wire_spi";
	hal_driver = &four_wire_spi_stream;
	hal_config_ptr = &four_wire_spi_stream_config;
	eglib_Init(
		&eglib,
		hal_driver, hal_config_ptr,
		display_driver, display_config_ptr
	);
	stream = four_wire_spi_stream_config.stream;
	buffer = &(four_wire_spi_stream_config.buffer);
}

void setup_i2c(void) {
	test_case_name = "i2c";
	hal_driver = &i2c_stream;
	hal_config_ptr = &i2c_stream_config;
	eglib_Init(
		&eglib,
		hal_driver, hal_config_ptr,
		display_driver, display_config_ptr
	);
	stream = i2c_stream_config.stream;
	buffer = &(i2c_stream_config.buffer);
}

void setup_parallel_8_bit_8080(void) {
	test_case_name = "parallel_8_bit_8080";
	hal_driver = &parallel_8_bit_8080_stream;
	hal_config_ptr = &parallel_8_bit_8080_stream_config;
	eglib_Init(
		&eglib,
		hal_driver, hal_config_ptr,
		display_driver, display_config_ptr
	);
	stream = parallel_8_bit_8080_stream_config.stream;
	buffer = &(parallel_8_bit_8080_stream_config.buffer);
}

START_TEST(init) {
	test_name = "init";
}END_TEST

START_TEST(sleep_in) {
	test_name = "sleep_in";
	eglib_SleepIn(&eglib);
}END_TEST

START_TEST(sleep_out) {
	test_name = "sleep_out";
	eglib_SleepOut(&eglib);
}END_TEST

START_TEST(get_dimension) {
	test_name = "get_dimension";
	coordinate_t read_width, read_height;

	eglib_GetDimension(&eglib, &read_width, &read_height);

	ck_assert_int_eq(read_width, width);
	ck_assert_int_eq(read_height, height);
}END_TEST

START_TEST(get_pixel_format) {
	test_name = "get_pixel_format";
	enum pixel_format_t read_pixel_format;

	eglib_GetPixelFormat(&eglib, &read_pixel_format);

	ck_assert_int_eq(read_pixel_format, pixel_format);
}END_TEST

START_TEST(draw_pixel_color) {
	test_name = "draw_pixel_color";
	eglib_SetIndexColor(&eglib, 0, 255, 0, 0);
	eglib_DrawPixel(&eglib, 0, 0);
	eglib_SetIndexColor(&eglib, 0, 0, 255, 0);
	eglib_DrawPixel(&eglib, eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2);
	eglib_SetIndexColor(&eglib, 0, 0, 0, 255);
	eglib_DrawPixel(&eglib, eglib_GetWidth(&eglib)-1, eglib_GetHeight(&eglib)-1);
}END_TEST

START_TEST(draw_line) {
	test_name = "draw_line";
	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);

	eglib_DrawLine(
		&eglib,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2,
		eglib_GetWidth(&eglib)/2, 0
	);
	eglib_DrawLine(
		&eglib,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)-1
	);
	eglib_DrawLine(
		&eglib,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2,
		eglib_GetWidth(&eglib)-1, eglib_GetHeight(&eglib)/2
	);
	eglib_DrawLine(
		&eglib,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2,
		0, eglib_GetHeight(&eglib)/2
	);
}END_TEST

START_TEST(send_buffer) {
	test_name = "send_buffer";
	frame_buffer_config_t frame_buffer_config;

	fclose(stream);
	free(*buffer);

	eglib_Init_FrameBuffer(
		&eglib, &frame_buffer_config,
		hal_driver, hal_config_ptr,
		display_driver, display_config_ptr
	);

	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_DrawLine(
		&eglib,
		0, 0,
		eglib_GetWidth(&eglib)-1, eglib_GetHeight(&eglib)-1
	);
	eglib_FrameBuffer_SendPartial(
		&eglib,
		eglib_GetWidth(&eglib)/4, eglib_GetHeight(&eglib)/4,
		eglib_GetWidth(&eglib)/2, eglib_GetHeight(&eglib)/2
	);
}END_TEST

START_TEST(refresh) {
	test_name = "refresh";
	eglib_SetIndexColor(&eglib, 0, 255, 255, 255);
	eglib_DrawLine(
		&eglib,
		0, 0,
		eglib_GetWidth(&eglib)-1, eglib_GetHeight(&eglib)-1
	);
	while(eglib_Refresh(&eglib));
}END_TEST

void teardown(void) {
	char *expectation_dir;
	char *file;
	char *expectation_path;
	int expectation_fd;
	struct stat expectation_stat;
	char *expectation_ptr;
	ssize_t buffer_len;
	char *expectation_error;

	fclose(stream);
	buffer_len = strlen(*buffer);



	ck_assert_ptr_ne(driver_name, NULL);
	ck_assert_ptr_ne(test_case_name, NULL);
	ck_assert_ptr_ne(test_name, NULL);

	file = malloc(strlen(__FILE__) + 1);
	if(NULL == file)
		exit(EXIT_FAILURE);
	strcpy(file, __FILE__);
	expectation_dir = dirname(file);

	if(asprintf(&expectation_path, "%s/%s.%s.%s", expectation_dir, driver_name, test_case_name, test_name) == -1)
		exit(EXIT_FAILURE);
	free(file);

	if(getenv("EGLIB_UPDATE_EXPECTATIONS")) {
		fprintf(stderr, "Updating expectation `%s' %ld bytes.\n", expectation_path, buffer_len);
		expectation_fd = open(expectation_path, O_WRONLY | O_TRUNC| O_CREAT, 0644);
		if(expectation_fd < 0) {
			fprintf(stderr, "Failed to open `%s': %s.\n", expectation_path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(buffer_len != write(expectation_fd, *buffer, buffer_len)) {
			fprintf(stderr, "Failed to write to `%s': %s.\n", expectation_path, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(close(expectation_fd)<0) {
			fprintf(stderr, "Failed to close `%s': %s.\n", expectation_path, strerror(errno));
			exit(EXIT_FAILURE);
		};
	}

	expectation_fd = open(expectation_path, O_RDONLY);
	if(expectation_fd < 0) {
		fprintf(
			stderr,
			"Failed to open `%s': %s.\n" EGLIB_UPDATE_EXPECTATIONS_MSG,
			expectation_path,
			strerror(errno)
		);
		exit(EXIT_FAILURE);
	}

	if(fstat(expectation_fd, &expectation_stat) < 0) {
		fprintf(stderr, "Failed to fstat `%s': %s.\n", expectation_path, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(buffer_len != expectation_stat.st_size) {
		fprintf(
			stderr,
			"Expectation `%s' is %ld bytes and test output %ld bytes.\n"
			EGLIB_UPDATE_EXPECTATIONS_MSG,
			expectation_path,
			expectation_stat.st_size, buffer_len
		);
		exit(EXIT_FAILURE);
	}

	expectation_ptr = mmap(
		NULL,
		expectation_stat.st_size,
		PROT_READ,
		MAP_SHARED,
		expectation_fd,
		0
	);
	if(expectation_ptr == MAP_FAILED) {
		fprintf(stderr, "Failed to map `%s': %s.\n", expectation_path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	close(expectation_fd);

	if(
		asprintf(
			&expectation_error,
			"Expectation `%s` does not match test output.\n"
			EGLIB_UPDATE_EXPECTATIONS_MSG,
			expectation_path
		) == -1
	)
		exit(EXIT_FAILURE);
	ck_assert_msg(0==memcmp(*buffer, expectation_ptr, buffer_len), expectation_error);

    if(munmap(expectation_ptr, expectation_stat.st_size)) {
		fprintf(stderr, "Failed to munmap `%s': %s.\n", expectation_path, strerror(errno));
		exit(EXIT_FAILURE);
    }
	free(expectation_path);
	free(expectation_error);
	free(*buffer);
}

void tcase_add_common_tests(TCase *tcase) {
	tcase_add_test(tcase, init);
	tcase_add_test(tcase, sleep_in);
	tcase_add_test(tcase, sleep_out);
	tcase_add_test(tcase, get_dimension);
	tcase_add_test(tcase, get_pixel_format);
	tcase_add_test(tcase, draw_pixel_color);
	tcase_add_test(tcase, draw_line);
	tcase_add_test(tcase, send_buffer);
	tcase_add_test(tcase, refresh);
}

Suite * build_suite(void) {
	Suite *suite;

	suite = suite_create("display");

	if(display_driver->comm.four_wire_spi != NULL) {
		TCase *tcase;

		tcase = tcase_create("four_wire_spi");
		tcase_add_checked_fixture(tcase, setup_four_wire_spi, teardown);
		tcase_add_common_tests(tcase);
		tcase_add_extra_tests(tcase);

		suite_add_tcase(suite, tcase);
	}

	if(display_driver->comm.i2c != NULL) {
		TCase *tcase;

		tcase = tcase_create("i2c");
		tcase_add_checked_fixture(tcase, setup_i2c, teardown);
		tcase_add_common_tests(tcase);
		tcase_add_extra_tests(tcase);

		suite_add_tcase(suite, tcase);
	}

	if(display_driver->comm.parallel_8_bit_8080 != NULL) {
		TCase *tcase;

		tcase = tcase_create("parallel_8_bit_8080");
		tcase_add_checked_fixture(tcase, setup_parallel_8_bit_8080, teardown);
		tcase_add_common_tests(tcase);
		tcase_add_extra_tests(tcase);

		suite_add_tcase(suite, tcase);
	}

	return suite;
}