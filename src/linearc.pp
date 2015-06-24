{ linearc.pp - A robust command-line argument parser.
  Copyright (C) 2011-2015 Michael Malicoat.

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

{$SMARTLINK ON}

{ This unit defines a @link(ALinearCParser robust parser) that can process
  command-line arguments, as well as @link(ALinearCConsoleApplication), which
  descends from @link(AConsoleApplication) and builds and executes such a parser
  automatically.

  In general, @link(ALinearCParser) follows the GNU standards which can be
  found at: @unorderedList(
    @item(http://www.gnu.org/prep/standards/html_node/Command_002dLine-Interfaces.html#Command_002dLine-Interfaces)
    @item(https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)
  )

  One notable exception that the parser applies to the standards is that
  abbreviated options are not limited to a single character.
}
unit linearc;

{$mode objfpc}{$H+}

interface

uses
  classwork, parsing, variants;

{$DEFINE ReadInterface}
{$I options.inc}
{$I booleanoptions.inc}
{$I numericoptions.inc}
{$I listoptions.inc}
{$I optiondictionaries.inc}
{$I linearc.inc}
{$I linearcparsers.inc}
{$I linearcstatements.inc}
{$I linearcexpressions.inc}
{$I linearcapplications.inc}

{$UNDEF ReadInterface}

resourcestring
{$DEFINE ReadResourceStrings}
{$I options.inc}
{$I booleanoptions.inc}
{$I numericoptions.inc}
{$I listoptions.inc}
{$I optiondictionaries.inc}
{$I linearctokenstrings.inc}
{$I linearcstatements.inc}
{$I linearcexpressions.inc}
{$I linearcapplications.inc}

{$UNDEF ReadResourceStrings}

implementation

uses
  Math, StrUtils, SysUtils;

{$DEFINE ReadImplementation}
{$I options.inc}
{$I booleanoptions.inc}
{$I numericoptions.inc}
{$I listoptions.inc}
{$I optiondictionaries.inc}
{$I linearc.inc}
{$I linearcparsers.inc}
{$I linearcstatements.inc}
{$I linearcexpressions.inc}
{$I linearcapplications.inc}

{$UNDEF ReadImplementation}
end.

