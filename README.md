# Bad Spelling Proxy
A proxy that randomly mispells words in HTML and plaintext documents.


## Usage
 1. Compile the proxy with `g++ proxy.cpp -lpthread -o proxy`
 2. Run proxy with `./proxy PORN_NUM MISTAKE_FREQ`. Where PORT_NUM is an integer that the proxy will be binded to, and MISTAKE_FREQ is an integer likelihood that there will be a spelling mistake (i.e. MISTAKE_FREQ = 4 will be a 1/4 chance of a mistake).
 3. Set proxy. In Firefox go to preferences and set as localhost and the chosen PORT_NUM

## Limitations and Bugs
 - Does not support HTTPS
 - Not tested with HTTP 1.0
 