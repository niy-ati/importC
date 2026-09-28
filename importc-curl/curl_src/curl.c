/* Zero-wrapper import shim: re-exports libcurl's real public API via ImportC.
   Consumers write `import curl;` and get libcurl's actual declarations,
   nothing hand-written in between. */
#include <curl/curl.h>
