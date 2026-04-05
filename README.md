## routerInfoParser

**routerInfoParser** is a specialized C++ utility designed to deconstruct and audit I2P NetDB RouterInfo files.

### Core Functionality

*   Decompression: Uses zlib to handle the Gzip-compressed binary format of .dat files found in the netDb storage.
*   Cryptographic Verification: Extracts the 384-byte identity block and uses OpenSSL SHA256 to calculate the Router Hash, which serves as the unique global identifier for nodes on the network.
*   Identity Mapping: Decodes Base64-encoded Router IDs using I2P’s specific alphabet (-~ instead of +/).
*   Metadata Extraction: Recursively parses I2P Mappings (key-value pairs) to reveal:
    *   Transport Protocols: Detailed configurations for `NTCP2` and `SSU2`.
    *   Network Topology: Global stats like knownRouters and knownLeaseSets.
    *   Temporal Audit: Converts raw Unix timestamps into human-readable UTC format for both the publication date and introductory endpoint expirations (`iexp`).


### Prerequisites

Before compiling **routerInfoParser**, ensure you have the following libraries installed on your system:

*   **zlib**: For decompressing Gzip-encoded `.dat` files.
*   **OpenSSL**: For calculating SHA-256 hashes of router identities.
*   **C++11 Compiler** (or newer): Such as `g++` or `clang++`.

#### Installing dependencies (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install build-essential zlib1g-dev libssl-dev
```

### Build Instructions

```bash
g++ routerInfoParser.cpp -o routerInfoParser -lz -lcrypto
```

### Running the Parser
Pass the path to an I2P routerInfo file as the first argument:

```bash
./routerInfoParser /var/lib/i2pd/netDb/ro/routerInfo-example.dat
```
Example output:

```bash
FILE: /var/lib/i2pd/netDb/ro/routerInfo-o3i0gbwiWt0HjRKv~ydnTG0Qky3P7gLotSr6pLZE8FQ=.dat
ROUTER ID (Base64): o3i0gbwiWt0HjRKv~ydnTG0Qky3P7gLotSr6pLZE8FQ=
ROUTER HASH (Hex): a378b481bc225add078d12afff27674c6d10932dcfee02e8b52afaa4b644f054
Published (UTC): Sat Feb 21 15:26:24 2026

  Address 1 (NTCP2) Cost: 3
    [Address Options]
      host = 47.234.215.81
      i = TaLhlu-xgHMm27fj9PGrhA==
      port = 10700
      s = C47lBgMetWFj4iH1Nvv7I2mJP7~aLjqBki8hja6goWw=
      v = 2

  Address 2 (NTCP2) Cost: 14
    [Address Options]
      caps = 6
      s = C47lBgMetWFj4iH1Nvv7I2mJP7~aLjqBki8hja6goWw=
      v = 2

  Address 3 (SSU2) Cost: 8
    [Address Options]
      caps = BC
      host = 47.234.215.81
      i = kAKSUCenog8bMUtyBWd~lrebKKJBnIDS~PJIx2LRzCI=
      port = 10700
      s = CaUDpY55nhCLdmoIKUBjsL~ow3gpmEahAEVjnsS1ISs=
      v = 2

  Address 4 (SSU2) Cost: 15
    [Address Options]
      caps = 6
      i = kAKSUCenog8bMUtyBWd~lrebKKJBnIDS~PJIx2LRzCI=
      iexp0 = 1771690667 (UTC: Sat Feb 21 16:17:47 2026)
      iexp1 = 1771690851 (UTC: Sat Feb 21 16:20:51 2026)
      iexp2 = 1771691890 (UTC: Sat Feb 21 16:38:10 2026)
      ih0 = pfgylxhh6A0EPEpwlLcujrCJ0qeEd6doevZeZZ-Aczs=
      ih1 = uO7bxJuPdfodKwKtCNiJzGC7d~Sqfh~cLpa9RWbc0uk=
      ih2 = rDta877Z57W9JXAISNTNhd~5bAgrQPMU5IoPiVQ6ziw=
      itag0 = 3219335955
      itag1 = 1064299866
      itag2 = 3359627914
      mtu = 1500
      s = CaUDpY55nhCLdmoIKUBjsL~ow3gpmEahAEVjnsS1ISs=
      v = 2
    [Router Metadata & Capabilities]
      caps = XfR
      netId = 2
      netdb.knownLeaseSets = 163
      netdb.knownRouters = 31555
      router.version = 0.9.64
```
