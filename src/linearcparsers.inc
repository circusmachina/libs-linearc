{ linearcparsers.inc - A parser for the LinearC command-line "language".

  This file is part of commandline.pp, a libray for robust command-line parsing.

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
{$IFDEF ReadInterface}
{ This class represents a parser for the
  @link(TheLinearCLanguage LinearC language).  It descends from
  @link(AParsedLanguageParser) because the parser does not output to an
  intermediate code stream, and though it still makes use of a symbol table
  representative of the options that can be set on the command line by the user,
  it needs only one of those.

  The parser automatically constructs an instance of
  @link(TheLinearCLanguage) to manage the tokens it parses.  This language
  specification will be freed when the parser is freed.

  To use the parser, simply pass an instance of @link(AStringStream) that
  contains a string with all command-line options on it to the
  @link(ALinearCParser.forSource constructor), then call
  @link(ALinearCParser.parse).
}
type ALinearCParser = class(AParsedLanguageParser)
  protected
    // Refers to the options that are parsed
    MyOptions: ACommandLineOptionDictionary;

  public
    { Construct a new instance of ALinearCParser that will parse the command
      line represented by @code(ThisSource) using the options specified by
      @code(TheseOptions), and outputting error and status information to
      @code(ThisLog).

      This method constructs a new instance of @link(ATextInputStream) around
      @code(ThisSource).
    }
    constructor forSource(const ThisSource: AStream;
      const TheseOptions: ACommandLineOptionDictionary;
      const ThisLog: ALog); overload; virtual;

    // Initializer
    function init: boolean; override;

    { Destroy the parser.

      This method frees the instance of @link(ATextInputStream) that was
      constructed by @link(ALinearCParser.forSource) and then calls the
      inherited routine.
    }
    destructor destroy; override;

    { Parse the command line.

      This method builds on the behavior inherited from
      @link(AParsedLanguageParser.parse); it calls that routine, then
      constructs an instance of @link(ALinearCStatementBlock) to parse the
      entire command line.

      If @link(ALinearCParser.Options) returns @nil, indicating that no
      instance of @link(ACommandLineOptionDictionary) was passed to the
      constructor when the parser was created, then this routine will do nothing
      and will return -1.
    }
    function parse: int64; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the dictionary of command-line options that may
      be set by the user.

      The caller should NOT attempt to free this reference before freeing the
      parser.
    }
    function Options: ACommandLineOptionDictionary; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor ALinearCParser.forSource(const ThisSource: AStream;
  const TheseOptions: ACommandLineOptionDictionary;
  const ThisLog: ALog);

begin
  inherited forSource(ATextInputStream.around(ThisSource, false), ThisLog);

  MyOptions := TheseOptions;
end;

// Initializer
function ALinearCParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyOptions := nil;

    { Establish the language specification used by the parser.  This
      specification is freed automatically by AParsedLanguageParser.destroy. }
    MyLanguage := TheLinearCLanguage.new;

    result := MyLanguage <> nil;
  end;
end;

// Destructor
destructor ALinearCParser.destroy;

begin
  if MySource <> nil then
    MySource.free;

  inherited destroy;
end;

// Parse the command line
function ALinearCParser.parse: int64;

var
  // Refers to a statement block parsed out of the source
  CommandLine: ALinearCStatementBlock;

begin
  result := inherited parse;
  if result <> 0 then
    exit;

  // Ensure we have a valid (non-nil) list of options
  result := -1;
  if MyOptions = nil then
    exit;

  { Establish the scanner used by the parser }
  MyScanner := ASourceScanner.forSource(ATextInputStream(MySource), MyLanguage);
  if MyScanner = nil then
    exit;

  { Whitespace is important to this scanner }
  MyScanner.setTokenBehaviors([SCAN_CONSOLIDATE_WHITESPACE]);

  // Parse the entire command line
  CommandLine := ALinearCStatementBlock.forSource(Self);
  result := CommandLine.parseUntilToken(TOKCAT_EOS);
  freeAndNil(CommandLine);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the list of options used by the parser
function ALinearCParser.Options: ACommandLineOptionDictionary;

begin
  Result := MyOptions;
end;

{$ENDIF}

