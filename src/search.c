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
#include "search.h"
#include "options.h"

/**
 * Search for a given code
 */
void search_entry(gchar * code)
{
    // Search for each code
    int i = 0;
    gchar *normalized_code;
    while (codes[i]) {
        // Reset the list for each entry
        list_entry = g_list_first(entries_list);
        normalized_code = search_get_normalized_code(codes[i]);
        // Cycle through all entries
        while (list_entry) {
            entry = json_node_get_object(list_entry->data);
            isocodes_show_entry(entry);
            list_entry = g_list_next(list_entry);
        }
        g_free(normalized_code);
        i++;
    }
}

/**
 * Return the normalized code, depending on ISO standard.
 */
gchar *search_get_normalized_code(gchar * code)
{
    return g_strdup(code);
}
