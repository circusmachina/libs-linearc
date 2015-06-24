{ linearcstatements.inc - Expression parsers for the LinearC command-line
    "language".

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
// ALinearCExpression **********************************************************
{ This class represents an expression in LinearC.  An expression can be made
  up of a numeric value, a boolean value, or a subexpression (delimited by
  parentheses).
}
type ALinearCExpression = class(AParsedLanguageParser)
  protected
    // Stores the value of the expression
    myValue: Variant;

  public
    { Construct a new expression parser that will use the same scanner and token
      list as the specified parser.

      The new expression inherits all of the properties of @code(ThisParser).
    }
    constructor forSource(const ThisParent: AParser); overload; override;

    // Initializer
    function init: boolean; override;

    { Destroy the expression parser.

      This method @code(nil)s its reference to the parent's token list to
      prevent the inherited routine from freeing it (since the reference is
      borrowed).  It then calls the inherited routine.
    }
    destructor destroy; override;

    { Parse the expression.

      This method checks @link(AParser.CurrentToken Self.CurrentToken) and,
      based on the value of the opcode, this method will construct an instance
      of:
      @unorderedList(
        @item(@link(ALinearCStringExpression) if the current token opcode is
          @link(lcdlQuote1) or @link(lcdlQuote2))
        @item(@link(ALinearCNumericExpression) if the current token opcode is
          @link(TOKCAT_NUMBER))
        @item(@link(ALinearCCompoundExpression) if the current token opcode is
          @link(lcdlLeftParen))
      )

      The value returned from the subexpression will be assigned to
      @link(ALinearCExpression.value Self.value).

      @return(The number of errors encountered while parsing.)
    }
    function parse: int64; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the expression, suitable
      for output to a text-based device such as a console.

      This method calls @code(Variants.varToStr) on
      @link(ALinearCExpression.value Self.value) and returns the result.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the current value of the expression.

      This value is calculated as the expression is parsed and cannot otherwise
      be changed.
    }
    function value: Variant; virtual;

    { Retrieve a reference to the list of command line options with which the
      expression works.

      The caller should NOT attempt to free the reference returned by this
      routine.
    }
    function Options: ACommandLineOptionDictionary; virtual;
end;

// ALinearCBooleanExpression ***************************************************
{ This class represents a boolean expression in LinearC, which is an expression
  that consists of a single keyword belonging to either
  @link(RULE_TRUTH) or @link(RULE_FALSEHOOD).  The expression evaluates the
  keyword and sets its own @link(ALinearCExpression.value value) accordingly.
}
type ALinearCBooleanExpression = class(ALinearCExpression)
  public
    // Initializer
    function init: boolean; override;

    { Parse the expression.

      This method checks to see whether the current token belongs to either
      @link(RULE_TRUTH) or @link(RULE_FALSEHOOD), and sets the
      @link(ALinearCExpression.value value) of the expression accordingly.

      On return from this routine, the current token in the parent expression
      will refer to the token immediately following the boolean expression.
    }
    function parse: int64; override;
end;

// ALinearCIdentifierExpression ************************************************
{ This class represents an identifier expression in LinearC, which is an
  expression that refers to the name of an option.  The value of the option is
  assigned to the value of the expression.
}
type ALinearCIdentifierExpression = class(ALinearCExpression)
  public
    { Parse the expression.

      This method uses the literal text of the current token to look up the
      named option in @link(ALinearCExpression.Options Self.Options).  The value
      of the option is stored as the value of the expression.

      If the token text specifies an option that is not known, then it is
      treated instead as an unquoted string literal and the literal text of
      the token is stored as the value of the expression.

      On return from this routine, the current token in the parent expression
      will refer to the token immediately following the identifier.
    }
    function parse: int64; override;
end;

// ALinearCStringExpression ****************************************************
{ This class represents a string expression in LinearC, which is an expression
  that consists of a single string, delimited by either
  @link(lcdlQuote1String) or @link(lcdlQuote2String).
}
type ALinearCStringExpression = class(ALinearCExpression)
  public
    // Initializer
    function init: boolean; override;

    { Parse the expression.

      This method processes tokens until another delimiter which matched the
      opening delimiter is found.  The literal value of each token is appended
      to the @link(ALinearCExpression.value value) of the expression.

      On return from this routine, the current token in the parent expression
      will refer to the token immediately following the string's closing
      delimiter.
    }
    function parse: int64; override;

    { Convert a C-style escape sequence into a string value.

      This method converts a C-style escape sequence into a string of one or
      more characters that can be appended to a string.  It is called by
      @link(ALinearCStringExpression.parse) whenever it encounters what appears
      to be a C-style escape sequence.
    }
    function convertEscapeSequence: string; virtual;
