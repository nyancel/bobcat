# Bobcat
a webserver in C

this is very much a work in progress, the idea is to have a
somewhat decent yet very simple web-server framework focused on
static files, templating and forms to build traditional 
SSR MPA applications. as well as handling things like file-transfers.

## Roadmap
1. Threading ✅
2. Request parsing 🟡
3. Handler mapping 🟡
4. Response generation ❌

-- maybe more to come?

## wishlist
**HOT RELOAD**: I would love to include things like support for 
hot-reloading, but im not sure exactly how to do that from a library 
perspective, that might be more of an application level thing.

**JSON**: i might implement the c-json lib in order to have access
to parsing json data from the request body for building API's, althouhg
that is not the primary focus at the moment.

## things i need help with
does it leak? does it leak a lot?
its really bad isnt it? memmory management is not my strongest suit,
and while i try to keep a track of stuff i realize that there are 
probably several places where bobcat isnt really "house-clean".

so if you're comfy with valgrind or have some other tools up your belt 
feel free to help sort out threading and memmor management where 
possible, its much apreciated ^~^

## known issues

* crashes on unmapped URL
* all "foreign" HTTP-VERBS are treated as bc_NONE


**todo:**
- [x] handler: threaded tasks
- [ ] handler: hot reloading
- [x] handler: map functions to rules
- [x] request parsing: methods
- [x] request parsing: uri - path
- [ ] request parsing: uri - args
- [ ] request parsing: headers
- [ ] request parsing: body - json
- [ ] request parsing: body - form
- [ ] response construction: head - statuscode
- [ ] response construction: head - headers
- [ ] response construction: head - cookie
- [ ] response construction: body - html
- [ ] response construction: body - json
