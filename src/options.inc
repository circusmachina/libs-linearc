{ options.inc - Classes and definitions that represent dynamic
    command-line options.

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
{ This enumeration indicates the types of options that can be accepted on the
  command line:
  @unorderedList(
    @item(@code(ovtNone): Indicates an invalid option; not really used)

    @item(@code(ovtFlag): A flag value, which is assumed to be @false unless
      it is specified on the command-line.  A flag value might be specified as
      @code(@--flag) when passed on the command line.  Represented by instances
      of @link(ACommandLineFlag).)

    @item(@code(ovtSwitch): A switch, which may be @true or @false.  Unlike
      flag values, the value of a switch must be explicitly set on the command
      line.  A switch might be specified as @code(@--switch=y) or
      @code(@--switch=false) when passed on the command line.  Represented by
      instances of @link(ACommandLineSwitch).)

    @item(@code(ovtNumber): A numeric value, either integer or floating-point.
      The only constraints on the value are the limits of the @code(int64) or
      @code(double) types.  A numeric value might be specified as
      @code(@--value=23) or @code(@--value=42.0) when passed on the command
      line.  Represented by instances of @link(ACommandLineNumber).)

    @item(@code(ovtIntegerRange), @code(ovtFloatRange): A numeric value that
      must fall between a minimum and maximum value.  Represented by instances
      of @link(ACommandLineIntegerRange) and @link(ACommandLineFloatRange).)

    @item(@code(ovtString): A string value.  A string value might be specified
      as @code(@--value=text) or @code(@--value="text") when passed on the
      command line.  Represented by instances of @link(ACommandLineString).)

    @item(@code(ovtMultipleChoice): A value that must match one of a list of
      provided values.  Represented by instances of @link(ACommandLineChoice).)

    @item(@code(ovtList): A value that can be specified more than once on the
      command line; each occurrence is collected into a list of values.
      Represented by instances of @link(ACommandLineList).)

    @item(@code(ovtRest): An argument that is specified without a name preceding
      it.  These are usually used to indicate file names.  This value is
      combined with one of the above values.)
  )
}
type TOptionValueType = (
  ovtNone,
  // A flag -- a value that is true only if the flag is specified
  ovtFlag,
  // A switch -- yes/no, on/off, true/false, etc.
  ovtSwitch,
  // A numeric value -- floating point or integer
  ovtNumber,
  // A numeric value that is constrained to a range
  ovtIntegerRange,
  // A numeric value that is constrained to a range
  ovtFloatRange,
  // A string value
  ovtString,
  // A value that is constrained to a set of values
  ovtMultipleChoice,
  // A list item
  ovtList,
  // A rest argument
  ovtRest
);

{ This type is used to contain the flags which indicate what type of value
  is accepted by an instance of @link(ACommandLineOption).
}
type AnOptionValueType = set of TOptionValueType;

// ACommandLineOption **********************************************************
type
  { This class represents a dynamic command-line option; that is, one which
    has been defined at runtime, either by reading it from a static definition
    or programatically.  Dynamic command-line options can be grouped together
    into a dictionary of options, which define the options that are valid and
    recognized by a program which accepts command-line parameters.

    This base class is not likely to be instantiated directly; instead one of
    its descendants will be used, since these are tailored to a specific type of
    option.
  }
  ACommandLineOption = class;

  { This type is used to specify a callback routine which is invoked after the
    options have been parsed and if the option has been set.

    The method invoked should return @true if processing of other callbacks
    should continue, and @false if not.
  }
  AnOptionDirtiedHandler = function(const ThisOption: ACommandLineOption): boolean of object;

  ACommandLineOption = class(ADictionaryEntry)
  protected
    // Stores the default value of the option
    myDefaultValue: Variant;
    // Indicates the option type
    myOptionType: AnOptionValueType;
    // Indicates whether the option is required
    myRequirement: boolean;
    // Indicates whether the option has been set by the user
    myDirtiness: boolean;
    // Stores a description of the type of value expected by the option
    myValueDescription: string;
    // Stores a description of the option
    myDescription: string;
    // Stores a callback which is invoked if the option has been dirtied (set)
    myOptionDirtiedHandler: AnOptionDirtiedHandler;

  public
    { Construct a new command-line option with the specified name.

      This method constructs a new instance of ACommandLineOption with the
      specified @code(defaultValue).  The caller can also set whether the
      option is required and can provide a description of the option which will
      be used when the user requests help.

      @param(thisName specifies the name of the option as it should be supplied
      on the command-line.  It is case-sensitive.)

      @param(isRequired indicates whether or not the user MUST supply the option
      on the command line.)

      @param(isRest indicates whether or not the option is a rest argument;
      if @true, then the user need not supply the name of the argument on the
      command line).

      @param(thisDefaultValue is used to set the initial value of the option.)

      @param(thisDescription is used to describe the purpose of the option to
      the user.)

      @param(thisValueDescription is used to describe the type of value accepted
      by the option.  If this parameter is left blank, a default description is
      used, depending on the type of option represented by the new instance.)
    }
    constructor named(const thisName: string; const isRequired: boolean;
      const isRest: boolean;
      const thisDefaultValue: Variant; const thisDescription: string;
      const thisValueDescription: string = '';
      const onOptionDirtied: AnOptionDirtiedHandler = nil); reintroduce;
      virtual;

    /// Initializer
    function init: boolean; override;

    { Destroy the option.

      This method overrides the behavior inherited from @link(ANode.destroy);
      it does not attempt to destroy any children of the node.  Since any such
      children represent abbreviated forms of the option, and so are entered
      into the main dictionary of options, they will be freed along with all
      other top-level options.

      @bold(This method still causes the destruction of all older siblings of
      the node) by recursively traversing
      @link(ANode.Previous ACommandLineOption.Previous) and freeing each node
      encountered until a @nil result is found.  If this behavior is not
      desired, then the node should be detached from its siblings by calling
      @link(ANode.detach ACommandLineOption.detach) before this node is freed.

      Otherwise, @code(TObject.free) can be called on the last option in a list
      of options and it will cause the destruction of the entire list.
    }
    destructor destroy; override;

    { Obtain the names of any abbreviations specified for the option.

      If an option has an abbreviated form, this abbreviation may be specified
      on the command line instead of the full name of the option.

      Abbreviations are specified at the time the option is entered into an
      instance of @link(ACommandLineOptionDictionary).  This method checks to
      see whether @link(ANode.Child ACommandLineOption.Child) has been set to a
      value that is not @nil; if so, it traverses the list of children, querying
      the name of each one.  These are inserted into a string list which is
      subsequently returned to the caller.

      @returns(An instance of @link(AStringList) that contains the names of all
      abbreviations specified for the option.  The caller is responsible for
      freeing this list when it is no longer required.  If the option has no
      abbreviations, this method returns @nil.)
    }
    function Abbreviations: AStringList; virtual;

    { Construct and return a string representation that describes the name,
      accepted values, and usage of the option.  This0 method is designed to
      provide assistance to the user when they request help from the command
      line.
    }
    function usageString: AnsiString; virtual;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ASymbol.shallowCopyFrom): it calls that routine first, then
      checks to see whether @code(Other) is an instance of
      @link(ACommandLineOption).  If so, it copies the values of
      @link(ACommandLineOption.required), @link(ACommandLineOption.dirty) and
      @link(ACommandLineOption.description) from @code(Other) to @code(Self),
      overwriting the corresponding values in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      Likewise, it does NOT copy the left or right subtrees and so cannot be
      used to create a full copy of any descendant of @link(ABinaryLeaf).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve the default value of the option.  This is set when the option is
      constructed and cannot be changed.
    }
    function defaultValue: Variant; virtual;

    { Set the current value of the option.

      If @code(makeDirty) is @false, then calling this method will not change
      the @link(ACommandLineOption.dirty dirty flag) that indicates the change
      has been made by the user.

      @return(The previous value of
        @link(ADictionaryEntry.value ACommandLineOption.value).)
    }
    function setValue(const newValue: Variant;
      const makeDirty: boolean = true): Variant; reintroduce; virtual;

    { Retrieve the option type.  This is set by the option when it is
      constructed and cannot be changed.
    }
    function optionType: AnOptionValueType; virtual;

    { Retrieve whether or not the option is required: whether it must be
      specified on the command-line by the user.

      If this routine returns @true, then an error will be returned to the user
      if @link(ACommandLineOption.dirty) returns @false, since that indicates
      the option is required but was not set by the user.
    }
    function required: boolean; virtual;

    { Set whether or not the option is required: whether it must be specified on
      the command-line by the user.

      If an option is required but @link(ACommandLineOption.dirty) returns
      @false, then an error will be returned to the user, since that indicates
      that the option is required but was not set by the user.

      @return(The previous value of @link(ACommandLineOption.required).)
    }
    function setRequired(const flag: boolean): boolean; virtual;

    { Determine whether or not the option is a rest argument; that is, an
      option that does not require its name to be provided on the command line.

      This method simply checks to see whether
      @link(TOptionValueType ovtRest) is in the set of values defined by
      @link(ACommandLineOption.optionType).
    }
    function isRestArgument: boolean; virtual;

    { Retrieve whether or not the option has been set by the user.

      If this routine returns @true, then the option was found on the
      command line and its value set from there; otherwise, the option still
      retains its default value or a value that was not set by the user.

      If this method returns @false, but @link(ACommandLineOption.required)
      is @true, then an error will be returned to the user, since that indicates
      that the option is required but was not set by the user.
    }
    function dirty: boolean; virtual;

    { Set whether or not the option is dirty.

      If @code(flag) is @true, then it indicates that the option and its value
      were found while parsing the command line -- the option has been set by
      the user.

      @return(The previous value of @link(ACommandLineOption.dirty).)
    }
    function setDirty(const flag: boolean): boolean; virtual;

    { Retrieve a description of the option.

      The description returned was set when the option was first constructed and
      cannot be changed.  It is most often presented to the user when the user
      requests help.
    }
    function description: string; virtual;

    { Retrieve a description of the type of value accepted by the option.

      The description returned was set when the option was first constructed and
      cannot be changed.  It is most often presented to the user when the user
      requests help.

      If no description of the value was provided when the option was first
      constructed, then this method will return the default value established
      by @link(ACommandLineOption.init).
    }
    function valueDescription: string; virtual;

    { Retrieve a reference to the callback that should be invoked after all
      options have been parsed, but only if the option has been set.
    }
    function optionDirtiedHandler: AnOptionDirtiedHandler; virtual;

    { Set a reference to the callback routine that should be invoked after
      all options have been parsed, but only if this specific option has
      been set.

      @return(The previous value of
      @link(ACommandLineOption.optionDirtiedHandler).)
    }
    function setOptionDirtiedHandler(const newHandler: AnOptionDirtiedHandler): AnOptionDirtiedHandler;
      virtual;
