### trie

An extremely small and very fast trie implementation. Keys are binary strings, and the span of the trie can be set from 1 to 8.
A trie with a span of one is essentially a bitwise binary tree, which this lib can be used as.
It is also relatively memory efficient with an average of 20 - 40 bytes of overhead per key, which is low for a trie.