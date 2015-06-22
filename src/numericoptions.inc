{ numericoptions.inc - Classes and definitions that represent dynamic
    command-line options which contain numeric values.

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
// ACommandLineNumber **********************************************************
{ This class represents an option that expectas a numeric value.

  The value of the number that can be assigned to the option is limited only by
  the precision of the @code(int64) and @code(double) types; to further
  constrain the values which may be entered by the user, one can instead use an
  instance of @link(ACommandLineIntegerRange), @link(ACommandLineFloatRange), or
  @link(ACommandLineChoice).
}
type ACommandLineNumber = class(ACommandLineOption)
  public
    // Initializer
    function init: boolean; override;

    // Properties --------------------------------------------------------------
    { Set the value of the option.

      This method overrides the behavior inherited from
      @link(ACommandLineOption.setValue); it attempts to cast @code(newValue) to
      a numeric value.  If that succeeds, the new value is set; otherwise it is
      ignored and an exception is thrown.

      @raises(AnOptionValueError in the event that the attempt to cast
      @code(newValue) to a numeric value fails.)

      @return(The previous value of
      @link(ADictionaryEntry.value ACommandLineNumber.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; override;
end;

// ACommandLineIntegerRange ****************************************************
{ This class represents an option that expects an integer value which falls
  between a predefined minimum and maximum.  If the user attempts to set a value
  that is outside the prescribed range of values, an error is returned to them.
}
type ACommandLineIntegerRange = class(ACommandLineNumber)
  protected
    /// Stores the minimum acceptable value
    myMinimumValue: int64;
    /// Stores the maximum acceptable value
    myMaximumValue: int64;

  public
    { Construct a new command-line option with the specified name.

      This method constructs a new instance of ACommandLineOption with the
      specified @code(defaultValue), @code(minimumValue), and
      @code(maximumValue).  The caller can also set whether the option is
      required, and can provide a description of the option which will be used
      when the user requests help.
    }
    constructor named(const thisName: string; const isRequired: boolean;
      const isRest: boolean;
      const thisDefaultValue: Variant;
      const thisMinimumValue, thisMaximumValue: int64;
      const thisDescription: string;
      const thisValueDescription: string = '';
      const onOptionDirtied: AnOptionDirtiedHandler = nil); reintroduce; virtual;

    /// Initializer
    function init: boolean; override;

    { Construct and return a string representation of the option, suitable for
      output to a text-based device, such as a console.

      This method builds upon the behavior inherited from
      @link(ACommandLineOption.usageString): it calls that routine first, then
      appends a string that indicates the minimum and maximum values for the
      range.  The format of the appended string is controlled by
      @link(clirAcceptedValuesDescription).
    }
    function usageString: AnsiString; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ACommandLineOption.shallowCopyFrom): it calls that routine first,
      then checks to see whether @code(Other) is an instance of
      @link(ACommandLineIntegerRange).  If so, it copies the values of
      @link(ACommandLineIntegerRange.minimum) and
      @link(ACommandLineIntegerRange.maximum) from @code(Other) to @code(Self),
      overwriting the corresponding values in @code(Self).

      This method is also compatible with instances of
      @link(ACommandLineFloatRange).  If @code(Other) is an instance of
      @link(ACommandLineFloatRange), this method will copy the values of
      @link(ACommandLineFloatRange.minimum) and
      @link(ACommandLineFloatRange.maximum) from @code(Other) to @code(Self),
      overwriting the corresponding values in @code(Self).  The values copied
      will be truncated to integer values.

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      Likewise, it does NOT copy the left or right subtrees and so cannot be
      used to create a full copy of any descendant of @link(ABinaryLeaf).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Set the value of the option.

      This method overrides the behavior inherited from
      @link(ACommandLineNumber.setValue).  As with the inherited routine, this
      method attempts to cast @code(newValue) to a numeric value.  If that
      succeeds, it checks to ensure that @code(newValue) falls between
      @link(ACommandLineIntegerRange.minimum) and
      @link(ACommandLineIntegerRange.maximum), inclusive.  If it does, the new
      value is set; otherwise it is ignored and an exception is thrown.

      @raises(AnOptionValueError in the event that the attempt to cast
      @code(newValue) to an integer value fails, or if @code(newValue) does not
      fall between @link(ACommandLineIntegerRange.minimum) and
      @link(ACommandLineIntegerRange.maximum).)

      @return(The previous value of
      @link(ADictionaryEntry.value ACommandLineIntegerRange.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; override;

    { Retrieve the minimum acceptable value allowed for the range.

      Calls to @link(ACommandLineIntegerRange.setValue) must specify a value
      that is greater than or equal to this value and less than or equal to the
      value of @link(ACommandLineIntegerRange.maximum).
    }
    function minimum: int64; virtual;

    { Set the minimum acceptable value allowed for the range.

      Calls to @link(ACommandLineIntegerRange.setValue) must be greater than or
      equal to @code(newMinimum) and less than or equal to the value of
      @link(ACommandLineIntegerRange.maximum).

      @return(The previous value of @link(ACommandLineIntegerRange.minimum).)
    }
    function setMinimum(const newMinimum: int64): int64; virtual;

    { Retrieve the maximum acceptable value allowed for the range.

      Calls to @link(ACommandLineIntegerRange.setValue) must specify a value
      that is less than or equal to this value and greater than or equal to the
      value of @link(ACommandLineIntegerRange.minimum).
    }
    function maximum: int64; virtual;

    { Set the maximum acceptable value allowed for the range.

      Calls to @link(ACommandLineIntegerRange.setValue) must specify a value
      that is less than or equal to this value and greater than or equal to the
      value of @link(ACommandLineIntegerRange.minimum).

      @return(The previous value of @link(ACommandLineIntegerRange.maximum).)
    }
    function setMaximum(const newMaximum: int64): int64; virtual;
end;

// ACommandLineFloatRange ******************************************************
{ This class represents an option that expects a floating-point value which
  falls between a predefined minimum and maximum.  If the user attempts to set a
  value that is outside the prescribed range of values, an error is returned to
  them.
}
type ACommandLineFloatRange = class(ACommandLineNumber)
  protected
    /// Stores the minimum acceptable value
    myMinimumValue: double;
    /// Stores the maximum acceptable value
    myMaximumValue: double;

  public
    { Construct a new command-line option with the specified name.

      This method constructs a new instance of ACommandLineOption with the
      specified @code(defaultValue), @code(minimumValue), and
      @code(maximumValue).  The caller can also set whether the option is
      required, and can provide a description of the option which will be used
      when the user requests help.
    }
    constructor named(const thisName: string; const isRequired: boolean;
      const isRest: boolean;
      const thisDefaultValue: Variant;
      const thisMinimumValue, thisMaximumValue: double;
      const thisDescription: string;
      const thisValueDescription: string = '';
      const onOptionDirtied: AnOptionDirtiedHandler = nil); reintroduce;
      virtual;

    /// Initializer
    function init: boolean; override;


    { Construct and return a string representation of the option, suitable for
      output to a text-based device, such as a console.

      This method builds upon the behavior inherited from
      @link(ACommandLineOption.usageString): it calls that routine first, then
      appends a string that indicates the minimum and maximum values for the
      range.  The format of the appended string is controlled by
      @link(clfrAcceptedValuesDescription).
    }
    function usageString: AnsiString; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ACommandLineOption.shallowCopyFrom): it calls that routine first,
      then checks to see whether @code(Other) is an instance of
      @link(ACommandLineFloatRange).  If so, it copies the values of
      @link(ACommandLineFloatRange.minimum) and
      @link(ACommandLineFloatRange.maximum) from @code(Other) to @code(Self),
      overwriting the corresponding values in @code(Self).

      This method is also compatible with instances of
      @link(ACommandLineIntegerRange).  If @code(Other) is an instance of
      @link(ACommandLineIntegerRange), this method will copy the values of
      @link(ACommandLineIntegerRange.minimum) and
      @link(ACommandLineIntegerRange.maximum) from @code(Other) to @code(Self),
      overwriting the corresponding values in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      Likewise, it does NOT copy the left or right subtrees and so cannot be
      used to create a full copy of any descendant of @link(ABinaryLeaf).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Set the value of the option.

      This method overrides the behavior inherited from
      @link(ACommandLineNumber.setValue).  As with the inherited routine, this
      method attempts to cast @code(newValue) to a numeric value.  If that
      succeeds, it checks to ensure that @code(newValue) falls between
      @link(ACommandLineFloatRange.minimum) and
      @link(ACommandLineFloatRange.maximum), inclusive.  If it does, the new
      value is set; otherwise it is ignored and an exception is thrown.

      @raises(AnOptionValueError in the event that the attempt to cast
      @code(newValue) to a floating-point value fails, or if @code(newValue)
      does not fall between @link(ACommandLineFloatRange.minimum) and
      @link(ACommandLineFloatRange.maximum).)

      @return(The previous value of
      @link(ADictionaryEntry.value ACommandLineFloatRange.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; override;

    { Retrieve the minimum acceptable value allowed for the range.

      Calls to @link(ACommandLineFloatRange.setValue) must specify a value
      that is greater than or equal to this value and less than or equal to the
      value of @link(ACommandLineFloatRange.maximum).
    }
    function minimum: double; virtual;

    { Set the minimum acceptable value allowed for the range.

      Calls to @link(ACommandLineFloatRange.setValue) must be greater than or
      equal to @code(newMinimum) and less than or equal to the value of
      @link(ACommandLineFloatRange.maximum).

      @return(The previous value of @link(ACommandLineFloatRange.minimum).)
    }
    function setMinimum(const newMinimum: double): double; virtual;

    { Retrieve the maximum acceptable value allowed for the range.

      Calls to @link(ACommandLineFloatRange.setValue) must specify a value
      that is less than or equal to this value and greater than or equal to the
      value of @link(ACommandLineFloatRange.minimum).
    }
    function maximum: double; virtual;

    { Set the maximum acceptable value allowed for the range.

      Calls to @link(ACommandLineFloatRange.setValue) must specify a value
      that is less than or equal to this value and greater than or equal to the
      value of @link(ACommandLineFloatRange.minimum).

      @return(The previous value of @link(ACommandLineFloatRange.maximum).)
    }
    function setMaximum(const newMaximum: double): double; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the extra string appended to the result
    returned when @link(ACommandLineIntegerRange.usageString) is called.

    The first integer placeholder is filled with the minimum value accepted by
    the option, as returned by a call to
    @link(ACommandLineIntegerRange.minimum).

    The second integer placeholder is filled with the maximum value accepted by
    the option, as returned by a call to
    @link(ACommandLineIntegerRange.maximum).
  }
  clirAcceptedValuesDescription = '  Accepted values fall between %d and ' +
    '%d, inclusive.';

  { This string controls the format of the extra string appended to the result
    returned when @link(ACommandLineFloatRange.usageString) is called.

    The first floating-point placeholder is filled with the minimum value
    accepted by the option, as returned by a call to
    @link(ACommandLineFloatRange.minimum).

    The second floating-point placeholder is filled with the maximum value
    accepted by the option, as returned by a call to
    @link(ACommandLineFloatRange.maximum).
  }
  clfrAcceptedValuesDescription = '  Accepted values fall between %f and ' +
    '%f, inclusive.';

  { This string controls the format of the error message emitted by
    @link(ACommandLineIntegerRange.setValue) in the event that the routine is
    given a value which is incompatible with the option.

    The string placeholder is filled with the name of the option.

    The first integer placeholder is filled with the value of
    @link(ACommandLineIntegerRange.minimum).

    The second integer placeholder is filled with the value of
    @link(ACommandLineIntegerRange.maximum).
  }
  clirErrorValueExpected = 'the option "%s" expects a value between %d and %d';

  { This string controls the format of the error message emitted by
    @link(ACommandLineFloatRange.setValue) in the event that the routine is
    given a value which is incompatible with the option.

    The string placeholder is filled with the name of the option.

    The first integer placeholder is filled with the value of
    @link(ACommandLineFloatRange.minimum).

    The second integer placeholder is filled with the value of
    @link(ACommandLineFloatRange.maximum).
  }
  clfrErrorValueExpected = 'the option "%s" expects a value between %f and %f';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription ACommandLineNumber.valueDescription)
    if no other description is set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clnmDefaultValueDescription = '<number>';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription ACommandLineIntegerRange.valueDescription)
    if no other description is set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clirDefaultValueDescription = '<integer>';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription ACommandLineFloatRange.valueDescription)
    if no other description is set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clfrDefaultValueDescription = '<number>';
{$ENDIF}
{$IFDEF ReadImplementation}
// ACommandLineNumber **********************************************************
// Initializer
function ACommandLineNumber.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtNumber];
    myValueDescription := clnmDefaultValueDescription;
  end;
end;

// Properties ------------------------------------------------------------------
// Set the value of the option
function ACommandLineNumber.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

begin
  result := myValue;

  // If the new value is already numeric, then set it
  if Variants.varIsNumeric(newValue) then
    myValue := newValue

  else
    // Otherwise, attempt to cast the value
    myValue := double(newValue);

  myDirtiness := makeDirty;
end;

// ACommandLineIntegerRange ****************************************************
// Constructor
constructor ACommandLineIntegerRange.named(const thisName: string;
  const isRequired: boolean; const isRest: boolean;
  const thisDefaultValue: Variant;
  const thisMinimumValue, thisMaximumValue: int64;
  const thisDescription: string; const thisValueDescription: string = '';
  const onOptionDirtied: AnOptionDirtiedHandler = nil);

begin
  // Call the inherited routine first
  inherited named(thisName, isRequired, isRest, thisDefaultValue,
    thisDescription, thisValueDescription, onOptionDirtied);

  myMinimumValue := thisMinimumValue;
  myMaximumValue := thisMaximumValue;
end;

// Initializer
function ACommandLineIntegerRange.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtIntegerRange];
    myMinimumValue := 0;
    myMaximumValue := 0;
    myValueDescription := clirDefaultValueDescription;
  end;