end;

// ACommandLineString **********************************************************
{ This class represents a command-line option that expects a string value. }
type ACommandLineString = class(ACommandLineOption)
  public
    // Initializer
    function init: boolean; override;
end;

// ACommandLineAbbreviation ****************************************************
{ This class represents the abbreviated form of a command-line option.  It
  descends from @link(AStringLeaf) because it does not need to do much more than
  hold the name of the abbreviation and refer back to its parent.

  Abbreviations for command-line options can be specified when instances of
  @link(ACommandLineOption) are added to an instance of
  @link(ACommandLineOptionDictionary).
}
type ACommandLineAbbreviation = class(AStringLeaf);

// AnOptionValueError **********************************************************
{ An instance of this exception class is raised by
  @link(ACommandLineOption.setValue) if the value to be set does not conform
  to the option's definition.
}
type AnOptionValueError = class(AnException);

const
  { This constant is used to determine where the names of command-line options
    begin when printed to a text-based device, such as a console.
  }
  clopNameColumn = 2;

  { This constant is used to determine where the descriptions of command-line
    options begin when printed to a text-based device, such as a console.
  }
  clopDescriptionColumn = 35;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string determines the characters that are used to indicate a parameter
    name on the command line.  All named parameters encountered must be prefixed
    by these characters.
  }
  clopParameterNameDelimiter = '--';

  { This string determines the characters that are used to indicate the
    abbreviated name of a command line parameter when it is passed on the
    command line.  The abbreviated form of any command line parameter may be
    specified by prefixing these characters to the abbreviation.
  }
  clopParameterAbbreviationDelimiter = '-';

  { This string determines the character that is used to indicate a value should
    be assigned to a named option.  This character is expected to be placed
    between the name of the option and its value when specified by the user on
    the command line.
  }
  clopAssignmentOperator = '=';

  { This string controls the format of the string used to indicate to the user
    what abbreviations may be given instead of the full name of an option.  It
    is part of the string returned to the caller when
    @link(ACommandLineOption.usageString) is called.

    The first string placeholder is filled with the value of
    @link(clopParameterAbbreviationDelimiter).

    The second string placeholder is filled with a delimited list of
    abbreviations, obtained by calling
    @link(ALinkedList.implode AStringList.implode) on the list returned by
    @link(ACommandLineOption.Abbreviations).  Each element in the delimited list
    is separated by a string that combines the value of
    @link(llstDefaultDelimiter) with the value of
    @link(clopParameterAbbreviationDelimiter).

    The third string placedholder is filled with the value of
    @link(llstDefaultDelimiter).
  }
  clopAbbreviationsRepresentation = '%s%s%s';

  { This string controls the format of the string used to indicate to the user
    which values may be assigned to an option.  It is part of the string
    returned to the caller when @link(ACommandLineOption.usageString) is called
    on an instance of @link(ACommandLineFlag).

    The first string placeholder is filled with the value of
    @link(clopParameterNameDelimiter).

    The second string placeholder is filled with the name of the option, as
    returned by a call to @link(AStringLeaf.name ACommandLineOption.name).
  }
  clopFlagRepresentation = '%s%s';

  { This string controls the format of the string used to indicate to the user
    which values may be assigned to an option.  It is part of the string
    returned to the caller when @link(ACommandLineOption.usageString) is called
    on an option that is not an instance of @link(ACommandLineFlag).

    The first string placeholder is filled with the value of
    @link(clopParameterNameDelimiter).

    The second string placeholder is filled with the name of the option, as
    returned by a call to @link(AStringLeaf.name ACommandLineOption.name).

    The third string placeholder is replaced by the value of
    @link(clopAssignmentOperator).

    The fourth string placeholder contains a description of values that are
    acceptable for the specific option, as returned by a call to
    @link(ACommandLineOption.valueDescription).
  }
  clopAssignmentRepresentation = '%s%s%s%s';

  { This string controls the format of the string returned when
    @link(ACommandLineOption.usageString) is called.

    The first string placeholder is filled with the result of calling
    @code(SysUtils.format) on @link(clopAssignmentRepresentation); it is used to
    indicate to the user what values are acceptable to the option.

    The second string placeholder is filled with padding so that the description
    of the option lines up at the column specified by
    @link(clopDescriptionColumn).

    The third string placeholder is filled with the description of the option,
    as returned by a call to @link(ACommandLineOption.description).
  }
  clopStringRepresentation = '%s%s%s';

  { This string controls the format of the string used to indicate the default
    value of an option to the user.  It is part of the string returned to the
    caller when @link(ACommandLineOption.usageString) is called, but only if the
    value of @link(ACommandLineOption.defaultValue) is not equal to
    @code(Variants.null) and @link(ACommandLineOption.required) is @false, since
    there is little point to specifying a default value for an option that must
    be specified by the user.

    The string placeholder is filled with a string representation of the
    default value, obtained by calling @code(Variants.varToStr) on
    @link(ACommandLineOption.defaultValue).
  }
  clopDefaultValueRepresentation = '  Defaults to %s.';

  { This string controls the format of the error message issued in the event
    that @link(ACommandLineOption.setValue) is given a value that it cannot
    use, based on the definition of the option.

    The first string placeholder is replaced by the name of the option in
    question.

    The second string placeholder is replaced by a description of the value
    expected by the option, as returned by a call to
    @link(ACommandLineOption.valueDescription).
  }
  clopErrorValueExpected = 'the option "%s" expects %s';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription) if no other description is
    set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clopDefaultValueDescription = '<value>';

  { This string contains the default description returned by
    @link(ACommandLineOption.valueDescription ACommandLineString.valueDescription)
    if no other description is set when the option is constructed.

    A value description is presented to the user as part of the usage
    information generated by @link(ACommandLineOption.usageString) and should
    contain a description of the value expected by the option.
  }
  clstDefaultValueDescription = '<string>';
{$ENDIF}
{$IFDEF ReadImplementation}
// ACommandLineOption **********************************************************
// Constructor
constructor ACommandLineOption.named(const thisName: string;
  const isRequired: boolean; const isRest: boolean;
  const thisDefaultValue: Variant;
  const thisDescription: string; const thisValueDescription: string = '';
  const onOptionDirtied: AnOptionDirtiedHandler = nil);