end;

// ALinearCPathExpression ******************************************************
{ This class represents a path expression in LinearC, which an expression that
  begins with a path delimiter, such as @link(lcopDivideString) or
  @link(lcdlBackslashString) and which is not contained in quotes (otherwise
  it would be seen as a string expression).
}
type ALinearCPathExpression = class(ALinearCStringExpression)
  public
    { Parse the expression.

      This method processes tokens until whitespace is encountered.  The literal
      value of each token is appended to the
      @link(ALinearCExpression.value value) of the expression.

      On return from this routine, the current token in the parent expression
      will refer to the space token (or end-of-stream token) that terminated
      the path.
    }
    function parse: int64; override;
end;

// ALinearCNumericExpression ***************************************************
{ This class represents a numeric expression in LinearC, which is an expression
  that consists of a single number, either integer or floating-point.
}
type ALinearCNumericExpression = class(ALinearCExpression)
  public
    // Initializer
    function init: boolean; override;

    { Parse the expression.

      This method processes the current token, which should evaluate to a
      valid number.  If so, the @link(ANumericConstantToken.value value) of the
      token is assigned to the @link(ALinearCExpression.value) of the
      expression.  If not, this method emits a syntax error.

      The current token encountered on entering this routine should be an
      instance of @link(ANumericConstantToken).  The method checks the
      @link(AToken.opcode opcode) of the token to determine whether the token
      represents a valid number; if it does, the token opcode will evaluate
      to @link(TOKCAT_NUMBER).  Otherwise, it will be @link(TOKCAT_ERROR).

      On return from this routine, the current token in the parent expression
      will refer to the token immediately following the number.
    }
    function parse: int64; override;
end;

