{ linearcstatements.inc - A descendant of AConsoleApplication that builds and
    executes a command-line parser automatically.

  This file is part of linearc.pp, a library for robust command-line
  parsing.

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 3 of the License or, at your option, any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABLITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ This class represents a single process running as a console application,
  which expects to receive arguments on its command line and which can parse
  those arguments with an instance of @link(ALinearCParser).

  This class modifies the behavior of @link(AConsoleApplication) somewhat:
  the @link(ALinearCConsoleApplication.run run) method is overridden to call the
  @link(ALinearCConsoleApplication.defineOptions) before
  calling @link(ALinearCConsoleApplication.parseCommandLine).
}
type ALinearCConsoleApplication = class(AConsoleApplication)
  protected
    { Refers to the command line arguments and options accepted by the program }
    MyOptions: ACommandLineOptionDictionary;
    { Refers to the log used for error and diagnostic output by the program }
    MyLog: ALog;

    { Define the command line options accepted by the program.

      This method may be overridden by descendant classes.  In the base
      implementation of @link(ALinearCConsoleApplication), it defines two
      @link(ACommandLineFlag flag options): one for "help" and one for
      "version".
    }
    procedure defineOptions; virtual;

    { Print usage information for the program.

      This method is called automatically by
      @link(ACommandLineOptionDictionary.handleDirtiedOptions) after all command
      line options have been parsed, if the @code(--help) option was specified
      by the user.  The routine simply calls
      @link(ALinearCConsoleApplication.printHelpTo Self.printHelpTo).

      @return(This method always returns @false to indicate that the processing
        of additional options should halt.)
    }
    function printHelp(const ThisOption: ACommandLineOption): boolean; virtual;

    { Print version information for the program.

      This method is called automatically by
      @link(ACommandLineOptionDictionary.handleDirtiedOptions) after all command
      line options have been parsed, if the @code(--version) option was
      specified by the user.  The routine simply calls
      @link(ALinearCConsoleApplication.printVersionTo Self.printVersionTo).

      @return(This method always returns @false to indicate that the processing
        of additional options should halt.)
    }
    function printVersion(const ThisOption: ACommandLineOption): boolean;
      virtual;

  public
    { Initializer}
    function init: boolean; override;

    { Destroy the application instance.

      This method frees @link(ALinearCConsoleApplication.Options), then calls
      the inherited routine.
    }
    destructor destroy; override;

    { Parse the command line.

      This method constructs an instance of @link(AStringStream) to represent
      the command line arguments passed to the program; it then passes that
      stream to an instance of @link(ALinearCParser), along with the items in
      @link(ALinearCConsoleApplication.Options), so that the command line can
      be parsed.

      @return(The value returned by @link(ALinearCParser.parse), which should
      indicate the number of errors that occurred while parsing.)
    }
    function parseCommandLine: int64; virtual;

    { Execute the application.

      This method builds on the behavior inherited from
      @link(AConsoleApplication.run): after calling the inherited routine, it
      calls @link(ALinearCConsoleApplication.defineOptions Self.defineOptions),
      then
      @link(ALinearCConsoleApplication.parseCommandLine Self.parseCommandLine).

      When the command-line arguments have been parsed, this method calls
      @link(ACommandLineOptionDictionary.handleDirtiedOptions)
      to handle switches which require additional handling.  If that method
      returns @false, then the method exits, returning
      @link(APP_RUN_INCOMPLETE).

      If either @link(ALinearCConsoleApplication.Options) or
      @link(ALinearCConsoleApplication.Log) are @nil when this method is
      entered, meaning that no descendant class has constructed a list of
      options or a log as part of their initialization, then this method will
      construct a basic instance of each.  The log will output to
      @link(AConsoleApplication.ErrorOut Self.ErrorOut).

      @return(Zero if execution was successful up to the point defined by this
      method (descendant classes may perform additional execution afterward),
      non-zero if an error occurred.)
    }
    function run: integer; override;

    { Print usage information for the program to the specified stream.

      This method first calls
      @link(ALinearCConsoleApplication.usageString Self.usageString) to obtain
      a usage string for the application, which it prints to
      @code(ThisStream); it then calls
      @link(ANode.printTo ACommandLineOptionsDictionary.printTo) on
      @link(ALinearCConsoleApplication.Options Self.Options).
    }
    procedure printHelpTo(const ThisStream: ATextOutputStream); virtual;

    { Print version information for the program to the specified stream.

      This method is called automatically by
      @link(ALinearCConsoleApplication.run), after the command line options have
      been parsed, if the @code(--version) option was specified by the user.

      The method uses the key/value pairs specified in
      @link(AnApplication.VersionInfo Self.VersionInfo) to print the name of
      the program and its version.  The format of this output is controlled by
      @link(lcappVersionStringFormat).

      Additional items can be specified in the
      @code(additionalItems) array.
    }
    procedure printVersionTo(const ThisStream: ATextOutputStream;
      const additionalItems: array of string); virtual;

    { Construct a usage string for the application.

      The usage string is printed whenever the user enters an option in error,
      and is also printed when the user requests help from the command-line.

      The usage string includes the executable name and the name of all rest
      arguments accepted by the application.  The format of the string is
      controlled by @link(lcappUsageStringFormat).
    }
    function usageString: string; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the dictionary of command-line options accepted
      by the program.

      The caller should NOT attempt to free the reference returned by this
      method; that will be done with the application instance is freed.
    }
    function Options: ACommandLineOptionDictionary; virtual;

    { Retrieve a reference to the log used for error and diagnostic output by
      the program.

      The caller should NOT attempt to free the reference returned by this
      method; that will be done when the application instance is freed.
    }
    function Log: ALog; virtual;
