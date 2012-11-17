Hi,

This is a light-weight directed graph implementation. 

It stores arcs (directed edges) in linked lists.

It supports weighted graphs, self-pointing arcs, multiple arcs from one node to another.

I also added shortest path finding (non-weighted) by using Dijkstra's algorithm. I'll add weighted version too. Maybe I can add A* after that.

More functionality would be nice, like connectivity checking, depth-first search etc. I don't know if I'll have time for those though.

I wrote two blog posts about the design and implementation, you can consult them for documentation. [Digraph Code - I] (http://aciliketcap.blogspot.com/2012/11/digraph-code-i.html) and [Digraph Code - II](http://aciliketcap.blogspot.com/2012/11/digraph-code-ii_18.html) I guess comments inside the source should be enough for the rest.

If you want to see shortest path finding at work, check out my other repo [cpp-codelets](https://github.com/aciliketcap/cpp-codelets). map.cpp program opens a maze in text file and converts it to digraph format. Then solves the maze.