end;

// Construct a string representation of the option
function ACommandLineIntegerRange.usageString: AnsiString;

begin
  // Call the inherited routine and add to the resulting string
  result := inherited usageString +
    SysUtils.format(clirAcceptedValuesDescription, [
      myMinimumValue, myMaximumValue
    ]);
end;

// Copy from another object
function ACommandLineIntegerRange.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  if result then
  begin
    if Other is ACommandLineIntegerRange then
    begin
      myMinimumValue := ACommandLineIntegerRange(Other).minimum;
      myMaximumValue := ACommandLineIntegerRange(Other).maximum;
    end

    else if Other is ACommandLineFloatRange then
    begin
      myMinimumValue := trunc(ACommandLineFloatRange(Other).minimum);
      myMaximumValue := trunc(ACommandLineFloatRange(Other).maximum);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Set the value of the option
function ACommandLineIntegerRange.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

var
  // Stores the new value as an integer
  valueAsInteger: int64;

begin
  result := myValue;

  try
    valueAsInteger := int64(newValue);

    if (valueAsInteger < Self.minimum) or (valueAsInteger > Self.maximum) then
      raise AnOptionValueError.because(clirErrorValueExpected, [
        Self.name, Self.minimum, Self.maximum
      ]);

    myValue := valueAsInteger;
    myDirtiness := makeDirty;

  except
    raise AnOptionValueError.because(clirErrorValueExpected, [
      Self.name, Self.minimum, Self.maximum
    ]);
  end;
