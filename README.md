# Bad Spelling Proxy
A proxy that randomly mispells words in HTML and plaintext documents.


## Usage
 1. Compile the proxy with `g++ proxy.cpp -lpthread -o proxy`
 2. Run proxy with `./proxy`
 3. Set proxy. In Firefox go to preferences and set as localhost and port 8080
 4. Disable favicons by going to about:config and searching for `browser.chrome.site_icons`

## Limitations and Bugs
 - Does not support HTTPS
 - Does not work with favicons (probably a consequence of HTTPS non support)