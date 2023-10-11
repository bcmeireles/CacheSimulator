# CacheSimulator

### Introduction
The goal of this assignment is the development of a complete memory hierarchy. To achieve this, the students will complete the base code provided by the faculty to implement the L1 cache and L2 cache from scratch. Finally, the students will integrate these components to form a complete memory hierarchy.

## Tasks
All the programs developed should be configurable via the Cache.h header, meaning the stu- dents cannot delete any of the constants defined in the file.
The memory hierarchy uses a write back policy and a LRU block replacement approach.

### Directly-Mapped L1 Cache
In this task, you must develop a memory hierarchy with an L1 direct mapped cache with multiple lines with the parameters provided in the constant file.
The students must copy SimpleCache.c, SimpleCache.h to L1Cache.c, L1Cache.h (respectively), and modify the code to support multiple lines.

### Directly-Mapped L2 Cache
In this task, you must develop an direct mapped L2 cache with the parameters specified in the constant file. In the resulting memory hierarchy of this task you must use the Directly-Mapped L1 Cache developed in the previous task

### 2-Way L2 Cache
In this task, you must change the L2 cache developed in the previous task and modify it to a two way set-associate cache. Note that, the other parameters remain the same, in particular the L2Size value.
In the resulting memory hierarchy of this task you must use the Directly-Mapped L1 Cache developed in task