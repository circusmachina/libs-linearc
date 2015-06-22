{ listoptions.inc - Classes and definitions that represent dynamic
    command-line options which work with lists of values.

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
// ACommandLineChoice **********************************************************
{ This class represents a command-line option that expects a value to be chosen
  from a list of pre-defined values.

  If the user supplies a value which is not in
  @link(ACommandLineChoice.AcceptedValues), then an error is returned to them.
}
type ACommandLineChoice = class(ACommandLineOption)
  protected
    // Refers to the list of allowed values
    MyAcceptedValues: AVariantList;

  public
    { Construct a new command-line option with the specified name.

      This method constructs a new instance of ACommandLineOption with the
      specified @code(defaultValue) and @code(theseValuesAccepted).  The caller
      can also set whether the option is required, and can provide a description
      of the option which will be used when the user requests help.
    }
    constructor named(const thisName: string; const isRequired: boolean;
      const isRest: boolean;
      const thisDefaultValue: Variant;
      const theseValuesAccepted: array of const;
      const thisDescription: string;
      const thisValueDescription: string = '';
      const onOptionDirtied: AnOptionDirtiedHandler = nil); reintroduce;
      virtual;

    // Initializer
    function init: boolean; override;

    { Destroy the option.

      This method frees the values in @link(ACommandLineChoice.AcceptedValues)
      and then calls the inherited routine.
    }
    destructor destroy; override;

    { Construct and return a string representation of the option, suitable for
      output to a text-based device, such as a console.

      This method builds upon the behavior inherited from
      @link(ACommandLineOption.usageString): it calls that routine first, then
      appends a string that indicates the acceptable values for the option.
      The format of the appended string is controlled by
      @link(clchAcceptedValuesDescription).
    }
    function usageString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the list of values allowed for the option.  The
      caller should NOT attempt to free this reference; that will be done when
      the option itself is freed.
    }
    function AcceptedValues: AVariantList; virtual;

    { Set the value of the option.

      This method overrides the behavior inherited from
      @link(ACommandLineOption.setValue).  It checks to ensure that
      @code(newValue) is in the list of values specified by
      @link(ACommandLineChoice.AcceptedValues Self.AcceptedValues).  If not, the
      value is not set and an exception is raised.

      @raises(AnOptionValueError if @code(newValue) specifies a value that
      is not in the list of
      @link(ACommandLineChoice.AcceptedValues accepted values).)

      @returns(The previous value of
      @link(ADictionaryEntry.value ACommandLineChoice.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; override;
end;

// ACommandLineList ************************************************************
{ This class represents an option that can be specified more than once on the
  command line; each time it occurs, the value specified is added to a list of
  values.

  @link(ADictionaryEntry.value ACommandLineList.value) will contain the most
  recent value assigned to this option, while @link(ACommandLineList.Values)
  will contain all values assigned to the option, including the most recent.
}
type ACommandLineList = class(ACommandLineOption)
  protected
    // Refers to the list of values collected for the option
    MyValues: AVariantList;

  public
    // Initializer
    function init: boolean; override;

    { Destroy the option.

      This method frees the values in @link(ACommandLineList.Values) and then
      calls the inherited routine.
    }
    destructor destroy; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the list of values collected for the option.

      The most recent value collected for the option will always be at the end
      of the list, and can also be obtained by calling
      @link(ADictionaryEntry.value ACommandLineList.value).

      The caller should NOT attempt to free the reference returned by this
      routine; that will be done when the option itself is freed.
    }
    function Values: AVariantList; virtual;

    { Set the value of the option.

      This method overrides the behavior inherited from
      @link(ACommandLineOption.setValue).  It adds @code(newValue) to the
      @link(ACommandLineList.Values list of values) maintained for the option,
      then sets @link(ADictionaryEntry.value Self.value) to @code(newValue).

      @returns(The previous value of
      @link(ADictionaryEntry.value ACommandLineList.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the extra string appended to the result
    returned when @link(ACommandLineChoice.usageString) is called.

    The string placeholder is filled with a list of acceptable values for the
    option, as returned by calling
    @link(ALinkedList.implode AVariantList.implode) on
    @link(ACommandLineChoice.AcceptedValues).
  }
  clchAcceptedValuesDescription = '  Accepted values are any one of: %s.';

  { This string controls the format of the error message emitted by
    @link(ACommandLineChoice.setValue) in the event that the routine is
    given a value which is incompatible with the option.

    The first string placeholder is filled with the name of the option.

    The second string placeholder is filled with a list of accepted values for
    the option, as returned by calling
    @link(ALinkedList.implode AVariantList.implode) on
    @link(ACommandLineChoice.AcceptedValues).
  }
  clchErrorValueExpected = 'the option "%s" expects one of the following ' +
    'values:' + lineEnding + #9'%s';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription ACommandLineChoice.valueDescription)
    if no other description is set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clchDefaultValueDescription = '<value>';
{$ENDIF}
{$IFDEF ReadImplementation}
// ACommandLineChoice **********************************************************
// Constructor
constructor ACommandLineChoice.named(const thisName: string;
  const isRequired: boolean; const isRest: boolean;
  const thisDefaultValue: Variant; const theseValuesAccepted: array of const;
  const thisDescription: string; const thisValueDescription: string = '';
  const onOptionDirtied: AnOptionDirtiedHandler = nil);

begin
  // Call the inherited routine first
  inherited named(thisName, isRequired, isRest, thisDefaultValue,
    thisDescription, thisValueDescription, onOptionDirtied);

  // Add the values accepted
  MyAcceptedValues.pushSeveral(theseValuesAccepted);
end;

// Initializer
function ACommandLineChoice.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtMultipleChoice];
    myValueDescription := clchDefaultValueDescription;

    MyAcceptedValues := AVariantList.new;

    result := MyAcceptedValues <> nil;
  end;
end;

// Destructor
destructor ACommandLineChoice.destroy;

begin
  // Free the list of accepted values
  if MyAcceptedValues <> nil then
    MyAcceptedValues.free;

  inherited destroy;
end;

// Construct a string representation of the option
function ACommandLineChoice.usageString: AnsiString;

begin
  // Call the inherited routine and add a list of allowed values
  result := inherited usageString +
    SysUtils.format(clchAcceptedValuesDescription, [
      MyAcceptedValues.implode(llstDefaultDelimiter + ' ')
    ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the list of accepted values
function ACommandLineChoice.AcceptedValues: AVariantList;

begin
  Result := MyAcceptedValues;
end;

// Set the value of the option
function ACommandLineChoice.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

begin
  result := myValue;

  // Only set the value if it is in the list of allowed values
  if MyAcceptedValues.indexOf(newValue) >= 0 then
  begin
    myValue := newValue;
    myDirtiness := makeDirty;
  end

  else
    raise AnOptionValueError.because(clchErrorValueExpected, [
      Self.name, MyAcceptedValues.implode(llstDelimiterTabbedColumn)
    ]);
end;

// ACommandLineList ************************************************************
// Initializer
function ACommandLineList.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtList];

    MyValues := AVariantList.new;

    result := MyValues <> nil;
  end;
end;

// Destructor
destructor ACommandLineList.destroy;

begin
  // Free the list of values
  if MyValues <> nil then
    MyValues.free;

  inherited destroy;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the list of collected values
function ACommandLineList.Values: AVariantList;

begin
  Result := MyValues;
end;

// Set the value of the option
function ACommandLineList.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

begin
  result := myValue;

  // Add the value to the list as well as setting it as the most recent value
  myValue := newValue;
  MyValues.push(newValue);

  myDirtiness := makeDirty;
end;
{$ENDIF}

