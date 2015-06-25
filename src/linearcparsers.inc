{ linearcparsers.inc - A parser for the LinearC command-line "language".

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

    { Initialize the parser prior to parsing the source.

      This method builds on the behavior inherited from
      @link(AParsedLanguageParser.readyToParse).  It calls the inherited routine
      and, if that routine returns @true to indicate all is well, it checks to
      see whether @link(AParser.Scanner Self.Scanner) is @nil, indicating that
      a descendant class has not yet assigned a scanner to the parser.  If not,
      the method constructs an instance of @link(ASourceScanner).

      The method also checks to see whether
      @link(ALinearCParser.Options Self.Options) is @nil, indicating that a
      descendant class has not yet assigned a list of options to the parser.
      If not, the method attempts to borrow the reference from its parent
      parser, if one is available.

      The method returns @true only if the inherited routine returns true and
      if both @link(AParser.Scanner Self.Scanner) and
      @link(ALinearCParser.Options Self.Options) are not @nil.

      @return(@true if the parser is ready to parse; @false if not.)
    }
    function readyToParse: boolean; override;

  public
    { Construct a new instance of ALinearCParser that will parse the command
      line represented by @code(ThisSource) using the options specified by
      @code(TheseOptions), and outputting error and status information to
      @code(ThisLog).

      This method constructs a new instance of @link(ATextInputStream) around
      @code(ThisSource).  That instance will be freed when the parser is freed.
      @code(takeOwnershipOfSource) is passed to 
      @link(AStreamWrapper.around ATextInputStream.around).
    }
    constructor forSource(const ThisSource: AStream;
      const takeOwnershipOfSource: boolean;
      const TheseOptions: ACommandLineOptionDictionary;
      const ThisLog: ALog); overload; virtual;

    { Construct a new parser that will inherit the properties of its parent
      parser.

      The new parser will inherit the values of @code(ThisParent)'s
      @link(AParser.Scanner Scanner), @link(AParser.Source Source),
      @link(AParser.Log Log), and @link(ALinearCParser.Options Options).
    }
    constructor forSource(const ThisParent: AParser); override;

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
  const takeOwnershipOfSource: boolean;
  const TheseOptions: ACommandLineOptionDictionary;
  const ThisLog: ALog);

begin
  inherited forSource(ATextInputStream.around(ThisSource, takeOwnershipOfSource),
    ThisLog);

  MyOptions := TheseOptions;
end;

// Constructor
constructor ALinearCParser.forSource(const ThisParent: AParser);

begin
  inherited forSource(ThisParent);

  if ThisParent is ALinearCParser then
    MyOptions := ALinearCParser(ThisParent).Options
end;

// Initializer
function ALinearCParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyOptions := nil;
  end;
end;

// Destructor
destructor ALinearCParser.destroy;

begin
  // Free some items if we do not have a parent
  if MyParent = nil then
  begin
    if MySource <> nil then
      SysUtils.freeAndNil(MySource);
  end;

  inherited destroy;
end;

// Determine whether or not the parser is ready to parse
function ALinearCParser.readyToParse: boolean;

begin
  if MyParent = nil then
  begin
    { Establish the language specification used by the parser.  This
      specification is freed automatically by AParsedLanguageParser.destroy.  We
      do this before calling the inherited routine because the inherited routine
      checks for a language.
    }
    if MyLanguage = nil then
      MyLanguage := TheLinearCLanguage.new;
      
    { Establish the scanner used by the parser.  We do this before calling the
      inherited routine because the inherited routine checks for a scanner.
    }
    if MyScanner = nil then
      MyScanner :=
        ASourceScanner.forSource(ATextInputStream(MySource), MyLanguage);
  end;
  
  result := inherited readyToParse;

  if result then
  begin
    // Ensure we have a valid list of options
    if (MyOptions = nil) and (MyParent <> nil) then
      if MyParent is ALinearCParser then
        MyOptions := ALinearCParser(MyParent).Options;

    // To succeed, we need a scanner and a valid list of options
    result := MyOptions <> nil;
  end;
end;

// Parse the command line
function ALinearCParser.parse: int64;

var
  // Refers to a statement block parsed out of the source
  CommandLine: ALinearCStatementBlock;

begin
  result := -1;

  if not(Self.readyToParse) then
    exit;

  { Whitespace is not important to this scanner }
  MyScanner.setTokenBehaviors([SCAN_CONSOLIDATE_WHITESPACE]);

  // Parse the entire command line
  CommandLine := ALinearCStatementBlock.forSource(Self);

  if CommandLine.readyToParse then
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