end;

// Retrieve the minimum acceptable value allowed for the range
function ACommandLineIntegerRange.minimum: int64;

begin
  result := myMinimumValue;
end;

// Set the minimum acceptable value allowed for the range
function ACommandLineIntegerRange.setMinimum(const newMinimum: int64): int64;

begin
  result := myMinimumValue;
  myMinimumValue := newMinimum;
end;

// Retrieve the maximum acceptable value allowed for the range
function ACommandLineIntegerRange.maximum: int64;

begin
  result := myMaximumValue;
end;

// Set the maximum acceptable value allowed for the range
function ACommandLineIntegerRange.setMaximum(const newMaximum: int64): int64;

begin
  result := myMaximumValue;
  myMaximumValue := newMaximum;
end;

// ACommandLineFloatRange ******************************************************
// Constructor
constructor ACommandLineFloatRange.named(const thisName: string;
  const isRequired: boolean; const isRest: boolean;
  const thisDefaultValue: Variant;
  const thisMinimumValue, thisMaximumValue: double;
  const thisDescription: string; const thisValueDescription: string;
  const onOptionDirtied: AnOptionDirtiedHandler = nil);

begin
  // Call the inherited routine first
  inherited named(thisName, isRequired, isRest, thisDefaultValue,
    thisDescription, thisValueDescription, onOptionDirtied);

  myMinimumValue := thisMinimumValue;
  myMaximumValue := thisMaximumValue;
