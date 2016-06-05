/**
 * Copyright © 2016 Dr. Tobias Quathamer <toddy@debian.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <string.h>
#include <unistd.h>

// Define the path to the executable for integration testing.
#define ISOQUERY_CALL "../src/isoquery", "../src/isoquery", "-p", "data"

/**
 * Test all codes for ISO standard.
 */
void test_integration_add_test_from_files(gconstpointer data)
{
    gchar *path_prefix = (gchar *) data;
    gchar *filename;
    GError *error = NULL;
    gchar *commandline = NULL;
    gchar *expected_stdout = NULL;
    gchar **commandline_args = NULL;

    if (g_test_subprocess()) {
        // Get the commandline
        filename = g_strdup_printf("%s_test_commandline.txt", path_prefix);
        g_file_get_contents(filename, &commandline, NULL, &error);
        g_assert_null(error);
        g_assert_nonnull(commandline);
        g_free(filename);

        // Set up arguments and execute program
        commandline_args = g_strsplit(g_strchomp(commandline), " ", -1);
        execv("../src/isoquery", commandline_args);
        g_strfreev(commandline_args);

        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();

    // Get the expected output on stdout
    filename = g_strdup_printf("%s_test_stdout.txt", path_prefix);
    g_file_get_contents(filename, &expected_stdout, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_stdout);
    g_free(filename);

    g_test_trap_assert_stdout(expected_stdout);
    g_test_trap_assert_stderr("");
}

/**
 * Test single code on command line
 */
void test_integration_single_code(void)
{
    gchar *expected_output = NULL;
    GError *error = NULL;
    g_file_get_contents("expected/iso_3166-1_single_code.txt", &expected_output, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_output);

    if (g_test_subprocess()) {
        execl(ISOQUERY_CALL, "TV", NULL);
        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();
    g_test_trap_assert_stdout(expected_output);
    g_test_trap_assert_stderr("");
}

/**
 * Test multiple invalid codes on command line
 */
void test_integration_invalid_codes(void)
{
    gchar *expected_output = NULL;
    gchar *expected_errors = NULL;
    GError *error = NULL;
    g_file_get_contents("expected/iso_3166-1_multiple_codes.txt", &expected_output, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_output);
    g_file_get_contents("expected/iso_3166-1_multiple_codes_errors.txt", &expected_errors, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_errors);

    if (g_test_subprocess()) {
        execl(ISOQUERY_CALL, "TV", "invalid", "öllö", "Deu", "643", "1234", "007", "URG", "fra", NULL);
        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();
    g_test_trap_assert_stdout(expected_output);
    g_test_trap_assert_stderr(expected_errors);
}

/**
 * Test multiple codes localized on command line
 */
void test_integration_multiple_codes_localized(void)
{
    gchar *expected_output = NULL;
    GError *error = NULL;
    g_file_get_contents("expected/iso_3166-1_multiple_codes_localized.txt", &expected_output, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_output);

    if (g_test_subprocess()) {
        execl(ISOQUERY_CALL, "--locale", "de", "ua", "frA", "158", NULL);
        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();
    g_test_trap_assert_stdout(expected_output);
    g_test_trap_assert_stderr("");
}

/**
 * Test multiple invalid codes localized on command line
 */
void test_integration_invalid_codes_localized(void)
{
    gchar *expected_output = NULL;
    gchar *expected_errors = NULL;
    GError *error = NULL;
    g_file_get_contents("expected/iso_3166-1_multiple_codes_localized.txt", &expected_output, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_output);
    g_file_get_contents("expected/iso_3166-1_multiple_codes_errors.txt", &expected_errors, NULL, &error);
    g_assert_null(error);
    g_assert_nonnull(expected_errors);

    if (g_test_subprocess()) {
        execl(ISOQUERY_CALL, "--locale", "de", "invalid", "öllö", "ua", "frA", "158", "1234", "007", "URG", NULL);
        return;
    }
    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();
    g_test_trap_assert_stdout(expected_output);
    g_test_trap_assert_stderr(expected_errors);
}

/**
 * Initializing all test functions
 */
int main(int argc, gchar * argv[])
{
    gchar *test_directories[] =
        { "iso_639-2", "iso_639-3", "iso_639-5", "iso_3166-1", "iso_3166-2", "iso_3166-3", "iso_4217", "iso_15924",
        NULL
    };
    gchar *pathname, *testpath, *testname;
    const gchar *filename;
    GDir *testdir;
    GError *error = NULL;
    GList *testfiles = NULL;
    int i = 0;

    g_test_init(&argc, &argv, NULL);

    // Add common tests for various standards
    while (test_directories[i]) {
        // Open the directory for the current standard
        testdir = g_dir_open(g_strdup_printf("expected/%s", test_directories[i]), 0, &error);
        g_assert_null(error);
        g_assert_nonnull(testdir);

        // Read in all files
        while (filename = g_dir_read_name(testdir)) {
            // Only add files for command lines, the stdout files
            // will be read by the test; the stderr files are optional.
            if (g_str_has_suffix(filename, "_test_commandline.txt")) {
                testfiles = g_list_prepend(testfiles, g_strdup(filename));
            }
        }
        testfiles = g_list_sort(testfiles, (GCompareFunc) * g_strcmp0);
        g_dir_close(testdir);

        // Add all test files to the queue
        for (GList * l = testfiles; l != NULL; l = l->next) {
            // Remove the file extension and test description to get a name
            testname = g_strndup(l->data, strlen(l->data) - strlen("_test_commandline.txt"));
            testpath = g_strdup_printf("/integration/%s/%s", test_directories[i], testname);

            // Construct the pathname to the file
            pathname = g_strdup_printf("expected/%s/%s", test_directories[i], testname);
            g_test_add_data_func(testpath, pathname, test_integration_add_test_from_files);
            g_free(testname);
            g_free(testpath);
        }
        g_list_free_full(testfiles, *g_free);
        testfiles = NULL;
        i++;
    }

    return g_test_run();
}
