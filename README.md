# jelly-bench
Comparing [jelly](https://github.com/demogorgon1/jelly) performance with other databases. As for now this is limited to two other embeddable databases: 

* RocksDB, which as a LSM storage engine is conceptually fairly similar to jelly, except it's much more involved and does a lot of different things. 
* SQLite, which is pretty much the gold standard for embeddable databases. Unlike RocksDB and jelly, this is more of a traditional "one size fits all" storage engine and not specifically designed for writing a lot of blobs.

In a distributed storage system, either of these could be used in place of jelly blob nodes, which is why I want to make some comparative benchmarks. There are other embeddable databases, but these seem to be the most common ones.

## Single node write benchmark
The goal of this benchmark is not find maximum writes per second attainable, but to measure resource consumtion at various steady workloads.

```
jelly-bench -write_test_run 120 -blob_key_count 20000 -compression_method none -rocksdb_compression none -write_backend <backend>
```

* Test runs for 2 minutes where it will write the specified number of blobs per second at a steady rate. 
* Every write will be to one of 20000 unique keys, picked randomly. 
* Each blob is 5-15 KB and contains bunch of random data.
* Compression is disabled.
* All storage engines configured to flushing their WALs every 500 ms.
* Only a single writing thread used.

All tests below have been run in a ubuntu VM on my laptop, using an SSD. Obviously this isn't ideal and it would interesting to run them in a more realistic environment.



