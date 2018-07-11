# chapter 27: IP options

IPv4 and IPv6  
  
```
+--------+-----------+--------------------+
|  IPv6  | Extension |    Upper layer     |
| header |  headers  | protocol data unit |
+--------+-----------+--------------------+
|        |                                |
|        |----------> payload <-----------|
|-----------> IPv6 pakcet <---------------|

IPv6 extension header
+------+--------+--------------+
| type | length | option value |
+------+--------+--------------+
   1       1        length

exmaple
+-----------------+ +--------------+
|   IPv6 header   | |              |
| next header = 6 | | TCP segement |
|      (TCP)      | |              |
+-----------------+ +--------------+

+------------------+ +-----------------+ +-------------+
|   IPv6 header    | | Routing header  | |             |
| next header = 43 | | next header = 6 | | TCP segment |
|    (routing)     | |      (TCP)      | |             |
+------------------+ +-----------------+ +-------------+

+------------------+ +-----------------+ +-------------+
|   IPv6 header    | | Fragment header | |             |
| next header = 43 | | next header = 6 | | TCP segment |
|    (routing)     | |      (TCP)      | |  fragment   |
+------------------+ +-----------------+ +-------------+

Extension Header Order
1. Hop-by-Hop Options order
2. Destination Options header(for intermadiate destinations when the routing header is present)
3. Routing header
4. Fragment header
5. Authentication header
6. Encapsulating Security Payload header
7. Destination Options header (for the final destination)

```

see https://tools.ietf.org/html/rfc2460 for more detail.  
  
```
IPv4 header
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IPv6 header
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version| Traffic Class |           Flow Label                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Payload Length        |  Next Header  |   Hop Limit   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                                                               +
|                                                               |
+                         Source Address                        +
|                                                               |
+                                                               +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
+                                                               +
|                                                               |
+                      Destination Address                      +
|                                                               |
+                                                               +
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

comparing the IPv4 and IPv6 headers
```
+------------------------+--------------------------------------------------+
|        version         |  same field but with different version numbers   |
+------------------------+--------------------------------------------------+
| Internet Header Length | Removed in IPv6. IPv6 does not include a header  |
|                        | length field because the IPv6 header is always a |
|                        | fixed size of 40 bytes. Each extension header is |
|                        |  either a fixed size or indicate its own size.   |
+------------------------+--------------------------------------------------+
|    Type of Service     |    Replaced by the IPv6 traffic class field.     |
+------------------------+--------------------------------------------------+
|      Total Length      |    Replaced by the IPv6 payload length field.    |
|                        |   which only indicates the size of the payload   |
+------------------------+--------------------------------------------------+
|     Identification     |  Removed In IPv6. Fragmentation information is   |
|  Fragmentation Flags   | not included in the IPv6 header. It is contained |
|  Fragmentation Offset  |         in a Fragment extension header.          |
+------------------------+--------------------------------------------------+
|      Time to Live      |      Replaced by the IPv6 Hop Limit field.       |
+------------------------+--------------------------------------------------+
|        Protocol        |     Replaced by the IPv6 Next Header field.      |
+------------------------+--------------------------------------------------+
|    Header Checksum     |    Removed in IPv6. In IPv6, bit-level error     |
|                        | detection of the entire IPv6 packet is performed |
|                        |                by the link layer.                |
+------------------------+--------------------------------------------------+
|     Source Address     | IPv6 128 bits length.                            |
+------------------------+--------------------------------------------------+
|  Destination Address   | IPv6 128 bits length.                            |
+------------------------+--------------------------------------------------+
|        Options         |   Remove in IPv6. IPv4 options are replaced by   |
|                        |             IPv6 extension headers.              |
+------------------------+--------------------------------------------------+
```