begin
  // Call the inherited routine
  inherited named(thisName, thisDefaultValue);

  myDefaultValue := thisDefaultValue;
  myRequirement := isRequired;

  if isRest then
    myOptionType := myOptionType + [ovtRest];

  myDescription := thisDescription;

  if thisValueDescription <> '' then
    myValueDescription := thisValueDescription;

  myOptionDirtiedHandler := onOptionDirtied;
end;

// Initializer
function ACommandLineOption.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myDefaultValue := Variants.null;
    myValue := Variants.null;
    myOptionType := [ovtNone];
    myRequirement := false;
    myDirtiness := false;
    // Start with the default value description
    myValueDescription := '';
    myDescription := clopDefaultValueDescription;
    myOptionDirtiedHandler := nil;
  end;
end;

// Destructor
destructor ACommandLineOption.destroy;

begin
  // Prevent the inherited destructor from attempting to free our children
  MyChild := nil;

  inherited destroy;
end;

// Obtain a list of abbreviations specified for the option
function ACommandLineOption.Abbreviations: AStringList;

var
  // Refers to a node
  Abbreviation: ANode;

begin
  Result := nil;

  if Self.Child <> nil then
  begin
    Result := AStringList.new;
    if Result = nil then
      exit;

    Abbreviation := Self.Child;
    // Loop through each node
    while Abbreviation <> nil do
    begin
      Result.push(Abbreviation.name);
      Abbreviation := Abbreviation.Previous;
    end;
  end;
