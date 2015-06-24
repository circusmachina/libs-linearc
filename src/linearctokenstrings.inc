{ linearctokenstrings.inc - Token strings recognized by the LinearC parser.

  This file is part of linearc.pp, a libray for robust command-line parsing.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadResourceStrings}
  // Character sets used by the language
  { This string specifies the characters that LinearC recognizes as
    alphanumeric.  The values specified here are appended to the ones in
    @link(plcsTypicalLetter) when
    @link(TheLinearCLanguage.defineCharacterCategories) is called.
  }
  lccsLetter             = '?/\:';
  { This string specifies the characters that LinearC recognizes as operators
    or delimiters.
  }
  lccsSpecial            = '+-*/!=''"^()\';

  { This string specifies the characters that LinearC recognizes as beginning
    a numeric constant.  The characters here are added to those defined by
    @link(plcsTypicalDigit) when
    @link(TheLinearCLanguage.defineCharacterCategories) is called.
  }
  lccsDigit              = '.$';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwTrueString                   = 'true';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwFalseString                  = 'false';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwOnString                     = 'on';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwOffString                    = 'off';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwYesShortString                    = 'y';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwYesFullString                    = 'yes';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwNoShortString                    = 'n';

  { In order to process @link(ACommandLineSwitch switch) options, LinearC must
    define some tokens that represent truth and falsehood.  This string defines
    the string for one such token.
  }
  lckwNoFullString                     = 'no';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopPlusString                   = '+';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopMinusString                  = '-';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopTimesString                  = '*';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopDivideString                 = '/';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopModuloString                 = 'MOD';

  { The LinearC parser supports basic arithmetic operations on the command line.
    To do so, it must define some tokens that represent basic mathematical
    operations.  This string defines one such token.
  }
  lcopExponentString               = '^';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopShlString                    = 'SHL';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopShrString                    = 'SHR';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopAndString                    = 'AND';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopOrString                     = 'OR';

  { The LinearC parser allows bitwise operations on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopXorString                     = 'XOR';

  { The LinearC parser allows boolean options to be defined as NOT true or
    NOT false.  To do so, the language must define a token which represents
    a boolean NOT operation.  This string defines that token.
  }
  lcopNotString                     = 'NOT';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopEqualityString                = 'EQ';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopInequalityString              = 'NEQ';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopGreaterString                = 'GT';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopGreaterOrEqualString         = 'GEQ';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopLesserString                = 'LT';

  { The LinearC parser allows equality tests on numeric values.  To do so,
    the language must define some tokens that represent such operations.  This
    string defines one such token.
  }
  lcopLesserOrEqualString        = 'LEQ';

  { In order to parse strings, the LinearC parser must know when one begins and
    ends.  Since strings are usually delimited by quotation marks, this string
    defines one such token.
  }
  lcdlQuote1String                 = '"';

  { In order to parse strings, the LinearC parser must know when one begins and
    ends.  Since strings are usually delimited by quotation marks, this string
    defines one such token.
  }
  lcdlQuote2String                 = '''';

  { In order to parse compound expressions, the LinearC parser must know when
    one expression begins and ends.  Since subexpressions are usually contained
    within parentheses, this string defines one such token.
  }
  lcdlLeftParenString             = '(';

  { In order to parse compound expressions, the LinearC parser must know when
    one expression begins and ends.  Since subexpressions are usually contained
    within parentheses, this string defines one such token.
  }
  lcdlRightParenString             = ')';

  { In order to parse Windows-style paths, the LinearC parser must know about
    backslash characters.
  }
  lcdlBackslashString              = '\';
{$ENDIF}
