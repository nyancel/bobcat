# Bobcat
a webserver in C

this is very much a work in progress, the idea is to have a
somewhat decent yet very simple web-server framework focused on
static files, templating and forms to build traditional 
SSR MPA applications. as well as handling things like file-transfers.

## Threading
Threading is done using pthread, each request
gets the fd from the accept() calll on the base socket,
these accepted fd's are passed alongside void* handler
descriptions to a detached thread in order to provide async
request handling

## Hot Reloading
Hot reloading van be built on POSIX opertaions and
has been a thing since forever. even for compiled languages
like C, using dlopen and dlsym to find void* referencing 
.so functions.

## route-mapping
Route-mapping would be done using a dictlist, or set of dictlists
for each relevant method. passing the URI to the list, and
retrieving the function-pointer contained in the data.

**todo:**
- [x] handler: threaded tasks
- [ ] handler: hot reloading? 🤔
- [ ] request parsing: methods
- [ ] request parsing: uri - path
- [ ] request parsing: uri - args
- [ ] request parsing: headers
- [ ] request parsing: body - json
- [ ] request parsing: body - form
- [ ] response construction: head - statuscode
- [ ] response construction: head - headers
- [ ] response construction: head - cookie? 🤔
- [ ] response construction: body - html
- [ ] response construction: body - json