end;

const
  { This constant is used as the return value of
    @link(ALinearCConsoleApplication.run) to indicate that the user requested
    help from the command line.  It is specified as a negative value to
    indicate that the condition is not an error, but that normal execution
    should not proceed.
  }
  LC_APP_RUN_HELP_REQUESTED = APP_RUN_INCOMPLETE - 1;

  { This constant is used as the return value of
    @link(ALinearCConsoleApplication.run) to indicate the user requested
    version information from the command line.  It is specified as a value
    less than zero to indicate that the condition is not an error, but that
    normal execution should not proceed.
  }
  LC_APP_RUN_VERSION_REQUESTED = LC_APP_RUN_HELP_REQUESTED - 1;

  { This array can be used to determine whether and how
    @link(ALinearCConsoleApplication.printVersionTo) prints additional items
    besides just the program name and version.

    Each string is printed to the stream specified.  If the strings represent
    the name of a key that may be in
    @link(AnApplication.VersionInfo Self.VersionInfo), and if such a key is
    found, then the value associated with that key is printed.  Otherwise, the
    literal value of the string is printed instead.

    If the string to be printed is not contained in
    @link(tosWrapDefaultBreakCharacters), then it is printed by means of
    @link(ATextOutputStream.printWrapped); otherwise it is printed as is, by
    means of @link(ATextOutputStream.print).
  }
  lcappVersionDefaultAdditionalItems: array[0..2] of string = (
    vinfLegalCopyright,             // printWrapped() will add an EOL after this
    vinfComments, System.lineEnding // printWrapped() will add an EOL after this
  );

{$ENDIF}
{$IFDEF ReadResourceStrings}
  lcappHelpShortString = '?';
  lcappHelpLongString = 'help';
  lcappHelpDescription = 'Display this usage information.';
  lcappVersionLongString = 'version';
  lcappVersionDescription = 'Display program version information.';
  lcappUsageStringFormat = 'Usage: %s [options] %s' + lineEnding;
  lcappUsageHelpDescription = 'Enter "%s %s%s" for a description of ' +
    'available options.' + lineEnding;
  lcappVersionStringFormat = '%s %s' + lineEnding;
  lcappErrorUnsetRequiredOption = 'the option "%s" is required' + lineEnding;
{$ENDIF}
{$IFDEF ReadImplementation}
// ALinearCConsoleApplication **************************************************
// Initializer
function ALinearCConsoleApplication.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyOptions := nil;
  end;