// ALinearCCompoundExpression **************************************************
{ This class represents a compound expression in LinearC, which can be made up
  of one or more subexpressions.
}
type ALinearCCompoundExpression = class(ALinearCExpression)
  protected
    { Refers to the operator encountered, if any}
    myOperation: TOpcode;

  public
    { Parse the expression.

      This method simply constructs an instance of ALinearCExpression and
      calls @link(ALinearCExpression.parse) on it.
    }
    function parse: int64; override;

    { Perform the operation indicated by the
      @link(ALinearCCompoundExpression.operation).  This method is called by
      @link(ALinearCCompoundExpression.parse) after two subexpressions,
      separated by an operator, have been parsed.

      Depending on the value of
      @link(ALinearCCompoundExpression.operation Self.operation), this method
      will call
      @link(ALinearCCompoundExpression.performArithmeticOperation Self.performArithmeticOperation),
      @link(ALinearCCompoundExpression.performBitwiseOperation Self.performBitwiseOperation), or
      @link(ALinearCCompoundExpression.performEqualityTestOperation Self.performEqualityTestOperation).
      If the method called returns @false, then this method flags a syntax
      error.

      @return(The total number of errors that occurred while attempting to
      perform the operation.  A value of zero (0) indicates success.)
    }
    function performOperation: int64; virtual;

    { Perform an arithmetic operation that involves
      @link(ALinearCExpression.value Self.value) and the value of
      @link(AParser.CurrentElement Self.CurrentElement).

      The type of value stored in @link(ALinearCExpression.value Self.value)
      determines the type of operation performed: @unorderedList(
        @item(If @link(ALinearCExpression.value Self.value) is a string value,
          then
          @link(AParser.CurrentElement Self.CurrentElement) is cast as a
          string and then: @unorderedList(
            @item(If @link(ALinearCCompoundExpression.operation Self.operation)
              is @link(lcopPlus), the value of
              @link(AParser.CurrentElement Self.CurrentElement) is appended to
              the end of @link(ALinearCExpression.value Self.value).)

            @item(If @link(ALinearCCompoundExpression.operation Self.operation)
              is @link(lcopMinus), then all occurrences of the value specified
              by @link(AParser.CurrentElement Self.CurrentElement) are removed
              from @link(ALinearCExpression.value Self.value).)

            @item(If @link(ALinearCCompoundExpression.operation Self.operation)
              is @link(lcopTimes), then the value in
              @link(ALinearCExpression.value Self.value) is repeated the number
              of times specified by
              @link(AParser.CurrentElement Self.CurrentElement).)
          )
        )

        @item(If @link(ALinearCExpression.value Self.value) is a floating-point
          or integer value, then the value of
          @link(AParser.CurrentElement Self.CurrentElement) is cast to the
          appropriate type and the relevant mathematical operation is performed.
          For division and modulus division, the method ensures that the value
          of
          @link(AParser.CurrentElement Self.CurrentElement) is not zero
          (@code(0)) so as to avoid division-by-zero errors.)

        @item(For all other types, this method fails and returns @false.)
      )

      If an exception is thrown when attempting to cast the value of
      @link(AParser.CurrentElement Self.CurrentElement) to the correct type,
      this method will exit and the caller is expected to handle the exception.
      When called from @link(ALinearCCompoundExpression.performOperation), a
      syntax error message is generated in response to the exception.

      @return(@true if the operation was successfully performed; @false
        otherwise.)
    }
    function performArithmeticOperation: boolean; virtual;

    { Perform a bitwise or logical operation that involves
      @link(ALinearCExpression.value Self.value) and the value of
      @link(AParser.CurrentElement Self.CurrentElement).

      The type of value stored in @link(ALinearCExpression.value Self.value)
      determines the type of operation performed: @unorderedList(
        @item(If @link(ALinearCExpression.value Self.value) is a boolean value
          and @link(ALinearCCompoundExpression.operation Self.operation) is
          @link(lcopAnd) or @link(lcopOr), then the value of
          @link(AParser.CurrentElement Self.CurrentElement) is cast as a
          boolean value and the relevant logical operation is performed.)

        @item(If @link(ALinearCExpression.value Self.value) is an integer value,
          @link(AParser.CurrentElement Self.CurrentElement) is cast to the
          appropriate type and the relevant bitwise operation is performed.)

        @item(For all other types, this method fails and returns @false.)
      )

      If an exception is thrown when attempting to cast the value of
      @link(AParser.CurrentElement Self.CurrentElement) to the correct type,
      this method will exit and the caller is expected to handle the exception.
      When called from @link(ALinearCCompoundExpression.performOperation), a
      syntax error message is generated in response to the exception.

      @return(@true if the operation was successfully performed; @false
        otherwise.)
    }
    function performBitwiseOperation: boolean; virtual;

    { Perform an equality-test operation that involves
      @link(ALinearCExpression.value Self.value) and the value of
      @link(AParser.CurrentElement Self.CurrentElement).

      The type of value stored in @link(ALinearCExpression.value Self.value)
      determines the type of operation performed: @unorderedList(
        @item(If @link(ALinearCExpression.value Self.value) is a string, then
          the value of
          @link(AParser.CurrentElement Self.CurrentElement) is cast as a
          string and @code(SysUtils.ansiCompareStr) is used to determine the
          relative equality of the two string values.)

        @item(If @link(ALinearCExpression.value Self.value) is an integer value
          or a floating point value
          @link(AParser.CurrentElement Self.CurrentElement) is cast to the
          appropriate type and the relevant equality test is performed.)

        @item(For all other types, this method fails and returns @false.)
      )

      If an exception is thrown when attempting to cast the value of
      @link(AParser.CurrentElement Self.CurrentElement) to the correct type,
      this method will exit and the caller is expected to handle the exception.
      When called from @link(ALinearCCompoundExpression.performOperation), a
      syntax error message is generated in response to the exception.

      @return(@true if the operation was successfully performed; @false
        otherwise.)
    }
    function performEqualityTestOperation: boolean; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve the operator encountered while parsing the compound expression,
      if any.

      Although a compound expression will generally consist of two or more
      subexpressions, separated by an operator, this is not required.  It is
      perfectly valid for a compound expression to consist of a single
      subexpression with no operators, in which case this method will return
      zero (0).
    }
    function operation: TOpcode;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the error message emitted by
    @link(ALinearCExpression.parse) when an invalid expression is encountered.

    The first string placeholder is filled with the token that occurred just
    prior to the expected expression, as determined by a call to
    @link(AToken.toString).

    The second string placeholder is filled with the token that should have
    started a valid expression, as determined by a call to
    @link(AToken.toString).
  }
  lcxpErrorValidExpressionExpected = 'expected valid expression after %s; ' +
    'got %s instead';

  { This string controls the format of the error message emitted by
    @link(ALinearCBooleanExpression.parse) when an invalid boolean expression
    is encountered.

    The first string placeholder is filled with the token that occurred just
    prior to the expected expression, as determined by a call to
    @link(AToken.toString).

    The second string placeholder is filled with the token that should have
    started a valid expression, as determined by a call to
    @link(AToken.toString).
  }
  lcbxErrorBooleanExpressionExpected = 'expected boolean expression after %s;' +
    ' got %s instead';

  { This string controls the format of the error message emitted by
    @link(ALinearCCompoundExpression.performOperation) when an attempt is made
    to peform a mathematical operation on incompatible value types.

    The first string placeholder is filled with the value of the left-hand
    operand, as obtained by calling @link(ALinearCExpression.toString) on
    @link(AParser.PreviousElement ALinearCCompoundExpression.PreviousElement).

    The second string placeholder is filled with the value of the right-hand
    operand, as obtained by calling @link(ALinearCExpression.toString) on
    @link(AParser.CurrentElement ALinearCCompoundExpression.CurrentElement).
  }
  lccxErrorInvalidOperation = 'cannot operate on incompatible values: %s and ' +
    '%s';

  { This string controls the format of the error message emitted by
    @link(ALinearCNumericExpression.parse) when it encounters something that
    looks like it should be a number, but which cannot be evaluated as such.

    The string placeholder is filled with a string representation of the
    offending number, as obtained by calling
    @link(ATokenFromSource.toString ANumericConstantToken.toString).
  }
  lcnxErrorInvalidNumber = 'invalid number: %s';

  { This string controls the format of the error message emitted by
    @link(ALinearCStringExpression.convertEscapeSequence) when it encounters
    something that looks like it should be a C-style escape sequence, but
    cannot parse it.

    The string placeholder is filled with a string representatin of the
    offending token, as obtained by calling @link(ATokenFromSource.toString).
  }
  lcsxErrorInvalidEscapeSpecifier = 'invalid escape specifier: %s';
{$ENDIF}
{$IFDEF ReadImplementation}
const
  { This constant controls the maximum number of times that a string can be
    repeated as part of a string-multiplication expression.
  }
  LCSX_STRING_MULTIPLIER_MAX = 255;

