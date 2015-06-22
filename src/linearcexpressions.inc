{ linearcstatements.inc - Expression parsers for the LinearC command-line
    "language".

  This file is part of commandline.pp, a library for robust command-line
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

      @return(The total number of errors that occurred while attempting to
      perform the operation.  A value of zero (0) indicates success.)
    }
    function performOperation: int64; virtual;

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
{$ENDIF}
{$IFDEF ReadImplementation}
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

    // A string (unquoted)
    TOKCAT_IDENTIFIER:
      Expression := ALinearCStringExpression.forSource(Self);

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
    Result := ALinearCParser(MyParent).Options;
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

  if CurrentToken.opcode = TOKCAT_IDENTIFIER then
  begin
    myValue := ATokenFromSource(CurrentToken).text;
    NextToken;
    exit;
  end;

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
    case myOperation of
      lcopPlus:
        if VarIsStr(myValue) then
        begin
          myValue := myValue +
            VarToStr(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end

        else if VarIsNumeric(myValue) then
        begin
          myValue := myValue +
            double(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopMinus:
        if VarIsNumeric(myValue) then
        begin
          myValue := myValue -
            double(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopTimes:
        if VarIsNumeric(myValue) then
        begin
          myValue := myValue *
            double(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopDivide:
        // Prevent attempts to divide by zero
        if (VarIsNumeric(myValue)) and
          (double(ALinearCExpression(CurrentElement).value) <> 0) then
        begin
          myValue := myValue /
            double(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopModulo:
        // Prevent attempts to divide by zero
        if (VarIsNumeric(myValue)) and
          (int64(ALinearCExpression(CurrentElement).value) <> 0) then
        begin
          myValue := myValue mod
            int64(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopExponent:
        if VarIsNumeric(myValue) then
        begin
          myValue := Math.power(myValue,
            double(ALinearCExpression(CurrentElement).value));

          operationSucceeded := true;
        end;

      lcopShl:
        // Works only on integer values
        if (VarIsNumeric(myValue)) and (not(VarIsFloat(myValue))) then
        begin
          myValue := myValue shl
            longword(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopShr:
        // We can do this only to integer values
        if (VarIsNumeric(myValue)) and (not(VarIsFloat(myValue))) then
        begin
          myValue := myValue shr
            longword(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopAnd:
        // We can do this only to integer values
        if (VarIsNumeric(myValue)) and (not(VarIsFloat(myValue))) then
        begin
          myValue := myValue and
            int64(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;

      lcopOr:
        // We can do this only to integer values
        if (VarIsNumeric(myValue)) and (not(VarIsFloat(myValue))) then
        begin
          myValue := myValue or
            int64(ALinearCExpression(CurrentElement).value);

          operationSucceeded := true;
        end;
    end;

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

// Properties ------------------------------------------------------------------
// Retrieve the operator used
function ALinearCCompoundExpression.operation: TOpcode;

begin
  result := myOperation;
end;

{$ENDIF}

