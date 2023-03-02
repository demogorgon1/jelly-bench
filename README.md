# jelly-bench
[![Build](https://github.com/demogorgon1/jelly-bench/actions/workflows/cmake.yml/badge.svg)](https://github.com/demogorgon1/jelly-bench/actions/workflows/cmake.yml)

Comparing [jelly](https://github.com/demogorgon1/jelly) performance with other databases. As for now this is limited to two other embeddable databases: 

* RocksDB, which as a LSM storage engine is conceptually fairly similar to jelly, except it's much more involved and does a lot of different things. 
* SQLite, which is pretty much the gold standard for embeddable databases. Unlike RocksDB and jelly, this is more of a traditional "one size fits all" storage engine and not specifically designed for writing a lot of blobs.

In a distributed storage system, either of these could be used in place of jelly blob nodes, which is why I want to make some comparative benchmarks. There are other embeddable databases, but these seem to be the most common ones.

All tests below have been run in a single-core ubuntu VM on my laptop, using an SSD. Obviously this isn't ideal and it would interesting to run them in a more realistic environment.

```
$ lsb_release -d && uname -s -r && lscpu
Description:    Ubuntu 20.04.4 LTS
Linux 5.4.0-139-generic
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   48 bits physical, 48 bits virtual
CPU(s):                          1
On-line CPU(s) list:             0
Thread(s) per core:              1
Core(s) per socket:              1
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       AuthenticAMD
CPU family:                      23
Model:                           96
Model name:                      AMD Ryzen 7 PRO 4750U with Radeon Graphics
Stepping:                        1
CPU MHz:                         1696.812
BogoMIPS:                        3393.62
Hypervisor vendor:               KVM
Virtualization type:             full
L1d cache:                       32 KiB
L1i cache:                       32 KiB
L2 cache:                        512 KiB
L3 cache:                        8 MiB
```

## Single node write benchmark
The goal of this benchmark is not find maximum writes per second attainable, but to measure resource consumtion at various steady workloads.

```
jelly-bench -write_backend <backend> -write_rate <rate> -write_run_seconds 120 -blob_key_count 20000 -compression_method none -rocksdb_compression none 
```

* Test runs for 2 minutes where it will write the specified number of blobs per second at a steady rate. 
* Every write will be to one of 20000 unique keys, picked randomly. This can be seen as the number of concurrent clients.
* Each blob is 5-15 KB and contains bunch of random data.
* Compression is disabled.
* All storage engines configured to flushing their WALs every 500 ms.
* Only a single writing thread used.
* Write rates from 10 to 10000 blobs per second.

The test system wasn't able to handle more than 10000 blobs per second at a steady rate using any storage engine. If using multiple CPU cores, both Jelly and RocksDB could be configured to scale beyond this, while SQLite would have problems.

### CPU usage
Blob writes/second|Jelly|SQLite|RocksDB
-|-|-|-
10|1.68%|1.66%|1.68%
100|1.87%|2.4%|2.89%
1000|4.92%|9.39%|11.67%
5000|21.00%|29.11%|44.14%
10000|35.02%|44.44%|62.73%

### Avg. system write calls/second
Blob writes/second|Jelly|SQLite|RocksDB
-|-|-|-
10|3|67|24
100|3|676|232
1000|44|11078|2320
5000|214|54773|11601
10000|362|101166|19785

Jelly is very aggressive about making very large write batches so number of system write calls will be very low. In general it's a function of number of bytes written and blob write rate has no direct effect on this. 

### Avg. disk throughput
Blob writes/second|Logical throughput|Jelly|SQLite|RocksDB
-|-|-|-|-
10|0.1 MB/s|0.1 MB/s|0.17 MB/s|0.1 MB/s
100|0.95 MB/s|0.95 MB/s|1.7 MB/s|1.39 MB/s
1000|9.54 MB/s|21.01 MB/s|22.7 MB/s|23.25 MB/s
5000|47.69 MB/s|105.81 MB/s|110.33 MB/s|122.64 MB/s
10000|95.36 MB/s|180.18 MB/s|208.43 MB/s|177.53 MB/s

The _logical throughput_ column denotes the size of the blobs written, while the other columns denote actual bytes written to disk as reported by the OS.
Note that for SQLite this is mostly linear, while for Jelly and RocksDB it's not due to store/sstable flushes and compactions. It's hard to predict exactly when these happens, so numbers look more random and are largely a matter of tuning.

