import curl;
import core.stdc.stdio : printf;
import core.stdc.string : strlen;

void main()
{
    const(char)* v = curl_version();
    assert(v !is null && strlen(v) > 0, "curl_version() returned nothing");
    printf("curl_version(): %s\n", v);

    // CURL_GLOBAL_DEFAULT is `#define CURL_GLOBAL_DEFAULT CURL_GLOBAL_ALL`, a
    // bare-identifier macro alias with no parens at all. ImportC's macro
    // converter currently only handles bare identifier(args) calls (see
    // PR #22347), not a plain "#define NEW OLD" alias, so it silently drops
    // this one. Using the aliased name directly until that's fixed.
    CURLcode initrc = curl_global_init(CURL_GLOBAL_ALL);
    assert(initrc == CURLcode.CURLE_OK, "curl_global_init failed");

    CURL* handle = curl_easy_init();
    assert(handle !is null, "curl_easy_init() returned null");

    // Real API calls, no network access: set a couple of options and read them back via getinfo.
    curl_easy_setopt(handle, CURLoption.CURLOPT_URL, "https://example.invalid/".ptr);
    curl_easy_setopt(handle, CURLoption.CURLOPT_TIMEOUT, 1L);

    char* effectiveUrl;
    curl_easy_getinfo(handle, CURLINFO.CURLINFO_EFFECTIVE_URL, &effectiveUrl);
    assert(effectiveUrl !is null, "CURLINFO_EFFECTIVE_URL was not set");
    printf("effective url readback: %s\n", effectiveUrl);

    curl_easy_cleanup(handle);
    curl_global_cleanup();

    printf("CURL ZERO-WRAPPER SMOKE TEST PASSED\n");
}
