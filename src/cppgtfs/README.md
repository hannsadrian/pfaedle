# cppgtfs

Parses GTFS feeds into memory. Extensive validation is applied. Currently used by LOOM, pfaedle, and some student projects. Not perfect, but does the job.

## Usage

```cpp
#include "ad/cppgtfs/Parser.h"

// [...]

ad::cppgtfs::Parser parser("path/to/gtfs/folder");
ad::cppgtfs::gtfs::Feed feed;

bool result = parser.parse(&feed);
```

## Optional dependencies

- LibZip
