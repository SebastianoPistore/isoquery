/* Copyright © 2013-2014 Tobias Quathamer
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

public class Handle_639 : Object {
    private static ProgramOptions options;
    private static ISO_639 iso;
    private static string standard;
    
    public static void show(ProgramOptions opts, string[] codes) {
        options = opts;
        iso = new ISO_639();
        standard = "639";
        if (options.filepath != null) {
            iso.set_filepath(options.filepath);
        }
        if (options.locale != null) {
            iso.set_locale(options.locale);
        }
        _show_codes(codes);
    }

    private static void _show_item(ISO_639_Item item) {
        stdout.printf("%s\t", item.iso_639_2B_code);
        stdout.printf("%s\t", item.iso_639_2T_code);
        stdout.printf("%s\t", item.iso_639_1_code);
        stdout.printf("%s", item.name);
        if (options.use_null_separator) {
            stdout.printf("%c", 0);
        }
        else {
            stdout.printf("\n");
        }
    }

    private static void _show_codes(string[] codes) {
        if (codes.length == 0) {
            try {
                var items = iso.find_all();
                foreach (var item in items) {
                    _show_item(item);
                }
            }
            catch (ISOCodesError err) {
                _handle_errors(err, null);
            }
        }
        else {
            foreach (var code in codes) {
                try {
                    var item = iso.find_code(code);
                    _show_item(item);
                }
                catch (ISOCodesError err) {
                    _handle_errors(err, code);
                }
            }
        }
    }

    private static void _handle_errors(ISOCodesError err, string? code) {
        var fatal_error = true;
        if (err is ISOCodesError.FILE_DOES_NOT_EXIST) {
            stderr.printf(_("isoquery: The file \"%s\" could not be opened.\n"), iso.get_filepath());
        }
        if (err is ISOCodesError.CANNOT_PARSE_FILE) {
            stderr.printf(_("isoquery: The file \"%s\" could not be parsed correctly.\n"), iso.get_filepath());
        }
        if (err is ISOCodesError.FILE_DOES_NOT_CONTAIN_ISO_DATA) {
            stderr.printf(_("isoquery: The file \"%s\" does not contain valid ISO %s data.\n"), iso.get_filepath(), standard);
        }
        if (err is ISOCodesError.CODE_NOT_DEFINED) {
            stderr.printf(_("isoquery: The code \"%s\" is not defined in ISO %s.\n"), code, standard);
            fatal_error = false;
        }
        // Exit the program with an error status,
        // unless the error is "CODE_NOT_DEFINED".
        if (fatal_error) {
            Posix.exit(Posix.EXIT_FAILURE);
        }
    }
}
