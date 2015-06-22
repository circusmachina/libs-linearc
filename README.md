**linearc.pp**: A command-line parser.
Copyright (C) 2014-2015 by [Michael Malicoat][Author]  
[Home page][] | [Source][] | [Source documentation][] (you're looking at it) |
[Bug tracker][]

This is a library that can be used with [Free Pascal][] sources to define and
parse command-line options.

The library defines a [robust parser][ALinearCParser] that can process 
command-line arguments, as well as an 
[application framework][ALinearCConsoleApplication] that builds and executes
the LinearC parser automatically.

In general, the LinearC parser follows the GNU standards for 
[Command-Line Interfaces][1] and [Argument Syntax][2].  One notable exception is
that the parser allows abbreviated options to have more than a single character.

For information on how to use the parser in your own source, see the
[series of articles][articles] that I've written on the subject.  Note that 
LinearC relies on a pair of other libraries that I've written:
[classwork.pp][] and [parsing.pp][].

The documentation seen here was generated with a modified version of [PasDoc][].
The [classes hierarchy graph](GVClasses.svg) was generated using [GraphViz][].

## License ##
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License or, at your option, any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABLITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.

## References ##
_(Not visible in the parsed HTML output because they've turned into the 
various links used in the text above.)_

[Author]: http://www.circusmachina.com/gladius/Profiles/Michael
[Home page]: http://docs.circusmachina.com/libslinearc/
[Source]: https://github.com/circusmachina/libs-linearc
[Source documentation]: http://docs.circusmachina.com/libs/linearc/
[Bug tracker]: http://monkeywrench.circusmachina.com

[Free Pascal]: http://www.freepascal.org
[PasDoc]: http://pasdoc.sourceforge.net
[GraphViz]: http://www.graphviz.org

[1]: http://www.gnu.org/prep/standards/html_node/Command_002dLine-Interfaces.html#Command_002dLine-Interfaces
[2]: https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html

[classwork.pp]: http://docs.circusmachina.com/libs/classwork/
[parsing.pp]: http://docs.circusmachina.com/libs/parsing/

[ALinearCParser]: http://docs.circusmachina.com/libs/linearc/linearc.ALinearCParser.html
[ALinearCConsoleApplication]: http://docs.circusmachina.com/libs/linearc/linearc.ALinearCConsoleApplication.html
[articles]: http://www.circusmachina.com/gladius/Tags/LinearC