end;

// Destructor
destructor ALinearCConsoleApplication.destroy;

begin
  // Free the list of command line options
  if MyOptions <> nil then
    MyOptions.free;

  // Free the log
  if MyLog <> nil then
    MyLog.free;

  inherited destroy;
end;

// Define the options accepted by the program
procedure ALinearCConsoleApplication.defineOptions;

begin
  if MyOptions = nil then
    exit;

  // Add the '--help' flag
  MyOptions.add(ACommandLineFlag.named(lcappHelpLongString,// name
    false,                                                 // is required?
    false,                                                 // is rest?
    false,                                                 // default value
    lcappHelpDescription,                                  // description
    '',                                                    // value description
    @Self.printHelp),                                      // dirty handler
    [lcappHelpShortString]);                               // abbreviations

  // Add the '--version' flag
  MyOptions.add(ACommandLineFlag.named(lcappVersionLongString,// name
    false,                                                    // is required?
    false,                                                    // is rest?
    false,                                                    // default value
    lcappVersionDescription,                                  // description
    '',                                                       // value description
    @Self.printVersion),                                      // dirty handler
    []);                                                      // abbreviations
end;

// Print help
function ALinearCConsoleApplication.printHelp(const ThisOption: ACommandLineOption): boolean;

begin
  Self.printHelpTo(MyConsoleOut);
  result := false;
end;

// Print version information
function ALinearCConsoleApplication.printVersion(const ThisOption: ACommandLineOption): boolean;

begin
  Self.printVersionTo(MyConsoleOut, lcAppVersionDefaultAdditionalItems);
  result := false;
end;

// Parse the command line
function ALinearCConsoleApplication.parseCommandLine: int64;

var
  // Refers to the string stream used as the source of options
  OptionsStream: ATextOutputStream;
  // Loop counter, used to collect options
  i: longword;
  // Refers to the parser used to parse the command line options
  CommandLine: ALinearCParser;

begin
  result := -1;

  if (MyOptions = nil) or (MyLog = nil) then
    exit;

  // Construct a string stream for options
  OptionsStream := ATextOutputStream.around(AStringStream.new);

  // Loop and add each option
  for i := 1 to System.paramCount do
  begin
    if i > 1 then
      OptionsStream.printEach([' ', System.paramStr(i)])

    else
      OptionsStream.print(System.paramStr(i));
  end;

  //OptionsStream.print(#26);
  OptionsStream.setPosition(0);

  // Parse the options
  CommandLine := ALinearCParser.forSource(OptionsStream.Target, MyOptions,
    MyLog);
  result := CommandLine.parse;

  CommandLine.free;
  OptionsStream.free;
end;

// Run the program
function ALinearCConsoleApplication.run: integer;

var
  // Refers to a required option that was not set by the user
  CleanRequiredOption: ACommandLineOption;
  // Refers to a logged error
  Error: ALoggedError;

