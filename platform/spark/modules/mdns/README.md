brewblox-mdns
=============

Multicast DNS and DNS-SD for Particle devices.

Forked from Mark Hornsby's implementation here: https://github.com/mrhornsby/spark-core-mdns.git

The original implementation by Mark consumed too much memory for me, so I set out to refactor it to be more memory efficient.
I ended up rewriting the majority of the code, to use different data structures and more STL.
In my own application, this rewrite reduced the memory use by about 1200 bytes.

With Wireshark I compared packets from the original library with my implementation and it should be idential.
The only difference is that for empty text records I include an empty string, as recommended by the spec.

The major changes in this implementation are:

#### Records are directly found by their label.
The old library used a vector of records and a map of labels, with many pointers back and forth. The structure seemed needlessly complex to me.

This meant that for many records, you would have a string index into the map and an almost identical string as label in the record.
I thought this didn't make much sense. Why first compare the query string to the map indices, instead of directly comparing it to the labels of the records?
Searching a vector is also faster and smaller than a map for only 10 entries.

I removed the map for labels entirely and made the labels part of the record. This means that we only have a single datastructure now.
Well actually, 2. For names that are re-used for the protocol I use a vector of Metarecords: `local`, `_tcp`, `_udp`, `_services`, `_dns-sd`.

The logic for which records are included has now also moved to the records themselves, with the SRV record handling most of the work for other records.
The logic of this could be cleaned up a bit perhaps.

##### Labels now point to other records for name compression
The old implementation used a linked list of labels to re-use parts of labels. At the same time though, the labels were full length strings as index to the map.
I changed the implementation to let labels point to another record for the remainder of its label.

##### No unnecessary copying of UDP buffer
The UPD class has an internal 512 byte buffer. This buffer was copied to another 512 byte buffer before processing the data. This was unnecessary. The MDNS class now uses the UDP buffer directly, for sending, receiving and processing MDNS packets.

##### std:string instead of Wiring String
I much prefer std over Arduino implementations.

##### No checks for instantiation parameters
The old implementation checked whether the labels length was under 63 and whether it used only allowed characters. I think providing valid arguments is responsibility of the programmer. A static assertion would be okay, but a runtime check is a waste of resources.


Feedback, bug reports and especially pull requests are welcome!