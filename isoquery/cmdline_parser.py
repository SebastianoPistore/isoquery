# - encoding: UTF-8 -
#
# Copyright © 2007-2011 Tobias Quathamer
#
# This file is part of isoquery.
#
# isoquery is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# isoquery is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import optparse
import gettext
from isoquery import __version__
translation = gettext.translation('isoquery', fallback=True)
_ = translation.ugettext

class CmdlineParser:
    def __init__(self):
        # Set up Parser
        usage = _("Usage: %prog [options] [ISO codes]")
        # Remove autogenerated help text, so that the --help option
        # itself can be translated
        self.parser = optparse.OptionParser(usage=usage, add_help_option=False)
        self.parser.add_option('-i', '--iso',
                               default='3166',
                               metavar=_("STANDARD"),
                               help=_("The ISO standard to use. " \
                               "Possible values: 639, 639-3, 3166, 3166-2, 4217, 15924. " \
                               "Default value: %default"))
        self.parser.add_option('-x', '--xmlfile',
                               default='/usr/share/xml/iso-codes/iso_3166.xml',
                               metavar=_("FILE"),
                               help=_("Use specified XML file with ISO data. " \
                               "Default value: %default"))
        self.parser.add_option('-l', '--locale',
                               default='',
                               metavar=_("LOCALE"),
                               help=_("Use this locale for output"))
        self.parser.add_option('-n', '--name',
                               action='store_true',
                               help=_("Display the name for the supplied " \
                                      "codes (default)"))
        self.parser.add_option('-o', '--official_name',
                               action='store_true',
                               help=_("Display the official name for the " \
                                      "supplied codes. This may be the " \
                                      "same as --name. (Only applies to ISO 3166)"))
        self.parser.add_option('-c', '--common_name',
                               action='store_true',
                               help=_("Display the common name for the " \
                                      "supplied codes. This may be the " \
                                      "same as --name. (Only applies to ISO 3166)"))
        self.parser.add_option('-0', '--null',
                               action='store_true',
                               dest="use_null_character",
                               help=_("Separate entries with a NULL " \
                                      "character instead of newline"))
        self.parser.add_option('-h', '--help',
                               action='help',
                               help=_("Show this information"))
        self.parser.add_option('-v', '--version',
                               action='store_true',
                               dest="show_version",
                               help=_("Show program version and copyright"))

    def parse(self, cmdline=None):
        """Parse the command line"""
        if cmdline is None:
            (self.options, self.args) = self.parser.parse_args()
        else:
            (self.options, self.args) = self.parser.parse_args([cmdline])
        self.check_options()
        return (self.options, self.args)

    def check_options(self):
        """Check for sensible default values in command line options"""
        if self.options.show_version is not None:
            self.show_version_and_copyright()
        # Assure a supported ISO standard
        supported = ["639", "639-3", "3166", "3166-2", "4217", "15924"]
        if self.options.iso not in supported:
            sys.stderr.write(_(u"isoquery: ISO standard '%(standard)s' " \
                               "is not supported.\n").encode("utf-8") % \
                               {"standard": self.options.iso})
            sys.exit(1)
        # If the XML file is not specified on the command line,
        # we get the default from above, so set the correct file
        if self.options.xmlfile == "/usr/share/xml/iso-codes/iso_3166.xml":
            self.options.xmlfile = "/usr/share/xml/iso-codes/iso_"
            self.options.xmlfile += self.options.iso.replace('-', '_')
            self.options.xmlfile += ".xml"
        # Remove extra spaces from locale
        self.options.locale = self.options.locale.strip()
        # If the locale does not exist for the given ISO standard, print a
        # warning message and use the untranslated (thus English) strings.
        if self.options.locale != "":
            try:
                t = gettext.translation("iso_" + self.options.iso.replace('-', '_'),
                                        languages=[self.options.locale])
            except IOError:
                sys.stderr.write(_(u"isoquery: The locale '%(locale)s' is not " \
                                   "available for ISO %(standard)s.\n").encode("utf-8") % \
                                   {"locale": self.options.locale,
                                   "standard": self.options.iso})
                self.options.locale = ""
        # Set default display attribute to 'name'
        self.options.display_name = "name"
        if self.options.official_name == True:
            self.options.display_name = "official_name"
        if self.options.common_name == True:
            self.options.display_name = "common_name"
        # Set a Boolean value for the null_character tests
        if self.options.use_null_character is None:
            self.options.use_null_character = False

    def show_version_and_copyright(self):
        print(_(u"isoquery %(version)s").encode("utf-8") % {"version": __version__})
        print(_(u"Copyright © 2007-2011 Tobias Quathamer").encode("utf-8"))
        # TRANSLATORS: Please change the uppercase words as appropriate for
        # your language.
        translation = _(u"Translation to LANGUAGE Copyright © YEAR YOUR-NAME")
        if translation.find("LANGUAGE") == -1:
            print(translation)
        print("""
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.""")
        sys.exit(0)