begin
  result := inherited run;

  if result <> APP_RUN_COMPLETE then
    exit;

  result := APP_RUN_INCOMPLETE;

  // Construct a dictionary of command-line options, if necessary
  if MyOptions = nil then
    MyOptions := ACommandLineOptionDictionary.new;

  // Construct a log, if necessary
  if MyLog = nil then
    MyLog := ALog.toStream(MyErrorOut);

  if (MyOptions = nil) or (MyLog = nil) then
    exit;

  // Build the dictionary of command-line options accepted by the program
  Self.defineOptions;
  // Parse the command line
  if System.paramCount > 0 then
    result := integer(Self.parseCommandLine);

  // Handle special case options
  if not(MyOptions.handleDirtiedOptions) then
    // If special case options were specified and handled, then exit
    exit(APP_RUN_INCOMPLETE)

  else begin
    // Check to ensure all required options were set
    CleanRequiredOption := MyOptions.FirstCleanRequiredOption;

    // There is at least one required option that has not been set
    if CleanRequiredOption <> nil then
    begin
      // Log the error
      MyLog.Log(ALoggedError.because(lcappErrorUnsetRequiredOption, [
        CleanRequiredOption.name
      ]));

      if result <= 0 then
        result := 1;
    end;
  end;

  { Print basic usage information to help the user, if necessary.  We only
    print it if the command line was not successfully parsed (indicating that
    an invalid option was specified, or a required option was not specified)
    AND if the result code is not one of our special case scenarios.
  }
  if result <> APP_RUN_COMPLETE then
    MyConsoleOut.printEach([
      Self.usageString,
      SysUtils.format(lcappUsageHelpDescription, [
        Self.executableName(true),
        clopParameterNameDelimiter, lcappHelpLongString
      ])
    ]);
end;

// Print usage and help information to the specified stream
procedure ALinearCConsoleApplication.printHelpTo(const ThisStream: ATextOutputStream);

begin
  if (ThisStream = nil) or (MyOptions = nil) then
    exit;

  // Print usage information for the application
  ThisStream.print(Self.usageString);

  // Print usage information for each option accepted by the application
  MyOptions.printUsageTo(ThisStream);
  // Add an extra line before we return to the command prompt
  ThisStream.print(System.lineEnding);
end;

// Print version information to the specified stream
procedure ALinearCConsoleApplication.printVersionTo(const ThisStream: ATextOutputStream;
  const additionalItems: array of string);

var
  // Loop counters
  i, j: longword;
  // Stores the name of a key
  key: string;
  // Stores the value of a key
  value: Variant;

begin
  if (ThisStream = nil) or (MyOptions = nil) then
    exit;

  // Print the name of the program and its version
  ThisStream.print(SysUtils.format(lcappVersionStringFormat, [
    Self.VersionInfo.valueOf(vinfProductName, ''),
    Self.VersionInfo.valueOf(vinfFileVersion, '')
  ]));

  // Print additional items, if specified
  for i := 0 to System.high(additionalItems) do
  begin
    // Check to see whether the string represents a version info key
    key := additionalItems[i];
    value := Self.VersionInfo.valueOf(key);

    if value <> Variants.null then
      ThisStream.printWrapped(Variants.varToStr(value),
        tosWrapDefaultBreakCharacters)

    else begin
      // This will flag whether the value was whitespace and was printed
      value := false;

      // Check to see whether the string represents whitespace or a line break
      for j := 0 to System.high(tosWrapDefaultBreakCharacters) do
        if key = tosWrapDefaultBreakCharacters[j] then
        begin
          ThisStream.print(key);
          value := true;
          break;
        end;

      if value = false then
        ThisStream.printWrapped(key, tosWrapDefaultBreakCharacters);
    end;
  end;
end;

// Construct a usage string for the application
function ALinearCConsoleApplication.usageString: string;

var
  // Refers to the list of rest argument names
  RestArguments: AStringList;
  // Stores a deliited list of rest argument names
  restArgumentNames: string;

begin
  restArgumentNames := '';

  RestArguments := MyOptions.RestArgumentNames;

  if RestArguments <> nil then
  begin
    RestArguments.sort;
    restArgumentNames := RestArguments.implode;
    RestArguments.free;
  end;

  result := SysUtils.format(lcappUsageStringFormat, [
    Self.executableName(true), restArgumentNames
  ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the list of options
function ALinearCConsoleApplication.Options: ACommandLineOptionDictionary;

begin
  Result := MyOptions;
end;

// Retrieve a reference to the log used for error and diagonstic output
function ALinearCConsoleApplication.Log: ALog;

begin
  Result := MyLog;
end;

{$ENDIF}

