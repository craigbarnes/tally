Tally
=====

A simple command-line SLOC counter.

Requirements
------------

* C99 compiler
* [GNU Make] 4.0+
* [GNU gperf] 3.1+
* [Ragel]

**Note:** Tally currently only works with GNU libc. This is only for
convenience during early development. The intention is to eventually be
portable to any [POSIX.1-2008] compliant OS.

Installation
------------

    git clone https://gitlab.com/craigbarnes/tally.git
    cd tally
    make && sudo make install

License
-------

Copyright (C) 2014-2025 Craig Barnes

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU [General Public License version 2] as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License version 2 for more details.


[General Public License version 2]: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
[GNU Make]: https://www.gnu.org/software/make/
[GNU gperf]: https://www.gnu.org/software/gperf/
[Ragel]: https://www.colm.net/open-source/ragel/
[POSIX.1-2008]: https://pubs.opengroup.org/onlinepubs/9699919799/
