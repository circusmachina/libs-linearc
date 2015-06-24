{ linearcstatements.inc - Statements parsers for the LinearC command-line
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
// ALinearCStatement ***********************************************************
{ This class represents a single statement that is pared by the LinearC parser.
  It is unlikely that this class will be instantiated directly, since it does
  not implement @link(AParser.parse AStatement.parse); instead one of its
  descendants will be constructed.
}
type ALinearCStatement = class(ALinearCParser);

// ALinearCAssignmentStatement *************************************************
{ This class represents an assignment statement in LinearC; that is, the portion
  of a command line where a value is assigned to a named option.
}
type ALinearCAssignmentStatement = class(ALinearCStatement)
  protected
    { Parse a special-case uses of an assignment statement.

      This method handles a couple of special cases where an assignment
      statement might be constructed where it is not actually needed.  Remember
      that assignment statements are constructed whenever the parser encounters
      @link(clopParameterNameDelimiter) or
      @link(clopParameterAbbreviationDelimiter), since these tokens are
      typically used to provide the name of an option to set.  However, in two
      instances these tokens might be specified by themselves on the command
      line:
      @unorderedList(
        @item(If @link(clopParameterNameDelimiter) is specified by itself on
          the command line, then that indicates that the user wants any all
          arguments which follow to be treated as rest arguments.)

        @item(If @link(clopParameterAbbreviationDelimiter) is specified by
          itself on the command line, then that typically indicates that the
          user wants to specify @code(stdin) or @code(stdout) as the value of
          a string option (which presumably represents a path or file name).  We
          parse the token as a rest argument.)
      )

      @return(The number of errors encountered while parsing.)
    }
    function parseSpecialCase: int64; virtual;

  public
    { Parse the assignment statement.

      @return(The number of errors encountered while parsing.)
    }
    function parse: int64; override;
end;

// ALinearCRestArgument ********************************************************
{ This class represents a statement in LinearC that is composed entirely of a
  @link(TOptionValueType rest argument); that is, an argument which does
  not include the name of an existing option.
}
type ALinearCRestArgument = class(ALinearCStatement)
  public
    { Parse the rest argument.

      This method will validate that the option named exists and the value
      assigned to it is correct; if not, it will emit a syntax error.

      @return(The number of errors encountered while parsing.)
    }
    function parse: int64; override;
end;

// ALinearCStatementBlock ******************************************************
{ This class represents a statement block that is parsed by the LinearC parser.
  In all actuality, there will probably only ever be one of these constructed,
  since the only block-level construct in a command line language is the
  entirety of the command line itself.
}
type ALinearCStatementBlock = class(ALinearCStatement)
  public
    { Parse the statements in the statement block.

      Based on the value of @link(AParser.CurrentToken CurrentToken), this
      method will construct one of the following instances of
      @link(ALinearCStatement) to parse a portion of the command line:
      @unorderedList(
        @item(@link(ALinearCAssignmentStatement) when the current token is
          @link(lcdlBeginLongOption) or @link(lcdlBeginShortOption))
        @item(@link(ALinearCRestArgument) when the current token is
          @link(TOKCAT_IDENTIFIER) or when the current token is allowed by
          @link(RULE_DELIMIT_STRING) or @link(RULE_BEGIN_PATH).)
      )

      @return(The number of errors encountered while parsing.)
    }
    function parse: int64; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the warning issued in the event that
    @link(ALinearCAssignmentStatement.parse) encounters an attempt to name an
    option on the command line that was not defined in its list of available
    options.

    The string placedholder is replaced by the name of the unknown option.
  }
  lcstWarningUnknownOption = 'ignoring unknown option %s';
{$ENDIF}
{$IFDEF ReadImplementation}
// ALinearCAssignmentStatement *************************************************
// Handle special cases
function ALinearCAssignmentStatement.parseSpecialCase: int64;

var
  // Refers to one or more remaining arguments on the command line
  RemainingArgument: ALinearCStatement;

begin
  result := -1;

  case CurrentToken.opcode of
    { If the current token is '--', then we treat any and all remaining
      arguments as rest arguments
    }
    lcdlBeginLongOption:
      begin
        // Check the next token
        Self.NextToken;

        if CurrentToken.opcode <> TOKCAT_EOS then
        begin
          // Skip the space
          if CurrentToken.opcode = TOKCAT_SPACE then
            Self.NextToken;

          RemainingArgument := ALinearCRestArgument.forSource(Self);
          result := RemainingArgument.parseUntilToken(TOKCAT_EOS);
          RemainingArgument.free;
        end

        else
          result := 0;
      end;

    { If the previous token was '-', then we are looking for a string rest
      argument for a path that represents stdin or stdout
    }
    lcopMinus, lcdlBeginShortOption:
      begin
        // Parse the argument
        RemainingArgument := ALinearCRestArgument.forSource(Self);
        result := RemainingArgument.parse;
        RemainingArgument.free;
      end;
    end;
end;

// Parse the assignment expression
function ALinearCAssignmentStatement.parse: int64;

var
  // Stores the name of the option to set
  optionName: string;
  // Refers to an expression parsed from the source
  Expression: ALinearCExpression;
  // Refers to the option in question
  ThisOption: ACommandLineOption;