end;

// Construct a string representation of how the option should be used
function ACommandLineOption.usageString: AnsiString;

var
  // Refers to a list of option abbreviations
  AbbreviatedForms: AStringList;
  // Stores a string that indicates the parameter prefix
  parameterPrefix: string;

begin
  result := Charstring.ofCharacter(tosWrapDefaultPadCharacter, clopNameColumn);

  // Start with any abbreviations
  AbbreviatedForms := Self.Abbreviations;
  if AbbreviatedForms <> nil then
  begin
    // Construct a string that contains a delimited list of all abbreviations
    result := result + SysUtils.format(clopAbbreviationsRepresentation, [
      clopParameterAbbreviationDelimiter,
      AbbreviatedForms.implode(llstDefaultDelimiter +
        clopParameterAbbreviationDelimiter),
      llstDefaultDelimiter
    ]);

    AbbreviatedForms.free;
  end;

  // Rest arguments do not get a prefix
  parameterPrefix := '';
  if not(ovtRest in Self.optionType) then
    parameterPrefix := clopParameterNameDelimiter;

  // Include the name and value description
  if not(ovtFlag in Self.optionType) then
    result := result + SysUtils.format(clopAssignmentRepresentation, [
      parameterPrefix, Self.name, clopAssignmentOperator,
      Self.valueDescription
    ])

  else
    result := result + SysUtils.format(clopFlagRepresentation, [
      clopParameterNameDelimiter, Self.name
    ]);

  // Now add appropriate padding and description
  result := SysUtils.format(clopStringRepresentation, [
    result,
      Charstring.ofCharacter(tosWrapDefaultPadCharacter,
        clopDescriptionColumn - System.length(result)),
    Self.description
  ]);

  // Finally, add the default value
  if (not (ovtFlag in Self.optionType)) and (Self.required = false) and
    (Self.defaultValue <> Variants.null) and
      (VarToStr(Self.defaultValue) <> '') then
        result := result + SysUtils.format(clopDefaultValueRepresentation, [
          Variants.varToStr(Self.defaultValue)
        ]);
