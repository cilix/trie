### trie

An extremely small and very fast trie implementation. Keys are binary strings, and the span of the trie can be set from 1 to 8.
```c
/* to change trie span, both variables below must be changed */
#define TRIE_SPAN  5 /* trie span */
#define TRIE_LIMIT 32 /* 2 ^ span */
```
The trie can also be configured to use chaining instead of a fixed size array of nodes
```c
#define TRIE_USE_CHAINING
```
A trie with a span of one is essentially a bitwise binary tree, which this lib can be used as.
It is also relatively memory efficient with an average of 50 - 70 bytes of overhead per key, which is low for a simple trie.