begin
  result := -1;

  if not(Self.readyToParse) then
    exit;

  result := 0;

  // The next token should contain the name of the option to set...
  Self.NextToken;

  // ...unless it's a space or end-of-stream, in which case...
  if ATokenFromSource(CurrentToken).isPartOf(RULE_END_EXPRESSION) then
  begin
    // Put it back and parse a special case
    Self.rewind;
    result := Self.parseSpecialCase;
    exit;
  end;

  optionName := ATokenFromSource(Self.CurrentToken).text;
  // Get the named option
  ThisOption := Self.Options.Option(optionName);
  // If not found, then we have an error (one that can be ignored, though)
  if ThisOption = nil then
    warn(lcstWarningUnknownOption, [Charstring.quote(optionName)]);

  // Check if we have an assignment operator or whitespace
  Self.NextToken;
  if ATokenFromSource(CurrentToken).isPartOf(RULE_PRECEDE_ASSIGNMENT) then
    // Skip the assignment operator or whitespace
    Self.NextToken;

  // Ensure we have what appears to be a valid expression
  if ATokenFromSource(CurrentToken).isPartOf(RULE_BEGIN_EXPRESSION) then
  begin
    { Parse the expression, the value of which will be assigned to the named
      option
    }
    Expression := ALinearCCompoundExpression.forSource(Self);
    result := Expression.parseUntil(RULE_END_EXPRESSION);
    Self.resyncTo(RULE_END_EXPRESSION);

    if ThisOption <> nil then
      { Try to set the option value }
      try
        ThisOption.setValue(Expression.value);

      { If it fails because the expression does not evaluate to the right kind
        of value, then an exception will be raised by the option.  We capture it
        and forward the exception message, which indicates to the user the
        appropriate type of value accepted by the option.
      }
      except
        on E: AnException do
        begin
          syntaxError(E.message, []);
          inc(result);
        end;
      end;

    Expression.free;
  end

  // No assignment operator; do we have a valid flag option?
  else if ThisOption <> nil then
  begin
    // Check to see if we have a valid flag option
    if ovtFlag in ThisOption.optionType then
      ThisOption.setValue(true)

    // Otherwise, we have an error
    else begin
      syntaxError(clopErrorValueExpected, [
        optionName, ThisOption.valueDescription
      ]);
      inc(result);
    end;
  end;
end;

// ALinearCRestArgument ********************************************************
// Parse the rest argument
function ALinearCRestArgument.parse: int64;

var
  // Refers to an expression parsed from the source
  Expression: ALinearCExpression;
  // Refers to the option in question
  ThisOption: ACommandLineOption;

begin
  result := -1;

  if not(Self.readyToParse) then
    exit;

  // Ensure we have a valid token list
  CurrentToken;

  { Parse the expression, the value of which will be assigned to the named
    option
  }
  Expression := ALinearCCompoundExpression.forSource(Self);
  result := Expression.parseUntil(RULE_END_EXPRESSION);
  Self.resyncTo(RULE_END_EXPRESSION);

  { Find a rest argument to which the value can be assigned }
  ThisOption := Self.Options.FirstCleanRestArgument;

  if ThisOption <> nil then
  begin
    { Try to set the option value }
    try
      ThisOption.setValue(Expression.value);

    { If it fails because the expression does not evaluate to the right kind of
      value, then an exception will be raised by the option.  We capture it and
      forward the exception message, which indicates to the user the appropriate
      type of value accepted by the option.
    }
    except
      on E: AnException do
      begin
        syntaxError(E.message, []);
        inc(result);
        E.free;
      end;
    end;
  end

  else
    warn(lcstWarningUnknownOption, [Charstring.quote(Expression.toString)]);

  Expression.free;

  result := Self.Log.Counter(lgerCounterName).value;
end;

// ALinearCStatementBlock ******************************************************
// Parse the block
function ALinearCStatementBlock.parse: int64;

var
  // Refers to the sub-statement to be parsed
  Statement: ALinearCStatement;

begin
  result := -1;

  if not(Self.readyToParse) then
    exit;

  result := 0;

  Statement := nil;

  // Check the current token
  case CurrentToken.opcode of
    lcOpMinus, lcdlBeginLongOption, lcdlBeginShortOption:
      Statement := ALinearCAssignmentStatement.forSource(Self);

    TOKCAT_IDENTIFIER:
      Statement := ALinearCRestArgument.forSource(Self);

    TOKCAT_SPACE:
      // Ignore white space at the beginning of the block
      Statement := nil;

  else
    if ATokenFromSource(CurrentToken).isPartOfAny([RULE_DELIMIT_STRING,
      RULE_BEGIN_PATH]) <> -1 then
        Statement := ALinearCRestArgument.forSource(Self)

    else
      warn(lcstWarningUnknownOption, [CurrentToken.toString]);
  end;

  if Statement <> nil then
  begin
    result := result + Statement.parse;
    freeAndNil(Statement);
    Self.resyncTo(RULE_END_STATEMENT);
  end;
end;
{$ENDIF}