// ALinearCExpression **********************************************************
// Constructor
constructor ALinearCExpression.forSource(const ThisParent: AParser);

begin
  inherited forSource(ThisParent);

  { We want to use the parent's token list instead of creating our own }
  if ThisParent <> nil then
    MyTokens := ThisParent.Tokens;
end;

// Initializer
function ALinearCExpression.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myValue := Variants.null;
  end;
end;

// Destructor
destructor ALinearCExpression.destroy;

begin
  { Nil this so that the inherited routine doesn't try to free it }
  MyTokens := nil;

  inherited destroy;
end;

// Parse the expression
function ALinearCExpression.parse: int64;

var
  // Refers to an expression parsed from the source
  Expression: ALinearCExpression;
  // Stores a unary operator
  unaryOperator: TOpcode;

begin
  result := 0;
  Expression := nil;
  unaryOperator := 0;

  // Check for unary operators
  if ATokenFromSource(CurrentToken).isPartOf(RULE_UNARY_OPERATORS) then
  begin
    unaryOperator := CurrentToken.opcode;
    NextToken;
  end;

  // Check the expression
  case CurrentToken.opcode of
    // A subexpression?
    lcdlLeftParen: begin
      // Skip the opening paren
      NextToken;
      Expression := ALinearCCompoundExpression.forSource(Self);
    end;

    // A numeric expression
    TOKCAT_NUMBER:
      Expression := ALinearCNumericExpression.forSource(Self);

    // An option name or a string (unquoted)
    TOKCAT_IDENTIFIER:
      Expression := ALinearCIdentifierExpression.forSource(Self);

  else
    // A string
    if ATokenFromSource(CurrentToken).isPartOf(RULE_DELIMIT_STRING) then
      Expression := ALinearCStringExpression.forSource(Self)

    // A path
    else if ATokenFromSource(CurrentToken).isPartOf(RULE_BEGIN_PATH) then
      Expression := ALinearCPathExpression.forSource(Self)

    // Also a path if we have a unary minus followed by a space or EOS
    else if (ATokenFromSource(CurrentToken).isPartOf(RULE_END_EXPRESSION)) and
      (PreviousToken.isOneOf([lcopMinus, lcdlBeginShortOption])) then
    begin
      // Rewind to the unary minus and parse it again as a path
      Self.rewind;
      Expression := ALinearCPathExpression.forSource(Self);
    end

    // Is the token a boolean value?
    else if ATokenFromSource(CurrentToken).isPartOfAny([
      RULE_TRUTH, RULE_FALSEHOOD]) <> -1 then
        Expression := ALinearCBooleanExpression.forSource(Self)

    // Otherwise, we do not have a valid expression
    else begin
      syntaxError(lcxpErrorValidExpressionExpected, [
        PreviousToken.toString, CurrentToken.toString
      ]);

      inc(result);
    end;
  end;

  // Parse the expression
  if Expression <> nil then
  begin
    if Expression is ALinearCCompoundExpression then
      ALinearCCompoundExpression(Expression).parseUntil(RULE_END_EXPRESSION)

    else
      Expression.parse;

    myValue := Expression.value;
    Expression.free;
  end;

  // Apply the unary operator, if any, if our value was successfully set
  if (not(Variants.varIsNull(myValue))) and (unaryOperator <> 0) then
    case unaryOperator of
      lcopNot:
        // The 'not' operator can only be applied to certain types of values
        if (Variants.varIsBool(myValue)) or
          (Variants.varIsNumeric(myValue)) then
          myValue := not(myValue);

      lcopMinus:
        // The unary minus operator can only be applied to certain value types
        if Variants.varIsNumeric(myValue) then
          myValue := -(myValue);
    end;

  // Resync to the end of the expression
  //Self.resyncTo(RULE_END_EXPRESSION);
end;

// CanPrint implementation -----------------------------------------------------
// Obtain a string representation of the expression
function ALinearCExpression.toString: AnsiString;

begin
  result := Variants.varToStr(myValue);
end;

// Properties ------------------------------------------------------------------
// Obtain the value of the expression
function ALinearCExpression.value: Variant;

begin
  result := myValue;
end;

// Retrieve a reference to the list of command line options
function ALinearCExpression.Options: ACommandLineOptionDictionary;

begin
  Result := nil;

  if MyParent is ALinearCParser then
    Result := ALinearCParser(MyParent).Options

  else if MyParent is ALinearCStatement then
    Result := ALinearCStatement(MyParent).Options

  else if MyParent is ALinearCExpression then
    Result := ALinearCExpression(MyParent).Options;
end;


// ALinearCBooleanExpression ***************************************************
// Initializer
function ALinearCBooleanExpression.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myValue := false;
  end;
end;

// Parse the expression
function ALinearCBooleanExpression.parse: int64;

begin
  result := -1;

  if CurrentToken = nil then
    exit;

  result := 0;

  // Check the current token
  // Does it evaluate as true?
  if ATokenFromSource(CurrentToken).isPartOf(RULE_TRUTH) then
    myValue := true

  // Or does it evaluate as false?
  else if ATokenFromSource(CurrentToken).isPartOf(RULE_FALSEHOOD) then
    myValue := false

  // Otherwise, we do not have a valid expression
  else begin
    syntaxError(lcbxErrorBooleanExpressionExpected, [
      PreviousToken.toString, CurrentToken.toString
    ]);

    inc(result);
  end;

  // Skip past the token
  NextToken;
end;

// ALinearCIdentifierExpression ************************************************
// Parse the identifier
function ALinearCIdentifierExpression.parse: int64;

begin
  result := 0;

  // Look up the value
  if Self.Options.hasSymbolNamed(ATokenFromSource(CurrentToken).text) then
    myValue := Self.Options.valueOf(ATokenFromSource(CurrentToken).text)

  else
    myValue := ATokenFromSource(CurrentToken).text;

  NextToken;
end;

// ALinearCStringExpression ****************************************************
// Initializer
function ALinearCStringExpression.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myValue := '';
  end;
end;

// Parse the string
function ALinearCStringExpression.parse: int64;

var
  // Stores the opening delimiter
  openingDelimiter: TOpcode;
  // Stores the behavior of the underlying scanner
  scannerBehavior: TScannerTokenBehaviors;

begin
  result := 0;

  { Whitespace is important }
  scannerBehavior := MyScanner.tokenBehaviors;
  MyScanner.setTokenBehaviors(scannerBehavior -
    [SCAN_NO_WHITESPACE, SCAN_CONSOLIDATE_WHITESPACE]
  );

  { Store the opening delimiter.  This same delimiter must be used to close
    the string.
  }
  openingDelimiter := CurrentToken.opcode;

  // Skip past the opening delimiter
  NextToken;

  while CurrentToken.opcode <> openingDelimiter do
  begin
    // Flag an unexpected end of stream
    if CurrentToken.opcode = TOKCAT_EOS then
      raise FatalError(parsFatalUnexpectedEOS, []);

    // Handle C escapes
    if CurrentToken.opcode = lcdlBackslash then
      myValue := myValue + Self.convertEscapeSequence

    else
      // Add the token literal
      myValue := myValue + ATokenFromSource(CurrentToken).text;

    // Get the next token
    NextToken;
  end;

  // Restore scanner behavior
  MyScanner.setTokenBehaviors(scannerBehavior);

  // Skip past the ending token
  NextToken;

  result := Self.Log.Counter(lgerCounterName).value;
end;

// Return a string built out of a C escape sequence
function ALinearCStringExpression.convertEscapeSequence: string;

var
  // Stores the escape character
  escapeCharacter: string;
  // Stores the remaining text
  remainingText: string;

begin
  result := '';

  // Skip the backslash
  NextToken;

  // Do we have an identifier?
  if CurrentToken.opcode = TOKCAT_IDENTIFIER then
  begin
    remainingText := ATokenFromSource(CurrentToken).text;

    // Check the first character for an escape sequence
    escapeCharacter := Charstring.character(remainingText, 1);
    // Strip the first character from the remaining text
    remainingText := copy(remainingText, System.length(escapeCharacter) + 1,
      System.length(remainingText));

    case escapeCharacter of
      'a':
        result := #7;

      'b':
        result := #8;

      'f':
        result := #$C;

      'n':
        result := System.lineEnding;

      'r':
        result := #$D;

      't':
        result := #9;

      'v':
        result := #$B;

    else
      syntaxError(lcsxErrorInvalidEscapeSpecifier, [CurrentToken.toString]);
    end;

    result := result + remainingText;
  end

  // A backslash character or quote?
  else
    case CurrentToken.opcode of
      lcdlBackslash, lcdlQuote1, lcdlQuote2:
        result := ATokenFromSource(CurrentToken).text;

    else
      // A number?
      if CurrentToken.opcode = TOKCAT_NUMBER then
        result :=
          Charstring.fromUnicodeCharacter(trunc(ANumericConstantToken(CurrentToken).value));
    end;

  if result = '' then
    syntaxError(lcsxErrorInvalidEscapeSpecifier, [CurrentToken.toString]);
end;

// ALinearCPathExpression ******************************************************
// Parse the path
function ALinearCPathExpression.parse: int64;

var
  // Stores the behavior of the underlying scanner
  scannerBehavior: TScannerTokenBehaviors;

begin
  result := 0;

  { Whitespace is important }
  scannerBehavior := MyScanner.tokenBehaviors;
  MyScanner.setTokenBehaviors(scannerBehavior -
    [SCAN_NO_WHITESPACE, SCAN_CONSOLIDATE_WHITESPACE]
  );

  while CurrentToken.opcode <> TOKCAT_SPACE do
  begin
    // Exit if the end of the stream is reached
    if CurrentToken.opcode = TOKCAT_EOS then
      break;

    // Add the token literal
    myValue := myValue + ATokenFromSource(CurrentToken).text;
    // Get the next token
    NextToken;
  end;

  // Restore scanner behavior
  MyScanner.setTokenBehaviors(scannerBehavior);

  // DON'T skip past the ending token!

  result := Self.Log.Counter(lgerCounterName).value;
end;

// ALinearCNumericExpression ***************************************************
// Initializer
function ALinearCNumericExpression.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myValue := 0;
  end;
end;

// Parse the number
function ALinearCNumericExpression.parse: int64;

begin
  result := 0;

  // Check that the current token is a valid number
  if (CurrentToken is ANumericConstantToken) and
    (CurrentToken.opcode = TOKCAT_NUMBER) then
    myValue := ANumericConstantToken(CurrentToken).value

  else begin
    syntaxError(lcnxErrorInvalidNumber, [CurrentToken.toString]);
    inc(result);
  end;

  // Skip past the token
  NextToken;
end;

// ALinearCCompoundExpression **************************************************
// Parse the compound expression
function ALinearCCompoundExpression.parse: int64;

var
  // Refers to the previous expression that was parsed
  MyPreviousExpression: ALinearCExpression;
  // Refers to the current expression being parsed
  MyCurrentExpression: ALinearCExpression;

begin
  result := 0;
  MyPreviousExpression := ALinearCExpression(Self.PreviousElement);

  { When we get here, the current token should be either an operator or an
    expression
  }
  if ATokenFromSource(CurrentToken).isPartOf(RULE_BEGIN_EXPRESSION) then
  begin
    // Swap expressions
    if MyPreviousExpression <> nil then
    begin
      // Keep the previous element from attempting to free everything
      MyPreviousExpression.detach;
      MyPreviousExpression.free;
    end;

    MyPreviousExpression := ALinearCExpression(Self.CurrentElement);

    // Parse the expression
    MyCurrentExpression := ALinearCExpression.forSource(Self);
    MyCurrentExpression.parse;

    { If this is the first subexpression encountered, then set our value, since
      it is valid for a compound expression to consist solely of a single
      subexpression.
    }
    if MyPreviousExpression = nil then
      myValue := MyCurrentExpression.value;
  end;

  { Check for an operator }
  if ATokenFromSource(CurrentToken).isPartOf(RULE_OPERATOR) then
    // Store the operation
    myOperation := CurrentToken.opcode

  { Check for a valid end of expression }
  else if ATokenFromSource(CurrentToken).isPartOf(RULE_END_EXPRESSION) = false
    then
      syntaxError(lcxpErrorValidExpressionExpected, [
        PreviousToken.toString, CurrentToken.toString
      ]);

  // If we have an operator and two subexpressions, then perform the operation
  if (MyPreviousExpression <> nil) and (MyCurrentExpression <> nil) and
    (myOperation <> 0) then
    result := Self.performOperation;

  result := Self.Log.Counter(lgerCounterName).value;

  { We don't move to the next token here because the 'parseUntil' loop that
    calls this routine will do so.
  }
end;

// Perform the operation
function ALinearCCompoundExpression.performOperation: int64;

var
  // Indicates whether the operation was successful
  operationSucceeded: boolean;

begin
  result := -1;
  if (PreviousElement = nil) or (CurrentElement = nil) or (myOperation = 0) then
    exit;

  operationSucceeded := false;
  result := 0;

  try
    { Perform the operation.  myValue is set by Self.parse after the first
      subexpression is encountered.  Since it is the left-hand operand, its
      type will determine how we handle the various operations.
    }

    // Perform an arithmetic operation
    if Self.Language.SyntaxRule(RULE_ARITHMETIC_OPERATORS).has(myOperation) then
      operationSucceeded := Self.performArithmeticOperation

    // Perform a bitwise operation
    else if Self.Language.SyntaxRule(RULE_BITWISE_OPERATORS).has(myOperation)
      then
      operationSucceeded := Self.performBitwiseOperation

    // Peform an equality-test operation
    else if Self.Language.SyntaxRule(RULE_EQUALITY_OPERATORS).has(myOperation)
      then
      operationSucceeded := Self.performEqualityTestOperation;

    if operationSucceeded = false then
    begin
      syntaxError(lccxErrorInvalidOperation, [
        PreviousElement.toString, CurrentElement.toString
      ]);

      inc(result);
    end;

  except
    { This should catch any typecast errors generated by attempting to cast
      the right-hand operand to a type that is compatible with the desired
      operation.  If the right hand operand cannot be cast to an appropriate
      type, then we have an invalid expression.
    }
    on E: EVariantTypecastError do
    begin
      syntaxError(lccxErrorInvalidOperation, [
        PreviousElement.toString, CurrentElement.toString
      ]);

      inc(result);

      E.free;
    end;
  end;
end;

// Perform an arithmetic operation
function ALinearCCompoundExpression.performArithmeticOperation: boolean;

var
  // Stores the left-hand operand
  leftHand: Variant;
  // Stores the right-hand operand
  rightHand: Variant;

begin
  result := false;

  try
    { myValue is set by Self.parse after the first subexpression is encountered.
      Since it is the left-hand operand, its type will determine how we handle
      the various operations.
    }
    leftHand := myValue;

    if varIsStr(leftHand) then
      rightHand := varToStr(ALinearCExpression(CurrentElement).value)

    else if varIsFloat(leftHand) then
      rightHand := double(ALinearCExpression(CurrentElement).value)

    else if varType(leftHand) in ordinalVarTypes then
      rightHand := int64(ALinearCExpression(CurrentElement).value)

    else
      // No other types are supported
      exit;

    // Perform the operation
    case myOperation of
      lcopPlus: begin
        myValue := leftHand + rightHand;
        result := true;
      end;

      lcopMinus: begin
        // Remove all occurrences of the given substring
        if varIsStr(leftHand) then
          myValue := StrUtils.ansiReplaceStr(varToStr(leftHand),
            varToStr(rightHand), '')

        else
          myValue := leftHand - rightHand;

        result := true;
      end;

      lcopTimes: begin
        // Fill a string with a repeating value
        if varIsStr(leftHand) then
        begin
          rightHand := ALinearCExpression(CurrentElement).value;

          if varIsNumeric(rightHand) then
          begin
            // Cap the number of repetitions to prevent buffer overruns
            if rightHand > LCSX_STRING_MULTIPLIER_MAX then
              rightHand := LCSX_STRING_MULTIPLIER_MAX;

            myValue := Charstring.ofString(varToStr(leftHand),
              trunc(rightHand));

            result := true;
          end;
        end

        else begin
          myValue := leftHand * rightHand;
          result := true;
        end;
      end;

      lcopDivide: begin
        // This operation cannot be performed on strings
        if varIsNumeric(leftHand) then
        begin
          // Prevent division by zero
          if rightHand <> 0 then
          begin
            if varIsFloat(leftHand) then
              myValue := leftHand / rightHand

            else
              myValue := leftHand div rightHand;

            result := true;
          end;
        end;
      end;

      lcopModulo: begin
        // This operation cannot be performed on strings
        if varIsNumeric(leftHand) then
        begin
          // Prevent division by zero
          if rightHand <> 0 then
          begin
            myValue := leftHand mod rightHand;
            result := true;
          end;
        end;
      end;

      lcopExponent: begin
        // This operation cannot be performed on strings
        if varIsNumeric(leftHand) then
        begin
          if varIsFloat(leftHand) then
            myValue := Math.power(leftHand, rightHand)

          else
            myValue := trunc(Math.power(leftHand, rightHand));

          result := true;
        end;
      end;
    end;

  finally
    // Do nothing; let the caller handle exceptions
  end;
end;

// Perform a bitwise operation
function ALinearCCompoundExpression.performBitwiseOperation: boolean;

var
  // Stores the left-hand operand
  leftHand: Variant;
  // Stores the right-hand operand
  rightHand: Variant;

begin
  result := false;

  try
    { myValue is set by Self.parse after the first subexpression is encountered.
      Since it is the left-hand operand, its type will determine how we handle
      the various operations.
    }
    leftHand := myValue;

    if varIsBool(leftHand) then
      rightHand := boolean(ALinearCExpression(CurrentElement).value)

    else if varType(leftHand) in ordinalVarTypes then
      rightHand := int64(ALinearCExpression(CurrentElement).value)

    else
      // These operation cannot be applied to any other type
      exit;

    // Perform the operation
    case myOperation of
      lcopAnd: begin
        myValue := leftHand and rightHand;
        result := true;
      end;

      lcopOr: begin
        myValue := leftHand or rightHand;
        result := true;
      end;

      lcopXor: begin
        // This operation cannot be applied to boolean values
        if not(varIsBool(leftHand)) then
        begin
          myValue := leftHand xor rightHand;
          result := true;
        end;
      end;

      lcopShl: begin
        // This operation cannot be applied to boolean values
        if not(varIsBool(leftHand)) then
        begin
          myValue := leftHand shl rightHand;
          result := true;
        end;
      end;

      lcopShr: begin
        // This operation cannot be applied to boolean values
        if not(varIsBool(leftHand)) then
        begin
          myValue := leftHand shr rightHand;
          result := true;
        end;
      end;
    end;

  finally
    // Do nothing; let the caller handle exceptions
  end;
end;

// Perform an equality-test operation
function ALinearCCompoundExpression.performEqualityTestOperation: boolean;

var
  // Stores the left-hand operand
  leftHand: Variant;
  // Stores the right-hand operand
  rightHand: Variant;

begin
  result := false;

  try
    { myValue is set by Self.parse after the first subexpression is encountered.
      Since it is the left-hand operand, its type will determine how we handle
      the various operations.
    }
    leftHand := myValue;

    if varIsStr(leftHand) then
      rightHand := varToStr(ALinearCExpression(CurrentElement).value)

    else if varIsFloat(leftHand) then
      rightHand := double(ALinearCExpression(CurrentElement).value)

    else if varType(leftHand) in ordinalVarTypes then
      rightHand := int64(ALinearCExpression(CurrentElement).value)

    else
      // No other types are supported
      exit;

    // Perform the operation
    case myOperation of
      lcopEquality: begin
        myValue := leftHand = rightHand;
        result := true;
      end;

      lcopInequality: begin
        myValue := leftHand <> rightHand;
        result := true;
      end;

      lcopGreater: begin
        if varIsStr(leftHand) then
          myValue := SysUtils.ansiCompareStr(varToStr(leftHand),
            varToStr(rightHand)) > 0

        else
          myValue := leftHand > rightHand;

        result := true;
      end;

      lcopGreaterOrEqual: begin
        if varIsStr(leftHand) then
          myValue := SysUtils.ansiCompareStr(varToStr(leftHand),
            varToStr(rightHand)) >= 0

        else
          myValue := leftHand >= rightHand;

        result := true;
      end;

      lcopLesser: begin
        if varIsStr(leftHand) then
          myValue := SysUtils.ansiCompareStr(varToStr(leftHand),
            varToStr(rightHand)) < 0

        else
          myValue := leftHand < rightHand;

        result := true;
      end;

      lcopLesserOrEqual: begin
        if varIsStr(leftHand) then
          myValue := SysUtils.ansiCompareStr(varToStr(leftHand),
            varToStr(rightHand)) <= 0

        else
          myValue := leftHand <= rightHand;

        result := true;
      end;
    end;

  finally
    // Do nothing; let the caller handle exceptions
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the operator used
function ALinearCCompoundExpression.operation: TOpcode;

begin
  result := myOperation;
end;

{$ENDIF}

