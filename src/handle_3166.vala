/* Copyright © 2013 Tobias Quathamer
 *
 * This file is part of isoquery.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

using libisocodes;

public class Handle_3166 : Object {
    private ISO_3166 iso;
    private ProgramOptions options;
    
    public void setup(ProgramOptions options) {
        this.iso = new ISO_3166();
        this.options = options;
        if (options.filepath != null) {
            this.iso.set_filepath(options.filepath);
        }
        if (options.locale != null) {
            this.iso.set_locale(options.locale);
        }
    }
    
    private void _show_item(ISO_3166_Item item) {
		stdout.printf("%s\t", item.alpha_2_code);
		stdout.printf("%s\t", item.alpha_3_code);
		stdout.printf("%s\t", item.numeric_code);
		if (options.official_name && item.official_name != "") {
			stdout.printf("%s\n", item.official_name);
		}
		else if (options.common_name && item.common_name != "") {
			stdout.printf("%s\n", item.common_name);
		}
		else {
			stdout.printf("%s\n", item.name);
		}
	}
	
    public void show(string[] codes) {
        if (codes.length == 0) {
            try {
                var items = this.iso.find_all();
                foreach (var item in items) {
                    _show_item(item);
                }
            }
            catch (ISOCodesError err) {
                // TRANSLATORS: This is an error message.
                stderr.printf(_("isoquery: %s\n"), err.message);
                // Exit the program with an error status.
                Posix.exit(Posix.EXIT_FAILURE);
            }
        }
        else {
            foreach (var code in codes) {
                try {
                    var item = this.iso.find_code(code);
                    _show_item(item);
                }
                catch (ISOCodesError err) {
                    // TRANSLATORS: This is an error message.
                    stderr.printf(_("isoquery: %s\n"), err.message);
                    // Exit the program with an error status,
                    // unless the error is "CODE_NOT_DEFINED".
                    if (!(err is ISOCodesError.CODE_NOT_DEFINED)) {
                        Posix.exit(Posix.EXIT_FAILURE);
                    }
                }
            }
        }
    }
}
