/*
 * capi - C Api
 * js function that will be used in C++ code
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#implement-a-c-api-in-javascript
 */

addToLibrary({
    $backend_api: "'http://localhost:3000'",
    // $backend_api: "'https://moses-fishing-improving-peas.trycloudflare.com'",
    fetchFrames__async: 'auto',
    fetchFrames__deps: ['$backend_api'],
    fetchFrames: async (offset, buffer_ptr, length) => {
        const buf = await fetch(`${backend_api}/fragments?offset=${offset}&length=${length}`)
            .then((r) => r.blob())
            .then((blob) => blob.arrayBuffer());

        const uint8View = new Uint8Array(buf);
        HEAPU8.set(uint8View, buffer_ptr);
    },
    getTotalSize__async: 'auto',
    getTotalSize__deps: ['$backend_api'],
    getTotalSize: async () => {
        return await fetch(`${backend_api}/total_size`).then((r) => r.json());
    },
});