end;

// Construct a shallow copy of the other item
function ACommandLineOption.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  if (result) and (Other is ACommandLineOption) then
  begin
    myDefaultValue := ACommandLineOption(Other).defaultValue;
    myOptionType := ACommandLineOption(Other).optionType;
    myRequirement := ACommandLineOption(Other).required;
    myDirtiness := ACommandLineOption(Other).dirty;
    myValueDescription := ACommandLineOption(Other).valueDescription;
    myDescription := ACommandLineOption(Other).description;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the default value for the option
function ACommandLineOption.defaultValue: Variant;

begin
  result := myDefaultValue;
end;

// Set the current value of the option
function ACommandLineOption.setValue(const newValue: Variant;
  const makeDirty: boolean = true): Variant;

begin
  result := myValue;
  myValue := newValue;
  myDirtiness := makeDirty;
end;

// Retrieve the option type
function ACommandLineOption.optionType: AnOptionValueType;

begin
  result := myOptionType;
end;

// Retrieve whether or not the option is required
function ACommandLineOption.required: boolean;

begin
  result := myRequirement;
end;

// Set whether or not the option is required
function ACommandLineOption.setRequired(const flag: boolean): boolean;

begin
  result := myRequirement;
  myRequirement := flag;
end;

// Determine whether or not the option is a rest argument
function ACommandLineOption.isRestArgument: boolean;

begin
  result := ovtRest in myOptionType;
end;

// Retrieve whether or not the option has been modified
function ACommandLineOption.dirty: boolean;

begin
  result := myDirtiness;
end;

// Set whether or not the option has been modified
function ACommandLineOption.setDirty(const flag: boolean): boolean;

begin
  result := myDirtiness;
  myDirtiness := flag;
end;

// Retrieve the option description
function ACommandLineOption.description: string;

begin
  result := myDescription;
end;

// Retrieve the option's value description
function ACommandLineOption.valueDescription: string;

begin
  result := myValueDescription;
end;

// Retrieve the option dirtied callback
function ACommandLineOption.optionDirtiedHandler: AnOptionDirtiedHandler;

begin
  result := myOptionDirtiedHandler;
end;

// Set the option dirtied callback
function ACommandLineOption.setOptionDirtiedHandler(const newHandler: AnOptionDirtiedHandler): AnOptionDirtiedHandler;

begin
  result := myOptionDirtiedHandler;
  myOptionDirtiedHandler := newHandler;
end;

// ACommandLineString **********************************************************
// Initializer
function ACommandLineString.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myOptionType := [ovtString];
    myValueDescription := clstDefaultValueDescription;
  end;
end;

{$ENDIF}