end;

// Initializer
function ACommandLineFloatRange.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtFloatRange];
    myMinimumValue := 0;
    myMaximumValue := 0;
    myValueDescription := clfrDefaultValueDescription;
  end;
end;

// Construct a string representation of the option
function ACommandLineFloatRange.usageString: AnsiString;

begin
  // Call the inherited routine and add to the resulting string
  result := inherited usageString +
    SysUtils.format(clfrAcceptedValuesDescription, [
      myMinimumValue, myMaximumValue
    ]);
end;

// Copy from another object
function ACommandLineFloatRange.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  if result then
  begin
    if Other is ACommandLineFloatRange then
    begin
      myMinimumValue := ACommandLineFloatRange(Other).minimum;
      myMaximumValue := ACommandLineFloatRange(Other).maximum;
    end

    else if Other is ACommandLineIntegerRange then
    begin
      myMinimumValue := trunc(ACommandLineIntegerRange(Other).minimum);
      myMaximumValue := trunc(ACommandLineIntegerRange(Other).maximum);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Set the value of the option
function ACommandLineFloatRange.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

var
  // Stores the new value as a double
  valueAsDouble: double;

begin
  result := myValue;

  try
    valueAsDouble := double(newValue);

    if (valueAsDouble < Self.minimum) or (valueAsDouble > Self.maximum) then
      raise AnOptionValueError.because(clfrErrorValueExpected, [
        Self.name, Self.minimum, Self.maximum
      ]);

    myValue := valueAsDouble;
    myDirtiness := makeDirty;

  except
    raise AnOptionValueError.because(clfrErrorValueExpected, [
      Self.name, Self.minimum, Self.maximum
    ]);
  end;
end;

// Retrieve the minimum acceptable value allowed for the range
function ACommandLineFloatRange.minimum: double;

begin
  result := myMinimumValue;
end;

// Set the minimum acceptable value allowed for the range
function ACommandLineFloatRange.setMinimum(const newMinimum: double): double;

begin
  result := myMinimumValue;
  myMinimumValue := newMinimum;
end;

// Retrieve the maximum acceptable value allowed for the range
function ACommandLineFloatRange.maximum: double;

begin
  result := myMaximumValue;
end;

// Set the maximum acceptable value allowed for the range
function ACommandLineFloatRange.setMaximum(const newMaximum: double): double;

begin
  result := myMaximumValue;
  myMaximumValue := newMaximum;
end;
{$ENDIF}

