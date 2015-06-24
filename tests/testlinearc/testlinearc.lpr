{ This program is built to test the LinearC parser }
program testlinearc;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  sysUtils, classwork, parsing, linearc;

type MyCustomApplication = class(ALinearCConsoleApplication)
  protected
    // Define command line options
    procedure defineOptions; override;

end;

procedure MyCustomApplication.defineOptions;

begin
  inherited defineOptions;

  // Test an integer range
  MyOptions.add(ACommandLineIntegerRange.named('thrust',    // name
    true,                                                   // is required?
    false,                                                  // is rest?
    0,                                                      // default value
    -100,                                                      // minimum
    100,                                                    // maximum
    'Determine the level of thrust used by the application.'),// description
    []);                                                    // abbreviations

  // Test a floating-point range
  MyOptions.add(ACommandLineFloatRange.named('pitch',       // name
    true,                                                   // is required?
    false,                                                  // is rest?
    0.0,                                                    // default value
    0.0,                                                    // minimum
    359.99,                                                 // maximum
    'Determine the pitch of the application.'),             // description
    ['p']);                                                 // abbreviation

  // Test a switch
  MyOptions.add(ACommandLineSwitch.named('complain',        // name
    false,                                                  // is required?
    false,                                                  // is rest?
    false,                                                  // default value
    'Whether or not the application should complain on errors.'),// description
    ['c']);                                                 // abbreviations

  // Test a choice
  MyOptions.add(ACommandLineChoice.named('flavor',          // name
    false,                                                  // is required?
    false,                                                  // is rest?
    'mint',                                                 // default value
    ['chocolate', 'vanilla', 'mint', 'strawberry'],         // values accepted
    'Determine the flavor of the application.'),            // description
    []);                                                    // abbreviations

  // Test a list
  MyOptions.add(ACommandLineList.named('include',           // name
    false,                                                  // is required?
    false,                                                  // is rest?
    '',                                                     // default value
    'The name of a file to include.',                       // description
    '<path to file>'),                                      // value description
    ['I']);                                                 // abbreviations

  // Test a rest argument
  MyOptions.add(ACommandLineList.named('file',              // name
    true,                                                   // is required?
    true,                                                   // is rest?
    '',                                                     // default value
    'The names of one or more files to read.',              // description
    '<path to file>'),                                      // value description
    []);                                                    // abbreviations

end;

var
  MyApplication: MyCustomApplication;
  ThisOption: AStringLeaf;

{$R *.res}

begin
  MyApplication := nil;

  try
    MyApplication := MyCustomApplication.new;

    exitCode := MyApplication.run;

    //if exitCode = 0 then
    //begin
      writeln(MyApplication.Options.toString);
      ThisOption := AStringLeaf(MyApplication.Options.Root);

      while ThisOption <> nil do
      begin
        if not(ThisOption is ACommandLineAbbreviation) then
        begin
          if ovtList in ACommandLineOption(ThisOption).optionType then
            writeln(#9, ThisOption.name, '=', ACommandLineList(ThisOption).Values.implode)

          else
            writeln(#9, ThisOption.toString);
        end;

        ThisOption := AStringLeaf(ThisOption.Next);
      end;
    //end;

  except
    on AE: AnException do
      // Instances of ALoggedItem have already been printed
      if not(AE is ALoggedItem) then
        AE.print;

    on E: Exception do
      writeln(E.message);
  end;

  if MyApplication <> nil then
    MyApplication.free;
end.

