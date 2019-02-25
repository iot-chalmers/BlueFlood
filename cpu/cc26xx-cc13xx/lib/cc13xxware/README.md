This is a git-versioned subset and slightly modified version of TI's [CC13xxware](http://www.ti.com/tool/cc13xxWare).

This repository's sole purpose is to provide CC13xxware as a submodule for the [Contiki Operating System](https://github.com/contiki-os/contiki/).

New versions will only appear in this repository only if and when Contiki's CC13x port needs to use them.

Modifications (for current and upcoming versions):

* Only files used by Contiki are included here. Documentation and files related to IAR and CCS have been removed.
* Line-endings have been converted to Unix
* File permissions have been changed to 644

All sources are and will remain otherwise intact.

If you need CC13xxware for any other purpose, do not clone this repository. A full version of the software can be downloaded by following the above link. A history of previous versions can be found in [TI's relevant wiki page](http://processors.wiki.ti.com/index.php/CC13xxWare).

Do not open pulls / issues on this repository, unless they are immediately related to using CC13xxware with Contiki.
